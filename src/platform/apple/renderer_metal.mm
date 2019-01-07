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



class MetalTexture : public Texture {
public:
    id<MTLTexture> _tex;
    id<MTLSamplerState> _sampler;
    
    MetalTexture(Bitmap* bitmap, id<MTLDevice> device) : Texture(bitmap) {
        _needsUpload = true;
        MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
        switch (bitmap->_format) {
            case BITMAPFORMAT_RGBA32:
                pixelFormat = MTLPixelFormatRGBA8Unorm;
                break;
            case BITMAPFORMAT_BGRA32:
                pixelFormat = MTLPixelFormatBGRA8Unorm;
                break;
            case BITMAPFORMAT_A8:
                pixelFormat = MTLPixelFormatA8Unorm;
                break;
        }
        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                        width:bitmap->_width
                                                                                       height:bitmap->_height
                                                                                    mipmapped:NO];
        _tex = [device newTextureWithDescriptor:desc];
        
        MTLSamplerDescriptor* samplerDesc = [MTLSamplerDescriptor new];
        _sampler = [device newSamplerStateWithDescriptor:samplerDesc];
    }
    
    void unload() override {
        _tex = NULL;
    }
    void upload() override {
        
    }
    int getSampler() override {
        return (_bitmap->_format==BITMAPFORMAT_A8) ? SAMPLER_TEXTURE2D_A8 : SAMPLER_TEXTURE2D_RGBA;
    }
    
};


class MetalSurface : public Surface {
public:
    id<MTLDevice> _device;
    MTLRenderPassDescriptor* _renderPassDescriptor;
    id<MTLRenderCommandEncoder> _renderCommandEncoder;
    MTLViewport _viewport;
    id<MTLTexture> _texture;
    id<MTLSamplerState> _sampler;

    MetalSurface(id<MTLDevice> device, bool isPrivate) : Surface(isPrivate) {
        _device = device;
        _viewport.originX = 0;
        _viewport.originY = 0;
        _viewport.znear = 0;
        _viewport.zfar = 1;
        _renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
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
        
        MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
            width:size.width height:size.height mipmapped:NO];

        textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        _texture = [_device newTextureWithDescriptor:textureDescriptor];
        MTLSamplerDescriptor* samplerDesc = [MTLSamplerDescriptor new];
        _sampler = [_device newSamplerStateWithDescriptor:samplerDesc];
    }
};

struct Uniforms {
    MATRIX4 mvp; // same as float4x4
};

class MetalShader : public Shader {
public:
    id<MTLDevice> _device;
    id<MTLLibrary> _library;
    id<MTLFunction> _vertexShader;
    id<MTLFunction> _fragShader;
    id<MTLRenderPipelineState> _pipelineState[3]; // one per blend mode

    MetalShader(id<MTLDevice> device, ShaderFeatures features) : Shader(features) {
        _device = device;
    }
    
