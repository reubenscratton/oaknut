//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE
#include <oaknut.h>
#if RENDERER_METAL
#if PLATFORM_IOS
 #include "../ios/NativeView.h"
#else
 #include "../macos/NativeView.h"
#endif


#define SAMPLER_NONE 0
#define SAMPLER_TEXTURE2D_A8 1
#define SAMPLER_TEXTURE2D_RGBA 2

static id<MTLDevice> s_device;


class MetalTexture : public Texture {
public:
    id<MTLTexture> _tex;
    id<MTLSamplerState> _sampler;
    
    MetalTexture(Renderer* renderer) : Texture(renderer) {
        _format = BITMAPFORMAT_BGRA32; // to get to MTLPixelFormatBGRA8Unorm;
    }
    
    void unload() {
        _tex = NULL;
        _sampler = NULL;
    }
    void resize(int width, int height) override {
        MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
        switch (_format) {
            case BITMAPFORMAT_RGBA32:
                pixelFormat = MTLPixelFormatRGBA8Unorm;
                break;
            case BITMAPFORMAT_BGRA32:
                pixelFormat = MTLPixelFormatBGRA8Unorm;
                break;
            case BITMAPFORMAT_RGB24:
                assert(0); // Metal does not support this format, you must use 32bpp
                break;
            case BITMAPFORMAT_A8:
                pixelFormat = MTLPixelFormatA8Unorm;
                break;
        }
        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                        width:width
                                                                                       height:height
                                                                                    mipmapped:_maxMipMapLevel>0];
        desc.mipmapLevelCount = _maxMipMapLevel+1;
        desc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        _tex = [s_device newTextureWithDescriptor:desc];
            
        MTLSamplerDescriptor* samplerDesc = [MTLSamplerDescriptor new];
        samplerDesc.minFilter = _minFilterLinear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
        samplerDesc.magFilter = _magFilterLinear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
        if (_maxMipMapLevel<=0) {
            samplerDesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
        } else {
            samplerDesc.mipFilter = _mipFilterLinear ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
        }
        _sampler = [s_device newSamplerStateWithDescriptor:samplerDesc];

    }

    int getSampler() override {
        return (_format==BITMAPFORMAT_A8) ? SAMPLER_TEXTURE2D_A8 : SAMPLER_TEXTURE2D_RGBA;
    }

    
};


class MetalSurface : public Surface {
public:
    MTLViewport _viewport;
    id<MTLTexture> getTexture() {
        return _texture.as<MetalTexture>()->_tex;
    };

    MetalSurface(Renderer* renderer, bool isPrivate) : Surface(renderer, isPrivate) {
        _viewport.originX = 0;
        _viewport.originY = 0;
        _viewport.znear = 0;
        _viewport.zfar = 1;
        _texture = new MetalTexture(renderer);
    }
    
    void setSize(const SIZE& size) override {
        Surface::setSize(size);
        _viewport.width = size.width;
        // NB: Flip Y for offscreen surface texture
        _viewport.height = _isPrivate ? (-size.height) : size.height;
        _viewport.originY = _isPrivate ? size.height : 0;
        if (!_isPrivate) {
            return;
        }
        _texture->resize(size.width, size.height);
    }
};

static void alignInt(int32_t& i, int32_t alignment) {
    int32_t mod = i % alignment;
    if (mod) {
        i += (alignment-mod);
    }
}

struct ShaderState {
    id<MTLFunction> _vertexShader;
    id<MTLFunction> _fragShader;
    id<MTLRenderPipelineState> _pipelineState[3]; // one per blend mode
    bytearray _vertexUniformData;
    bytearray _fragUniformData;
    bool _vertexUniformsValid;
    bool _fragUniformsValid;

