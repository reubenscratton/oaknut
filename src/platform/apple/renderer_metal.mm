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


static id<MTLDevice> s_device;


class MetalTexture : public Texture {
public:
    id<MTLTexture> _tex;
    id<MTLSamplerState> _sampler;
    
    MetalTexture(Renderer* renderer, int format) : Texture(renderer, format) {
    }
    
    void unload() {
        _tex = NULL;
        _sampler = NULL;
    }
    void resize(int width, int height) override {
        MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
        switch (_format) {
            case PIXELFORMAT_RGBA32:
                pixelFormat = MTLPixelFormatRGBA8Unorm;
                break;
            case PIXELFORMAT_BGRA32:
                pixelFormat = MTLPixelFormatBGRA8Unorm;
                break;
            case PIXELFORMAT_RGB24:
                assert(0); // Metal does not support this format, you must use 32bpp
                break;
            case PIXELFORMAT_A8:
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
        _texture = new MetalTexture(renderer, renderer->_primarySurfaceFormat);
    }
    
    void setSize(const SIZE& size) override {
        Surface::setSize(size);
        _viewport.width = size.width;
        _viewport.height = size.height;
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

static string getUniformFields(Shader* shader, Shader::Uniform::Usage usage) {
    string s="";
    for (auto& uniform : shader->_uniforms) {
        if (usage == uniform.usage) {
            s += sl_getTypeString(uniform.type);
            s+= " ";
            s+= uniform.name;
            s+= ";\n";
        }
    }
    return s;
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
        
        _vertexUniformsValid = false;
        _fragUniformsValid = false;

        // Work out the uniform structs, respecting alignments as per Metal Language spec
        int32_t cbVert=0, cbFrag=0;
        int32_t vertMaxAlign=0, fragMaxAlign=0;
        for (auto& uniform: shader->_uniforms) {
            int32_t alignment = 4;
            switch (uniform.type) {
                case Shader::VariableType::Color: alignment=8; break; // Color is a half4
                case Shader::VariableType::Int1: alignment=4; break;
                case Shader::VariableType::Float1: alignment=4; break;
                case Shader::VariableType::Float2: alignment=8; break;
                case Shader::VariableType::Float4: alignment=16; break;
                case Shader::VariableType::Matrix4: alignment=16; break;
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
        for (auto& attribute : shader->_attributes) {
            if (0!=attribute.name.compare("position")) {
                s += sl_getTypeString(attribute.type);
                s += " ";
                s += attribute.name;
                s += ";\n";
            }
        }
        s+= "};\n"
        "struct VertexUniforms {\n";
        s+= getUniformFields(shader, Shader::Uniform::Usage::Vertex);
        s+= "};\n";
        
        // Vertex shader
        s+= "vertex VertexOutput vertex_shader(uint vid [[vertex_id]],\n"
        "                                  constant VertexInput* v_in [[buffer(0)]],\n"
        "                                  constant VertexUniforms* uniforms [[buffer(1)]]) {\n"
        "   VertexOutput output;\n"
        "   output.position = uniforms->mvp * float4(v_in[vid].position,0,1);\n";
        
        // All non-position attributes
        for (auto& attribute : shader->_attributes) {
            if (0!=attribute.name.compare("position")) {
                auto szname = attribute.name.c_str();
                s += string::format("output.%s = ", szname);
                if (attribute.type == Shader::VariableType::Color) {
                    s += string::format("unpack_unorm4x8_to_half(v_in[vid].%s)", szname);
                } else {
                    if (attribute.outValue.length() > 0) {
                        s += attribute.outValue;
                    } else {
                        s += string::format("v_in[vid].%s", szname);
                    }
                }
                s += ";\n";
            }
        }

        s+= "   return output;\n"
            "}\n";

        // Frag shader header
        string fragUniforms = getUniformFields(shader, Shader::Uniform::Usage::Fragment);
        if (fragUniforms.length() > 0) {
            s+= "struct FragUniforms {\n";
            s+= fragUniforms;
            s+= "};\n";
        }
        s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]\n";
        if (fragUniforms.length() > 0) {
            s += ",constant FragUniforms* uniforms [[buffer(0)]]\n";
        }
        if (shader->_features.textures[0] != Texture::Type::None) {
            s+= ",texture2d<half> colorTexture [[ texture(0) ]]\n";
            s+= ",sampler textureSampler [[ sampler(0) ]]";
        }
        s+= ") {\n"
        " half4 c;\n";
        
        s += shader->getFragmentSource();

        s+= "return c;\n";
        s += "}\n";
        

        NSError* error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:[NSString stringWithUTF8String:s.c_str()] options:nil error:&error];
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




class CoreVideoTexture : public MetalTexture {
public:
    
    CVMetalTextureRef _cvTexture;
    CVMetalTextureCacheRef _cvTextureCache;
    
    CoreVideoTexture(BitmapApple* bitmap, Renderer* renderer, CVMetalTextureCacheRef cvTextureCache) : MetalTexture(renderer, bitmap->_format) {
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
        _metalLayer.contentsScale = app->_defaultDisplay->_scale;// [NSScreen mainScreen].backingScaleFactor;
#if PLATFORM_MACOS
        _metalLayer.displaySyncEnabled = 0;
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
        if (clip.size.width<0) clip.size.width = 0;
        if (clip.size.height<0) clip.size.height = 0;
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
        // app->log("push: %ld,%ld x %ld,%ld", rect.x,rect.y, rect.width, rect.height);
        [_renderCommandEncoder setScissorRect:rect];
    }
    void popClip() override {
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        assert(_clips.size()>0);
        _clips.pop();
        MTLScissorRect rect;
        if (!_clips.size()) {
            rect.x = 0;
            rect.y = 0;
            rect.width = metalSurface->_size.width;
            rect.height = metalSurface->_size.height;
        } else {
            RECT clip = _clips.top();
            rect.x = clip.left();
            rect.y = clip.top();
            rect.width = clip.size.width;
            rect.height = clip.size.height;
        }
        // app->log("pop: %ld,%ld x %ld,%ld", rect.x,rect.y, rect.width, rect.height);
        [_renderCommandEncoder setScissorRect:rect];
    }

    Texture* createTexture(int format) override {
        return new MetalTexture(this, format);
    }
    

    
    void flushQuadBuffer() override {
        
    }
    
    void setCurrentBlendMode(int blendMode) override {
        _blendMode = blendMode;
    }

    static inline uint16_t make_half(float f) {
        uint32_t x = *((uint32_t*)&f);
        int e = ((x&0x7f800000U) >> 23) -127; // undo 32-bit bias to get the real exponent
        e = MIN(15, MAX(-15,e)) + 15; // clamp to 5-bit range and add the 16-bit bias
        uint16_t h = ((x>>16)&0x8000U)
                   | (e<<10) // exponent is 5 bits, down from 8
                   | ((x>>13)&0x03ffU); // significand is 10 bits, truncated from 23
        return h;
    }
    
    void setColorUniform(int16_t uniformIndex, const float* rgba) override {
        uint16_t halfs[4];
        halfs[0] = make_half(rgba[0]);
        halfs[1] = make_half(rgba[1]);
        halfs[2] = make_half(rgba[2]);
        halfs[3] = make_half(rgba[3]);
        setUniformData(uniformIndex, halfs, sizeof(halfs));
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
        
        if (_window->_backgroundColor) {
            float f[4] = {
                (_window->_backgroundColor & 0xFF) / 255.0f,
                ((_window->_backgroundColor & 0xFF00)>>8) / 255.0f,
                ((_window->_backgroundColor & 0xFF0000)>>16) / 255.0f,
                ((_window->_backgroundColor & 0xFF000000)>>24) / 255.0f
            };
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            _renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(f[0], f[1], f[2], f[3]);
        } else {
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
        }

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


Renderer* Renderer::create(Window* window) {
    return new RendererApple(window);
}


#endif
#endif