    void load() override {
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
            "   float4 position [[position]];\n"
            "   float2 texcoord;\n"
            "   half4 color;\n"
            "};\n"
            "struct Uniforms {\n"
            "   float4x4 mvp;\n"
            "};\n";

        s+= "vertex VertexOutput vertex_shader(uint vid [[vertex_id]],\n"
            "                                  constant VertexInput* v_in [[buffer(0)]],\n"
            "                                  constant Uniforms* uniforms [[buffer(1)]]) {\n"
            "   VertexOutput output;\n"
            "   output.position = uniforms->mvp * float4(v_in[vid].position,0,1);\n";
        if (useTexCoords) {
            s += "   output.texcoord = v_in[vid].texcoord;\n";
        }
        s+= "   output.color = unpack_unorm4x8_to_half(v_in[vid].color);\n"
            "   return output;\n"
            "}\n";
        
        s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]";
        if (useTexSampler) {
            if (_features.sampler0 == SAMPLER_TEXTURE2D_A8) {
                s+= ", texture2d<half> colorTexture [[ texture(0) ]]";
            } else {
                s+= ", texture2d<half> colorTexture [[ texture(0) ]]";
            }
            s+= ", sampler textureSampler [[ sampler(0) ]]";
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
        s+= "return c;\n";
        s += "}\n";
        /*
        if (_features.alpha) {
            fs += "uniform mediump float alpha;\n";
        }
        if (roundRect) {
            fs += "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
            "uniform lowp vec4 strokeColor;\n";
            if (roundRect == SHADER_ROUNDRECT_1) {
                fs += "uniform mediump float radius;\n";
            } else {
                fs += "uniform mediump vec4 radii;\n";
            }
        }

         if (_features.sampler0 == GLSAMPLER_TEXTURE_2D) {
            fs += "uniform sampler2D texture;\n";
        }
        if (_features.sampler0 == GLSAMPLER_TEXTURE_EXT_OES) {
            fs += "uniform samplerExternalOES texture;\n";
        }
        */
        
        
        
        /*fs += "void main() {\n";
        
        
        
        
        if (roundRect) {
            
            if (roundRect == SHADER_ROUNDRECT_1) {
                fs += "    vec2 b = u.xy - vec2(radius); \n"
                "    float dist = length(max(abs(v_texcoord)-b, 0.0)) - radius  - 0.5;\n";
            }
            else if (roundRect == SHADER_ROUNDRECT_2H) {
                // branchless selection of radius=r.x if on left side of quad or radius=r.y on right side
                fs += "   vec2 size = u.xy; \n"
                "   vec2 r = radii.xw\n;" // TODO: this is specific to left|right config
                "   float s=step(v_texcoord.x,0.0);\n"
                "   float radius = s*r.x + (1.0-s)*r.y;\n"
                "   size -= vec2(radius);\n"
                "   vec2 d = abs(v_texcoord) - size;\n"
                "   float dist = min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;\n";
            }
            fs +=   "   vec4 col = strokeColor;\n"
            "   col.a = mix(0.0, strokeColor.a, clamp(-dist, 0.0, 1.0));\n"   // outer edge blend
            "   gl_FragColor = mix(col, " + color_src + ", clamp(-(dist + u.w), 0.0, 1.0));\n";
        }
        else {
            fs += "    gl_FragColor = " + color_src + ";\n";
        }
        if (_features.alpha) {
            fs += "    gl_FragColor.a *= alpha;\n";
        }
        fs += "}\n";
        */
        
        NSError* error = nil;
        id<MTLLibrary> library = [_device newLibraryWithSource:[NSString stringWithUTF8String:s.data()] options:nil error:&error];
        assert(library);
        _vertexShader = [library newFunctionWithName: @"vertex_shader"];
        _fragShader = [library newFunctionWithName: @"frag_shader"];

        
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineStateDescriptor.vertexFunction = _vertexShader;
        pipelineStateDescriptor.fragmentFunction = _fragShader;
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineStateDescriptor.colorAttachments[0].blendingEnabled = NO;
        _pipelineState[BLENDMODE_NONE] = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                 error:&error];