    ShaderState(Shader* shader, id<MTLDevice> device) {
        
        // Work out the uniform structs, respecting alignments as per Metal Language spec
        int32_t cbVert=0, cbFrag=0;
        int32_t vertMaxAlign=0, fragMaxAlign=0;
        for (auto& uniform: shader->_uniforms) {
            int32_t alignment = 4;
            switch (uniform.type) {
                case Shader::Uniform::Type::Int1: alignment=4; break;
                case Shader::Uniform::Type::Float1: alignment=4; break;
                case Shader::Uniform::Type::Float2: alignment=8; break;
                case Shader::Uniform::Type::Float4: alignment=16; break;
                case Shader::Uniform::Type::Matrix4: alignment=16; break;
                default: assert(0);
            }
            int32_t& cb = (uniform.usage == Shader::Uniform::Fragment) ? cbFrag : cbVert;
            int32_t& maxAlign = (uniform.usage == Shader::Uniform::Fragment) ? fragMaxAlign : vertMaxAlign;
            maxAlign = MAX(maxAlign, alignment);
            alignInt(cb, alignment);
            uniform.offset = cb;
            cb += uniform.length();
        }
        alignInt(cbVert, vertMaxAlign);
        if (fragMaxAlign) {
            alignInt(cbFrag, fragMaxAlign);
        }
        _vertexUniformData.resize(cbVert);
        _fragUniformData.resize(cbFrag);

        string s = shader->getVertexSource();
        NSError* error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:[NSString stringWithUTF8String:s.data()] options:nil error:&error];
        assert(library);
        _vertexShader = [library newFunctionWithName: @"vertex_shader"];
        _fragShader = [library newFunctionWithName: @"frag_shader"];
        
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineStateDescriptor.vertexFunction = _vertexShader;
        pipelineStateDescriptor.fragmentFunction = _fragShader;
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineStateDescriptor.colorAttachments[0].blendingEnabled = NO;
        _pipelineState[BLENDMODE_NONE] = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                                error:&error];
        
        pipelineStateDescriptor.colorAttachments[0].blendingEnabled = YES;
        pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        _pipelineState[BLENDMODE_NORMAL] = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                                  error:&error];
        
        pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
        pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        _pipelineState[BLENDMODE_PREMULTIPLIED] = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                                         error:&error];

    }
    
};


static string getUniformFields(Shader* shader, Shader::Uniform::Usage usage) {
    string s="";
    for (auto& uniform : shader->_uniforms) {
        if (usage == uniform.usage) {
            switch (uniform.type) {
                case Shader::Uniform::Int1: s+="int"; break;
                case Shader::Uniform::Float1: s+="float"; break;
                case Shader::Uniform::Float2: s+="float2"; break;
                case Shader::Uniform::Float4: s+="float4"; break;
                case Shader::Uniform::Matrix4: s+="float4x4"; break;
            }
            s+= " ";
            s+= uniform.name;
            s+= ";\n";
        }
    }
    return s;
}

static string getShaderSource(Shader* shader, bool useTexCoordsAttrib, bool useColorAttrib) {
    string s =
    "using namespace metal;\n"
    "\n"
    "struct VertexInput {\n"
    "   float2 position [[attribute(0)]];\n"
    "   float2 texcoord [[attribute(1)]];\n"
    "   uint color [[attribute(2)]];\n"
    "   float unused1;\n"
    "   float unused2;\n"
    "   float unused3;\n"
    "};\n"
    "struct VertexOutput {\n"
    "   float4 position [[position]];\n";
    if (useTexCoordsAttrib) {
        s+= "   float2 texcoord;\n";
    }
    if (useColorAttrib) {
        s+= "   half4 color;\n";
    }
    s+= "};\n"
        "struct VertexUniforms {\n";
    s+= getUniformFields(shader, Shader::Uniform::Usage::Vertex);
    s+= "};\n";
    s+= "vertex VertexOutput vertex_shader(uint vid [[vertex_id]],\n"
    "                                  constant VertexInput* v_in [[buffer(0)]],\n"
    "                                  constant VertexUniforms* uniforms [[buffer(1)]]) {\n"
    "   VertexOutput output;\n"
    "   output.position = uniforms->mvp * float4(v_in[vid].position,0,1);\n";
    if (useTexCoordsAttrib) {
        s += "   output.texcoord = v_in[vid].texcoord;\n";
    }
    if (useColorAttrib) {
        s+= "   output.color = unpack_unorm4x8_to_half(v_in[vid].color);\n";
    }
    s+= "   return output;\n"
        "}\n";
    return s;
}

