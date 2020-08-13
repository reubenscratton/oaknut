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
static dispatch_queue_t s_uploaderQueue;



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
    
    bool readPixels(RECT rect, bytearray& target) const override {
        int stride = rect.size.width * 4;
        int cp = stride * rect.size.height;
        if (target.size() < cp) {
            target.resize(cp);
        }
        [_tex getBytes:(void *)target.data()
           bytesPerRow:stride
            fromRegion:MTLRegionMake2D(rect.origin.x,rect.origin.y, rect.size.width, rect.size.height)
                   mipmapLevel:0];
        return true;
    }
    
    void createMetalTexture() {
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
                //pixelFormat = MTLPixelFormatR8Unorm;
                break;
        }
        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                        width:_size.width
                                                                                       height:_size.height
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
        
        _isNativeTextureValid = true;
    }
};

class MetalSurface : public Surface {
public:
    CAMetalLayer* _metalLayer; // nil for private surface
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
    
    void bindToNativeWindow(long nativeWindowHandle) override {
        NativeView* nativeView = (__bridge NativeView*)(void*)nativeWindowHandle;
        _metalLayer = (CAMetalLayer*)nativeView.layer;
        _metalLayer.opaque = YES;
        _metalLayer.device = s_device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        _metalLayer.framebufferOnly = false;
        _metalLayer.contentsScale = app->_defaultDisplay->_scale;// [NSScreen mainScreen].backingScaleFactor;
        // _metalLayer.maximumDrawableCount = 3;
        _metalLayer.drawsAsynchronously = YES;
#if PLATFORM_MACOS
        _metalLayer.displaySyncEnabled = 1;
        nativeView.wantsLayer = YES;
#endif
    }

    void setSize(const SIZE& size) override {
        Surface::setSize(size);
        _viewport.width = size.width;
        _viewport.height = size.height;
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
    bytearray _vertexUniformData;
    bytearray _fragUniformData;
    bool _vertexUniformsValid;
    bool _fragUniformsValid;
    unordered_map<BlendParams, id<MTLRenderPipelineState>> _pipelineStates;
    
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
        for (auto& attribute : shader->_vertexShaderOutputs) {
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
        for (auto& attribute : shader->_vertexShaderOutputs) {
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
        
        s += shader->getSupportingSource();
        
        s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]\n";
        if (fragUniforms.length() > 0) {
            s += ",constant FragUniforms* uniforms [[buffer(0)]]\n";
        }
        for (int i=0 ; i<shader->getTextureCount() ; i++) {
            auto tex = shader->getTextureType(i);
            if (tex != Texture::Type::None) {
                s+= ",texture2d<half> colorTexture [[ texture(0) ]]\n";
                s+= ",sampler textureSampler [[ sampler(0) ]]";
            }
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
    }
    
};




class CoreVideoTexture : public MetalTexture {
public:
    
    CVMetalTextureRef _cvTexture;
    CVMetalTextureCacheRef _cvTextureCache;
    
    CoreVideoTexture(BitmapApple* bitmap, Renderer* renderer, CVMetalTextureCacheRef cvTextureCache) : MetalTexture(renderer, bitmap->_format) {
        _size.width = bitmap->_width;
        _size.height = bitmap->_height;
        _isNativeTextureValid = true;
        
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
            CFRelease(_cvTexture);
            _cvTexture = NULL;
            CVMetalTextureCacheFlush(_cvTextureCache, 0);
            _cvTextureCache = NULL;
        }
    }

};


class RenderTaskApple : public RenderTask {
public:
    id<MTLCommandBuffer> _commandBuffer;
    MTLRenderPassDescriptor* _renderPassDescriptor;
    MTLRenderPipelineDescriptor* _pipelineStateDescriptor;
    id<MTLRenderCommandEncoder> _renderCommandEncoder;
    id<MTLBlitCommandEncoder> _blitCommandEncoder;
    id<MTLRenderPipelineState> _currentPipelineState;
    id<MTLBuffer> __strong* _vertexBuffer;
    id<MTLBuffer> __strong* _indexBuffer;
    id<CAMetalDrawable> _drawable;

    RenderTaskApple(Renderer* renderer, id<MTLCommandBuffer> commandBuffer, id<MTLBuffer> __strong * vertexBuffer, id<MTLBuffer> __strong* indexBuffer) : RenderTask(renderer) {
        _commandBuffer = commandBuffer;
        _renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
        _vertexBuffer = vertexBuffer;
        _indexBuffer = indexBuffer;
        _pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    }
  