        pipelineStateDescriptor.colorAttachments[0].blendingEnabled = YES;
        pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        _pipelineState[BLENDMODE_NORMAL] = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                 error:&error];

        pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
        pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        _pipelineState[BLENDMODE_PREMULTIPLIED] = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                                   error:&error];
    }
    
    void unload() override {
        
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

    RendererApple(Window* window) : Renderer(window) {
        _device = MTLCreateSystemDefaultDevice();
        _primarySurface = new MetalSurface(_device, false);

        _quadBuffer._resizeFunc = [=](int oldItemCount, int newItemCount) {
            long cb = newItemCount * _quadBuffer._itemSize;
            //_vertexBuffer = [_device newBufferWithLength:cb options:MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared];
            _vertexBuffer = [_device newBufferWithLength:cb options:MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeShared];
            _quadBuffer._base = (uint8_t*)_vertexBuffer.contents;


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
    
    void bindToNativeWindow(long nativeWindowHandle) override {
        _metalLayer = [CAMetalLayer layer];
        _metalLayer.opaque = YES;
        _metalLayer.contentsScale = [NSScreen mainScreen].backingScaleFactor;
        _metalLayer.device = _device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        _metalLayer.framebufferOnly = true;
        NativeView* nativeView = (__bridge NativeView*)(void*)nativeWindowHandle;
        nativeView->_metalLayer =_metalLayer;
        nativeView.wantsLayer = YES;        
        _commandQueue = [_device newCommandQueue];
    }
    Surface* getPrimarySurface() override {
        return _primarySurface;
    }
    Surface* createPrivateSurface() override {
        return new MetalSurface(_device, true);

    }
    
    
    Shader* getShader(ShaderFeatures features) override {
        Shader* shader = _shaders[features.all];
        if (!shader) {
            shader = new MetalShader(_device, features);
            _shaders[features.all] = shader;
        }
        return shader;
        
    }
    
    void pushClip(RECT clip) override {
    }
    void popClip() override {
        
    }

    Texture* createTexture(Bitmap* bitmap) override {
        return new MetalTexture(bitmap, _device);
    }
    void flushQuadBuffer() override {
        
    }
    
    void setCurrentBlendMode(int blendMode) override {
        // no-op
    }

    Uniforms _uniforms;
    bool _uniformsValid;

    void prepareToRenderRenderOp(RenderOp* op, Shader* shader, const MATRIX4& mvp) override {
        Renderer::prepareToRenderRenderOp(op, shader, mvp);

        if (_uniforms.mvp != mvp) {
            _uniforms.mvp = mvp;
            _uniformsValid = false;
        }
    }

    
    void drawQuads(int numQuads, int index) override {
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;

        if (!_uniformsValid) {
            _uniformsValid = true;
            [((MetalSurface*)_currentSurface)->_renderCommandEncoder setVertexBytes:&_uniforms length:sizeof(Uniforms) atIndex:1];
        }

        [metalSurface->_renderCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
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
        _primarySurface->_texture = _drawable.texture;


        _renderCounter++;
        _currentSurface = NULL;
        _currentTexture = NULL;
        _currentShader = NULL;

    }

    void commit() override {
        if (!_drawable) {
            return;
        }
        [_primarySurface->_renderCommandEncoder endEncoding];
        _primarySurface->_renderCommandEncoder = nil;
        [_commandBuffer presentDrawable:_drawable];
        [_commandBuffer commit];
    }

    void setCurrentSurface(Surface* surface) override {

        // Terminate the current encoder if there is one
        if (_currentSurface) {
            MetalSurface* prevSurface = (MetalSurface*)_currentSurface;
            [prevSurface->_renderCommandEncoder endEncoding];
            prevSurface->_renderCommandEncoder = nil;
        }

        
        // Create new encoder
        MetalSurface* metalSurface = (MetalSurface*)surface;
        metalSurface->_renderPassDescriptor.colorAttachments[0].texture = metalSurface->_texture;
        metalSurface->_renderCommandEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:metalSurface->_renderPassDescriptor];
        
        // Bind the encoder to current texture and shader
        if (_currentTexture) {
            MetalTexture* metalTexture = (MetalTexture*)_currentTexture;
            [metalSurface->_renderCommandEncoder setFragmentTexture:metalTexture->_tex atIndex:0];
            [metalSurface->_renderCommandEncoder setFragmentSamplerState:metalTexture->_sampler atIndex:0];
        }
        if (_currentShader) {
            setCurrentShader(_currentShader);
        }
        
        // Mark uniforms as invalid (i.e. so drawQuads will inject them into the command stream).
        _uniformsValid = false;
        
        // Update the viewport
        [metalSurface->_renderCommandEncoder setViewport:metalSurface->_viewport];
    }
    
    void setCurrentTexture(Texture* texture) override {
        MetalTexture* metalTexture = (MetalTexture*)texture;
        if (metalTexture->_needsUpload) {
            metalTexture->_needsUpload = false;
            PIXELDATA pixeldata;
            metalTexture->_bitmap->lock(&pixeldata, false);
            [metalTexture->_tex replaceRegion:MTLRegionMake2D(0,0,metalTexture->_bitmap->_width,metalTexture->_bitmap->_height) mipmapLevel:0 withBytes:pixeldata.data bytesPerRow:pixeldata.stride];
            metalTexture->_bitmap->unlock(&pixeldata, false);
        }
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        [metalSurface->_renderCommandEncoder setFragmentTexture:metalTexture->_tex atIndex:0];
        [metalSurface->_renderCommandEncoder setFragmentSamplerState:metalTexture->_sampler atIndex:0];
    }

    void setCurrentShader(Shader* shader) override {
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        MetalShader* metalShader = (MetalShader*)shader;

        // Bind the render encoder to our vertex buffer
        [metalSurface->_renderCommandEncoder setRenderPipelineState:metalShader->_pipelineState[_blendMode]];
        [metalSurface->_renderCommandEncoder setVertexBuffer:_vertexBuffer offset:0 atIndex:0];
    }
    
    void uploadQuad(ItemPool::Alloc* alloc) override {
        // no-op cos vertex buffer is shared between CPU and GPU
    }
    
    void renderPrivateSurface(Surface* privateSurface, ItemPool::Alloc* alloc) override {
        MetalSurface* privateMetalSurface = (MetalSurface*)privateSurface;
        MetalSurface* metalSurface = (MetalSurface*)_currentSurface;
        [metalSurface->_renderCommandEncoder setFragmentTexture:privateMetalSurface->_texture atIndex:0];
        [metalSurface->_renderCommandEncoder setFragmentSamplerState:privateMetalSurface->_sampler atIndex:0];

        drawQuads(1, alloc->offset);
        if (_currentTexture) {
            setCurrentTexture(_currentTexture);
        }
    }



};


void BlurRenderOp::render(Renderer* renderer, int numQuads, int vboOffset) {
    
}
void BlurRenderOp::rendererLoad(Renderer* renderer) {
    
}
void BlurRenderOp::rendererResize(Renderer* renderer) {
    
}
void BlurRenderOp::rendererUnload(Renderer* renderer) {
    
}


Renderer* Renderer::create(Window* window) {
    return new RendererApple(window);
}


#endif
#endif