string StandardShader::getVertexSource() {
    bool useTexCoords = false;
    bool useTexSampler = false;
    int roundRect = _features.roundRect;
    if (roundRect) {
        useTexCoords = true; // we don't use the sampler, we just want the texcoord attributes, which are
        // not actually texture coords, v_texcoords is x-dist and y-dist from quad centre
    }
    if (_features.sampler0 != SAMPLER_NONE) {
        useTexSampler = true;
        useTexCoords = true;
    }
    
    string s = getShaderSource(this, useTexCoords, true);

    
    // Frag shader
    string fragUniforms = getUniformFields(Uniform::Usage::Fragment);
    if (fragUniforms.length() > 0) {
        s+= "struct FragUniforms {\n";
        s+= fragUniforms;
        s+= "};\n";
    }
    s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]\n";
    if (fragUniforms.length() > 0) {
       s += ",constant FragUniforms* uniforms [[buffer(0)]]\n";
    }
    if (useTexSampler) {
        s+= ",texture2d<half> colorTexture [[ texture(0) ]]\n";
        s+= ",sampler textureSampler [[ sampler(0) ]]";
    }
    s+= ") {\n";
    
    if (!useTexSampler) {
        s += "half4 c = in.color;\n";
    } else {
        s += "half4 c = colorTexture.sample(textureSampler, in.texcoord);\n";
        if (_features.tint) {
            s += "c.rgb = in.color.rgb;\n";
        }
    }
    
    if (roundRect) {
        if (roundRect == SHADER_ROUNDRECT_1) {
            s += "    float2 b = uniforms->u.xy - float2(uniforms->radius); \n"
                 "    float dist = length(max(abs(in.texcoord)-b, 0.0)) - uniforms->radius  - 0.5;\n";
        }
        else if (roundRect == SHADER_ROUNDRECT_2H) {
            // branchless selection of radius=r.x if on left side of quad or radius=r.y on right side
            s += "   float2 size = uniforms->u.xy; \n"
                "   float2 r = uniforms->radii.xw\n;" // TODO: this is specific to left|right config
                "   float s=step(in.texcoord.x,0.0);\n"
                "   float radius = s*r.x + (1.0-s)*r.y;\n"
                "   size -= float2(radius);\n"
                "   float2 d = abs(in.texcoord) - size;\n"
                "   float dist = min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;\n";
        }
        s +=    "   half4 col = half4(uniforms->strokeColor);\n"
                "   col.a = mix(0.0, uniforms->strokeColor.a, clamp(-dist, 0.0, 1.0));\n"   // outer edge blend
                "   c = mix(col, c, half4(clamp(-(dist + uniforms->u.w), 0.0, 1.0)));\n";
    }


    
    
    if (_features.alpha) {
        s += "c.a *= uniforms->alpha;\n";
    }
        
    s+= "return c;\n";
    s += "}\n";
    
    return s;
}
    
string StandardShader::getFragmentSource() {
    return "";
}


class CoreVideoTexture : public MetalTexture {
public:
    
    CVMetalTextureRef _cvTexture;
    CVMetalTextureCacheRef _cvTextureCache;
    
    CoreVideoTexture(BitmapApple* bitmap, Renderer* renderer, CVMetalTextureCacheRef cvTextureCache) : MetalTexture(renderer) {
        _cvTextureCache = cvTextureCache;
        CVReturn err = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, cvTextureCache, bitmap->_cvImageBuffer, NULL, MTLPixelFormatBGRA8Unorm, bitmap->_width, bitmap->_height, 0, &_cvTexture);
        assert(err==0);
        _tex = CVMetalTextureGetTexture(_cvTexture);
        assert(_tex);
        