    bool bindToNativeSurface(Surface* surface) override {
        MetalSurface* metalSurface = (MetalSurface*)surface;
        MetalTexture* metalTex = metalSurface->_texture.as<MetalTexture>();
        assert(metalSurface->_metalLayer);
        _drawable = [metalSurface->_metalLayer nextDrawable];
        if (!_drawable) {
            return false;
        }
        metalTex->_tex = _drawable.texture;
        return true;
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



    inline static MTLBlendFactor convertMTLBlendFactor(BlendFactor blendFactor) {
        switch (blendFactor) {
            case BlendFactor::Zero: return MTLBlendFactorZero;
            case BlendFactor::SourceColor: return MTLBlendFactorSourceColor;
            case BlendFactor::SourceAlpha: return MTLBlendFactorSourceAlpha;
            case BlendFactor::DestinationColor: return MTLBlendFactorDestinationColor;
            case BlendFactor::DestinationAlpha: return MTLBlendFactorDestinationAlpha;
            case BlendFactor::One: return MTLBlendFactorOne;
            case BlendFactor::OneMinusSourceColor: return MTLBlendFactorOneMinusSourceColor;
            case BlendFactor::OneMinusSourceAlpha: return MTLBlendFactorOneMinusSourceAlpha;
            case BlendFactor::OneMinusDestinationColor: return MTLBlendFactorOneMinusDestinationColor;
            case BlendFactor::OneMinusDestinationAlpha: return MTLBlendFactorOneMinusDestinationAlpha;
        }
        assert(0);
    }
    void draw(PrimitiveType type, int count, int index) override {

        // If the surface being drawn to is "invalid", a new 'render command encoder' is required
        if (!_currentSurfaceValid) {
            _currentSurfaceValid = true;
            
            // Close the current encoder if there is one
            if (_renderCommandEncoder) {
                [_renderCommandEncoder endEncoding];
                _renderCommandEncoder = nil;
            }
            if (_blitCommandEncoder) {
                [_blitCommandEncoder endEncoding];
                _blitCommandEncoder = nil;
            }

            // Configure new encoder to use the current surface
            MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
            MetalTexture* metalTex = metalSurface->_texture.as<MetalTexture>();
            
            if (!metalTex->_isNativeTextureValid) {
                metalTex->createMetalTexture();
            }
            
            _renderPassDescriptor.colorAttachments[0].texture = metalTex->_tex;
            if (_currentSurface->_clearNeeded) {
                float f[4] = {
                    (_currentSurface->_clearColor & 0xFF) / 255.0f,
                    ((_currentSurface->_clearColor & 0xFF00)>>8) / 255.0f,
                    ((_currentSurface->_clearColor & 0xFF0000)>>16) / 255.0f,
                    ((_currentSurface->_clearColor & 0xFF000000)>>24) / 255.0f
                };
                _currentSurface->_clearNeeded = false;
                _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                _renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(f[0], f[1], f[2], f[3]);
            } else {
                _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
            }
            
            // Create new encoder
            _renderCommandEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];
            
            // Bind vertex buffer which never changes
            [_renderCommandEncoder setVertexBuffer:*_vertexBuffer offset:0 atIndex:0];
            _currentTextureValid = false;
            _currentPipelineState = nullptr;
            
            // When starting a new render command encoder, all shaders need to resend their
            // uniform data even if it hasn't changed
            for (auto& resource : _renderer->_shaders) { // TODO: this feels wrong. We only want to iterate "shaders currently used by this task, not all known shaders"
                Shader* shader = (Shader*)resource;
                ShaderState* state = (ShaderState*)shader->_shaderState;
                if (state) {
                    state->_vertexUniformsValid = false;
                    state->_fragUniformsValid = false;
                }
            }

            // Update the viewport
            [_renderCommandEncoder setViewport:metalSurface->_viewport];
        };
        

        ShaderState* state = (ShaderState*)_currentShader->_shaderState;
        
        // If shader or blend has changed, need a new pipeline state
        if (!_currentPipelineState || !_currentShaderValid || !_blendParamsValid) {
            _currentShaderValid = true;
            _blendParamsValid = true;

            // TODO: special case no-blend and normal blend to avoid the hash lookup here
            auto it = state->_pipelineStates.find(_blendParams);
            if (it != state->_pipelineStates.end()) {
                _currentPipelineState = it->second;
            } else {
                _pipelineStateDescriptor.vertexFunction = state->_vertexShader;
                _pipelineStateDescriptor.fragmentFunction = state->_fragShader;
                MTLRenderPipelineColorAttachmentDescriptor* att = _pipelineStateDescriptor.colorAttachments[0];
                att.pixelFormat = _currentSurface->_texture.as<MetalTexture>()->_tex.pixelFormat;
                if (_blendParams.op == BlendOp::None) {
                    att.blendingEnabled = NO;
                } else {
                    att.blendingEnabled = YES;
                    MTLBlendOperation op = (_blendParams.op == BlendOp::Add) ? MTLBlendOperationAdd : MTLBlendOperationSubtract;
                    att.rgbBlendOperation = op;
                    att.alphaBlendOperation = op;
                    att.sourceRGBBlendFactor = convertMTLBlendFactor(_blendParams.srcRGB);
                    att.sourceAlphaBlendFactor = convertMTLBlendFactor(_blendParams.srcA);
                    att.destinationRGBBlendFactor = convertMTLBlendFactor(_blendParams.dstRGB);
                    att.destinationAlphaBlendFactor = convertMTLBlendFactor(_blendParams.dstA);
                }
                
                NSError* error = nil;
                _currentPipelineState = [s_device newRenderPipelineStateWithDescriptor:_pipelineStateDescriptor
                                                                                        error:&error];
                state->_pipelineStates.emplace(_blendParams, _currentPipelineState);
            }

            [_renderCommandEncoder setRenderPipelineState:_currentPipelineState];
            
            // Texture needs rebinding after pipeline state change
            _currentTextureValid = false;
        }
        
        // Validate source texture
        if (_currentTexture) {
            MetalTexture* metalTexture = (MetalTexture*)_currentTexture;
            if (!_currentTexture->_isNativeTextureValid) {
                metalTexture->createMetalTexture();
            }
            
            // Ensure source texture is in graphics RAM
            if (metalTexture->_needsUpload) {
                metalTexture->_needsUpload = false;
                metalTexture->retain();
                dispatch_async(s_uploaderQueue, ^{
                    PIXELDATA pixeldata;
                    metalTexture->_bitmap->lock(&pixeldata, false);
                    //log_dbg("texture upload %d x %d", _bitmap->_width, _bitmap->_height);
                    [metalTexture->_tex replaceRegion:MTLRegionMake2D(0,0,metalTexture->_bitmap->_width,metalTexture->_bitmap->_height) mipmapLevel:0 withBytes:pixeldata.data bytesPerRow:pixeldata.stride];
                    metalTexture->_bitmap->unlock(&pixeldata, false);
                    dispatch_async(dispatch_get_main_queue(), ^{
                        /* EXPERIMENT: Some bitmaps don't need to hang around after upload...
                         if (metalTexture->_format != PIXELFORMAT_A8) {
                            metalTexture->_bitmap = nullptr;
                        }*/
                        metalTexture->release();
                    });
                });
            }

            
            // Bind
            if (!_currentTextureValid) {
                _currentTextureValid = true;
                [_renderCommandEncoder setFragmentTexture:metalTexture->_tex atIndex:0];
                [_renderCommandEncoder setFragmentSamplerState:metalTexture->_sampler atIndex:0];
                state->_fragUniformsValid = false;
            }
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


        // Clip
        if (!_currentClipValid) {
            _currentClipValid = true;
            MTLScissorRect rect;
            rect.x = _currentClip.left();
            rect.y = _currentClip.top();
            rect.width = _currentClip.size.width;
            rect.height = _currentClip.size.height;
            // log_dbg("push: %ld,%ld x %ld,%ld", rect.x,rect.y, rect.width, rect.height);
            [_renderCommandEncoder setScissorRect:rect];
        }

        if (type == Quad) {
            [_renderCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                              indexCount:count*6
                                               indexType:MTLIndexTypeUInt16
                                             indexBuffer:*_indexBuffer
                                       indexBufferOffset:index*6*sizeof(uint16_t)];
        } else if (type == Line) {
            [_renderCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeLine
                                              indexCount:count*2
                                               indexType:MTLIndexTypeUInt16
                                             indexBuffer:*_indexBuffer
                                       indexBufferOffset:index*6*sizeof(uint16_t)];
/*            [_renderCommandEncoder drawPrimitives:MTLPrimitiveTypeLine
                                      vertexStart:index
                                      vertexCount:count*2];*/
        } else {
            assert(false);
        }
    }

    
    void useBlitEncoder() {
        if (_blitCommandEncoder) {
            return;
        }
        if (_renderCommandEncoder) {
            [_renderCommandEncoder endEncoding];
            _renderCommandEncoder = nil;
            _currentTextureValid = false;
        }
        _blitCommandEncoder = [_commandBuffer blitCommandEncoder];
    }
    
    void copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin) override {
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        MetalTexture* metalDestTex = (MetalTexture*)destTex;
        if (!metalDestTex->_isNativeTextureValid) {
            metalDestTex->createMetalTexture();
        }

        useBlitEncoder();
        
        [_blitCommandEncoder copyFromTexture:metalSurface->getTexture()
               sourceSlice:0 sourceLevel:0
              sourceOrigin:{static_cast<NSUInteger>(rect.origin.x),static_cast<NSUInteger>(rect.origin.y),0}
                sourceSize:{static_cast<NSUInteger>(rect.size.width),static_cast<NSUInteger>(rect.size.height),1}
                 toTexture:metalDestTex->_tex
          destinationSlice:0 destinationLevel:0
         destinationOrigin:{static_cast<NSUInteger>(destOrigin.x),static_cast<NSUInteger>(destOrigin.y),0}];

    }