        MTLSamplerDescriptor* samplerDesc = [MTLSamplerDescriptor new];
        samplerDesc.minFilter = _minFilterLinear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
        samplerDesc.magFilter = _magFilterLinear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
        samplerDesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
        _sampler = [s_device newSamplerStateWithDescriptor:samplerDesc];
    }
    ~CoreVideoTexture() {
        if (_cvTexture) {
            _cvTexture = NULL;
            CVMetalTextureCacheFlush(_cvTextureCache, 0);
            _cvTextureCache = NULL;
        }
    }
    void resize(int width, int height) override {
        assert(0); // CV textures aren't resizable
    }

};


class RendererApple : public Renderer {
public:

    id<MTLDevice> _device;
    CAMetalLayer* _metalLayer;
    id<MTLBuffer> _vertexBuffer;
    id<MTLBuffer> _indexBuffer;
    id<MTLCommandQueue> _commandQueue;
    id<MTLCommandBuffer> _commandBuffer;
    id<CAMetalDrawable> _drawable;
    MetalSurface* _primarySurface;
    MTLRenderPassDescriptor* _renderPassDescriptor;
    Texture* _currentTextureBound;
    id<MTLRenderPipelineState> _currentPipelineState;
    
    id<MTLRenderCommandEncoder> _renderCommandEncoder;
    id<MTLBlitCommandEncoder> _blitCommandEncoder;

    RendererApple(Window* window) : Renderer(window) {
        _device = MTLCreateSystemDefaultDevice();
        s_device = _device;
        _renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        _primarySurface = new MetalSurface(this, false);

        _quadBuffer._resizeFunc = [=](int oldItemCount, int newItemCount) {
            long cb = newItemCount * _quadBuffer._itemSize;
            id<MTLBuffer> oldVertexBuffer = _vertexBuffer;
            //_vertexBuffer = [_device newBufferWithLength:cb options:MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared];
            _vertexBuffer = [_device newBufferWithLength:cb options:MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeShared];
            _quadBuffer._base = (uint8_t*)_vertexBuffer.contents;
            if (oldVertexBuffer) {
                memcpy(_quadBuffer._base, oldVertexBuffer.contents, oldVertexBuffer.allocatedSize);
            }


            // Grow the index buffer, copying the old one into place
            int cbIndexes = sizeof(uint16_t) * 6 * newItemCount;
            id<MTLBuffer> oldIndexBuffer = _indexBuffer;
            _indexBuffer = [_device newBufferWithLength:cbIndexes options:MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeShared];
            uint16_t* newIndexes = (uint16_t*)_indexBuffer.contents;
            if (oldItemCount) {
                memcpy(newIndexes, oldIndexBuffer.contents, sizeof(uint16_t) * 6 * oldItemCount);
            }
            
            /*
             
             C------D
             |     /|
             |    / |
             |   /  |
             |  /   |
             | /    |
             A------B
             
             Clockwise triangles are ABC & CBD
             */
            for (int i=oldItemCount ; i<newItemCount ; i++) {
                newIndexes[i*6+0] = i*4+0; // A
                newIndexes[i*6+1] = i*4+2; // C
                newIndexes[i*6+2] = i*4+3; // D
                newIndexes[i*6+3] = i*4+0; // A
                newIndexes[i*6+4] = i*4+3; // D
                newIndexes[i*6+5] = i*4+1; // B
            }
            
            _fullBufferUploadNeeded =true;

        };

    }
    
    int getIntProperty(IntProperty property) override {
        
        // See https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
        if (property == IntProperty::MaxVaryingFloats) {
            return 60; // See note on "Maximum number of inputs to a fragment function"
        }
        assert(0);
        return 0;
    }
    
    void bindToNativeWindow(long nativeWindowHandle) override {
        NativeView* nativeView = (__bridge NativeView*)(void*)nativeWindowHandle;
#if PLATFORM_MACOS
        _metalLayer = [CAMetalLayer layer];
        nativeView->_metalLayer =_metalLayer;
#else
        _metalLayer = (CAMetalLayer*)nativeView.layer;
#endif
        _metalLayer.opaque = YES;
        _metalLayer.device = _device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        _metalLayer.framebufferOnly = false;//true;
        _metalLayer.contentsScale = _window->_scale;// [NSScreen mainScreen].backingScaleFactor;
#if PLATFORM_MACOS
        _metalLayer.displaySyncEnabled = 0;
#endif
#if PLATFORM_MACOS
        nativeView.wantsLayer = YES;
#endif
        _commandQueue = [_device newCommandQueue];
    }
    Surface* getPrimarySurface() override {
        return _primarySurface;
    }
    Surface* createPrivateSurface() override {
        return new MetalSurface(this, true);

    }
    
    
    void* createShaderState(Shader* shader) override {
        return new ShaderState(shader, _device);
    }
    void deleteShaderState(void* shaderState) override {
        ShaderState* state = (ShaderState*)shaderState;
        delete state;
    }

    
    void pushClip(RECT clip) override {
        bool firstClip = _clips.size()==0;
        if (!firstClip) {
            clip.intersectWith(_clips.top());
        }
        _clips.push(clip);
        MTLScissorRect rect;
        rect.x = clip.left();
        rect.y = clip.top();
        rect.width = clip.size.width;
        rect.height = clip.size.height;
        [_renderCommandEncoder setScissorRect:rect];
    }
    void popClip() override {
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        assert(_clips.size()>0);
        RECT clip = _clips.top();
        _clips.pop();
        MTLScissorRect rect;
        rect.x = clip.left();
        rect.y = clip.top();
        rect.width = clip.size.width;
        rect.height = clip.size.height;
        if (_clips.size() == 0) {
            rect.x = 0;
            rect.y = 0;
            rect.width = metalSurface->_size.width;
            rect.height = metalSurface->_size.height;
        }
        [_renderCommandEncoder setScissorRect:rect];
    }

    Texture* createTexture() override {
        return new MetalTexture(this);
    }
    

    
    void flushQuadBuffer() override {
        
    }
    
    void setCurrentBlendMode(int blendMode) override {
        _blendMode = blendMode;
    }