    void generateMipmaps(Texture* tex) override {
        useBlitEncoder();
        MetalTexture* metalTex = (MetalTexture*)tex;
        [_blitCommandEncoder generateMipmapsForTexture:metalTex->_tex];
    }


    void commit(std::function<void()> onComplete) override {
        [_renderCommandEncoder endEncoding];
        _renderCommandEncoder = nil;
        if (_drawable) {
            [_commandBuffer presentDrawable:_drawable];
        }
        if (onComplete) {
            [_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> _Nonnull) {
                // on a random metal thread here, handle event on main thread
                dispatch_async(dispatch_get_main_queue(), ^{
                    onComplete();
                });
            }];
        }
        [_commandBuffer commit];

        
        // NB! Here we stall the CPU until the GPU has processed the command buffer.
        // This will be unsatisfactory if the app needs more CPU time and we should
        // cook up a multibuffer approach for that.
        if (_drawable) {
            [_commandBuffer waitUntilCompleted];
            //[_commandBuffer waitUntilScheduled];
        }
        
        _commandBuffer = nil;
    }
    
    


};

class RendererApple : public Renderer {
public:

    id<MTLDevice> _device;
    id<MTLBuffer> _vertexBuffer;
    id<MTLBuffer> _indexBuffer;
    id<MTLCommandQueue> _commandQueue;
    