    void setUniformData(int16_t uniformIndex, const void* data, int32_t cb) override {
        auto& uniform = _currentShader->_uniforms[uniformIndex];
        bool isVertexUniform = (uniform.usage==Shader::Uniform::Usage::Vertex);
        ShaderState* state = (ShaderState*)_currentShader->_shaderState;
        bytearray& uniformData = isVertexUniform ? state->_vertexUniformData : state->_fragUniformData;
        auto currentData = (uniformData.data() + uniform.offset);
        if (0 == memcmp(currentData, data, cb)) {
            return;
        }
        memcpy(currentData, data, cb);
        if (isVertexUniform) {
            state->_vertexUniformsValid = false;
        } else {
            state->_fragUniformsValid = false;
        }
    }

    
    void drawQuads(int numQuads, int index) override {
        
        ShaderState* state = (ShaderState*)_currentShader->_shaderState;
        

        // Bind pipeline state
        if (_currentPipelineState != state->_pipelineState[_blendMode]) {
            _currentPipelineState = state->_pipelineState[_blendMode];
            [_renderCommandEncoder setRenderPipelineState:_currentPipelineState];
            _currentTextureBound = NULL;
        }
        
        // Ensure uniforms are up to date
        if (!state->_vertexUniformsValid) {
            state->_vertexUniformsValid = true;
            [_renderCommandEncoder setVertexBytes:state->_vertexUniformData.data() length:state->_vertexUniformData.size() atIndex:1];
        }
        if (!state->_fragUniformsValid) {
            state->_fragUniformsValid = true;
            [_renderCommandEncoder setFragmentBytes:state->_fragUniformData.data() length:state->_fragUniformData.size() atIndex:0];
        }

        // Bind to texture if there is one
        if (_currentTexture && _currentTexture != _currentTextureBound) {
            _currentTextureBound = _currentTexture;
            MetalTexture* metalTexture = (MetalTexture*)_currentTexture;
            
            if (metalTexture->_needsUpload) {
                metalTexture->_needsUpload = false;
                PIXELDATA pixeldata;
                metalTexture->_bitmap->lock(&pixeldata, false);
                [metalTexture->_tex replaceRegion:MTLRegionMake2D(0,0,metalTexture->_bitmap->_width,metalTexture->_bitmap->_height) mipmapLevel:0 withBytes:pixeldata.data bytesPerRow:pixeldata.stride];
                metalTexture->_bitmap->unlock(&pixeldata, false);
            }
            [_renderCommandEncoder setFragmentTexture:metalTexture->_tex atIndex:0];
            [_renderCommandEncoder setFragmentSamplerState:metalTexture->_sampler atIndex:0];
        }

        [_renderCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                          indexCount:numQuads*6
                                           indexType:MTLIndexTypeUInt16
                                         indexBuffer:_indexBuffer
                                   indexBufferOffset:index*6*sizeof(uint16_t)];
    }
    void prepareToDraw() override {
        _drawable = [_metalLayer nextDrawable];
        if (!_drawable) {
            return;
        }

        _commandBuffer = [_commandQueue commandBuffer];
        _primarySurface->_texture.as<MetalTexture>()->_tex = _drawable.texture;
        
        _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
        //_renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        //_renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 1, 0, 1);


        _renderCounter++;
        _currentSurface = NULL;
        _currentTexture = NULL;
        _currentTextureBound = NULL;
        _currentPipelineState = NULL;
        _currentShader = NULL;
    }

    void commit() override {
        if (!_drawable) {
            return;
        }
        [_renderCommandEncoder endEncoding];
        _renderCommandEncoder = nil;
        [_commandBuffer presentDrawable:_drawable];
        [_commandBuffer commit];
        
        // NB! Here we stall the CPU until the GPU has processed the command buffer.
        // This will be unsatisfactory if the app needs more CPU time and we should
        // cook up a multibuffer approach for that.
        [_commandBuffer waitUntilCompleted];
    }
    
    enum EncoderType {
        Render,
        Blit,
        None
    };
    