    RendererApple() : Renderer() {
        _device = MTLCreateSystemDefaultDevice();
        s_device = _device;
        _commandQueue = [_device newCommandQueue];

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
        s_uploaderQueue = dispatch_queue_create("TextureUploads", DISPATCH_QUEUE_SERIAL);
        /*if (@available(macOS 10.14, *)) {
            _uploaderEvent = [_device newEvent];
        }*/
    }
    
    int getIntProperty(IntProperty property) override {
        
        // See https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
        if (property == IntProperty::MaxVaryingFloats) {
            return 60; // See note on "Maximum number of inputs to a fragment function"
        }
        assert(0);
        return 0;
    }
    
    Surface* createSurface(bool isPrivate) override {
        return new MetalSurface(this, isPrivate);
    }
    
    
    void* createShaderState(Shader* shader) override {
        return new ShaderState(shader, _device);
    }
    void deleteShaderState(void* shaderState) override {
        ShaderState* state = (ShaderState*)shaderState;
        delete state;
    }

    

    Texture* createTexture(int format) override {
        return new MetalTexture(this, format);
    }
    

    
    void flushQuadBuffer() override {
        
    }
    
    
    
    
    RenderTask* createRenderTask() override {
        return new RenderTaskApple(this, [_commandQueue commandBuffer], &_vertexBuffer, &_indexBuffer);
    }


    

    void uploadQuad(ItemPool::Alloc* alloc) override {
        // no-op cos vertex buffer is shared between CPU and GPU
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


Renderer* Renderer::create() {
    return new RendererApple();
}


#endif
#endif