    void updateEncoder(EncoderType type) {
        if (type==Render && _renderCommandEncoder) {
            return;
        }
        if (type==Blit && _blitCommandEncoder) {
            return;
        }
        if (_renderCommandEncoder) {
            [_renderCommandEncoder endEncoding];
            _renderCommandEncoder = nil;
            _currentTextureBound = NULL;
        }
        if (_blitCommandEncoder) {
            [_blitCommandEncoder endEncoding];
            _blitCommandEncoder = nil;
        }
        if (type==Render) {
            _renderCommandEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];
            // Bind vertex buffer which never changes
            [_renderCommandEncoder setVertexBuffer:_vertexBuffer offset:0 atIndex:0];
            _currentTextureBound = NULL;
            _currentPipelineState = NULL;
            
            // When starting a new render command encoder, all shaders need to resend their
            // uniform data even if it hasn't changed
            for (auto& resource : _shaders) {
                Shader* shader = (Shader*)resource;
                ShaderState* state = (ShaderState*)shader->_shaderState;
                if (state) {
                    state->_vertexUniformsValid = false;
                    state->_fragUniformsValid = false;
                }
            }
        }
        else if (type==Blit) {
            _blitCommandEncoder = [_commandBuffer blitCommandEncoder];
        }
    }

    void setCurrentSurface(Surface* surface) override {

        // Terminate the current encoder if there is one
        updateEncoder(None);
        
        // Create new encoder
        MetalSurface* metalSurface = (MetalSurface*)surface;
        _renderPassDescriptor.colorAttachments[0].texture = metalSurface->_texture.as<MetalTexture>()->_tex;
        updateEncoder(Render);
        //if (surface == _primarySurface) {
        //    _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
        //} else {
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
        //}

        // Update the viewport
        [_renderCommandEncoder setViewport:metalSurface->_viewport];
    }
    
    void setCurrentTexture(Texture* texture) override {
        _currentTexture = texture;
        // defer texture update to blit
    }

    
    void bindCurrentShader() override {
        // no-op, binding is deferred to drawQuads
    }
    
    void uploadQuad(ItemPool::Alloc* alloc) override {
        // no-op cos vertex buffer is shared between CPU and GPU
    }
    
    void copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin) override {
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        MetalTexture* metalDestTex = (MetalTexture*)destTex;

        updateEncoder(Blit);
        
        [_blitCommandEncoder copyFromTexture:metalSurface->getTexture()
                  sourceSlice:0 sourceLevel:0
                 sourceOrigin:{static_cast<NSUInteger>(rect.origin.x),static_cast<NSUInteger>(rect.origin.y),0}
                   sourceSize:{static_cast<NSUInteger>(rect.size.width),static_cast<NSUInteger>(rect.size.height),1}
                    toTexture:metalDestTex->_tex
             destinationSlice:0 destinationLevel:0
            destinationOrigin:{static_cast<NSUInteger>(destOrigin.x),static_cast<NSUInteger>(destOrigin.y),0}];

    }

    void generateMipmaps(Texture* tex) override {
        updateEncoder(Blit);
        MetalTexture* metalTex = (MetalTexture*)tex;
        [_blitCommandEncoder generateMipmapsForTexture:metalTex->_tex];
    }
    
    CVMetalTextureCacheRef _cvTextureCache;
    
    void createTextureForBitmap(Bitmap* abitmap) override {
        BitmapApple* bitmap = (BitmapApple*)abitmap;
        if (bitmap->_cvImageBuffer) {
            if (!_cvTextureCache) {
                CVReturn err = CVMetalTextureCacheCreate(NULL, NULL, _device, NULL, &_cvTextureCache);
                assert(err==0);
            }
            bitmap->_texture = new CoreVideoTexture(bitmap, this, _cvTextureCache);
        } else {
            Renderer::createTextureForBitmap(abitmap);
        }
    }


};







string BlurShader::getVertexSource() {
    int numOptimizedOffsets = (int)_op->_optimizedOffsets.size();

    string s =
    "using namespace metal;\n"
    "\n"
    "struct VertexInput {\n"
    "   float2 position [[attribute(0)]];\n"
    "   float2 texcoord [[attribute(1)]];\n"
    "   uint unused0;\n"
    "   float unused1;\n"
    "   float unused2;\n"
    "   float unused3;\n"
    "};\n"
    "struct VertexOutput {\n"
    "   float4 position [[position]];\n";
    for (int i=0 ; i<1 + numOptimizedOffsets * 2 ; i++) {
        s+= string::format("   float2 blurCoordinates%d;\n", i);
    }
    s+= "};\n";

    s+= "struct VertexUniforms {\n";
    s+= getUniformFields(Uniform::Usage::Vertex);
    s+="};\n"
    "struct FragUniforms {\n";
    s+= getUniformFields(Uniform::Usage::Fragment);
    s+= "};\n";

    
    s+= "vertex VertexOutput vertex_shader(uint vid [[vertex_id]],\n"
    "                                  constant VertexInput* v_in [[buffer(0)]],\n"
    "                                  constant VertexUniforms* uniforms [[buffer(1)]]) {\n"
    "   VertexOutput output;\n"
    "   output.position = uniforms->mvp * float4(v_in[vid].position,0,1);\n";

    s += "   output.blurCoordinates0 = v_in[vid].texcoord;\n";
    for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
        s+= string::format(
            "   output.blurCoordinates%lu = v_in[vid].texcoord + uniforms->texOffset * %f;\n"
            "   output.blurCoordinates%lu = v_in[vid].texcoord - uniforms->texOffset * %f;\n",
            (unsigned long)((i * 2) + 1), _op->_optimizedOffsets[i],
            (unsigned long)((i * 2) + 2), _op->_optimizedOffsets[i]);
    }
    s += "    return output;\n"
         "}\n\n";

    
    
    
    s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]\n"
        ",constant FragUniforms* uniforms [[buffer(0)]]\n";
    s+= ",texture2d<half> colorTexture [[ texture(0) ]]\n";
    s+= ",sampler textureSampler [[ sampler(0) ]]";
    s+= ") {\n";
    
    s += string::format(
         "half4 c = colorTexture.sample(textureSampler, in.blurCoordinates0) * %f;\n",
            _op->_standardGaussianWeights[0]);
    // Inner texture loop
    for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
        GLfloat firstWeight = _op->_standardGaussianWeights[i * 2 + 1];
        GLfloat secondWeight = _op->_standardGaussianWeights[i * 2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
        s+= string::format("c += colorTexture.sample(textureSampler, in.blurCoordinates%lu) * %f;\n", (unsigned long)((i * 2) + 1), optimizedWeight);
        s+= string::format("c += colorTexture.sample(textureSampler, in.blurCoordinates%lu) * %f;\n", (unsigned long)((i * 2) + 2), optimizedWeight);
    }
    
    // If the number of required samples exceeds the amount we can pass in via varyings, we
    // have to do dependent texture reads in the fragment shader
    uint32_t trueNumberOfOptimizedOffsets = _op->_blurRadius / 2 + (_op->_blurRadius % 2);
    
    
    if (trueNumberOfOptimizedOffsets > numOptimizedOffsets) {
        for (uint32_t i = numOptimizedOffsets; i < trueNumberOfOptimizedOffsets; i++) {
            GLfloat firstWeight = _op->_standardGaussianWeights[i * 2 + 1];
            GLfloat secondWeight = _op->_standardGaussianWeights[i * 2 + 2];
            
            GLfloat optimizedWeight = firstWeight + secondWeight;
            GLfloat optimizedOffset = (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) / optimizedWeight;
            
            s+= string::format("c += colorTexture.sample(textureSampler, in.blurCoordinates0 + texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
            s+= string::format("c += colorTexture.sample(textureSampler, in.blurCoordinates0 - texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
        }
    }
    
    s+= "return c;\n";
    s += "}\n";
    return s;
}
string BlurShader::getFragmentSource() {
    return "";
}


string PostBlurShader::getVertexSource() {
    string s = getShaderSource(this, true, true);

    s+= "constant half3 luminanceWeighting = half3(0.2125h, 0.7154h, 0.0721h);\n";

    // Generic stuff, move elsewhere
    bool useTexSampler = true;
    string fragUniforms = getUniformFields(Uniform::Usage::Fragment);
    if (fragUniforms.length() > 0) {
        s+= "struct FragUniforms {\n";
        s+= fragUniforms;
        s+= "};\n";
    }
    s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]\n";
    if (fragUniforms.length() > 0) {
        s += ",constant FragUniforms* uniforms [[buffer(0)]]\n";
    }
    if (useTexSampler) {
        s+= ",texture2d<half> colorTexture [[ texture(0) ]]\n";
        s+= ",sampler textureSampler [[ sampler(0) ]]";
    }
    s+= ") {\n";
    
    s += "   half4 c = mix(colorTexture.sample(textureSampler, in.texcoord), in.color, 0.9h);\n"
        "   half lum = dot(c.rgb, luminanceWeighting);\n"
        "   half lumRatio = (0.5h - lum) * 0.1h;\n"
        "   return half4(mix(half3(lum), c.rgb, half3(0.8h)) + lumRatio, 1.0h);\n"
    "}\n";
    return s;
}
string PostBlurShader::getFragmentSource() {
    return "";
}





Renderer* Renderer::create(Window* window) {
    return new RendererApple(window);
}


#endif
#endif
