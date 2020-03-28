//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

#if RENDERER_GL

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif
#ifndef GL_TEXTURE_MAX_LEVEL
#define GL_TEXTURE_MAX_LEVEL 0x813D
#endif
#ifndef GL_READ_FRAMEBUFFER_BINDING
#define GL_READ_FRAMEBUFFER GL_READ_FRAMEBUFFER_APPLE
#define GL_READ_FRAMEBUFFER_BINDING GL_READ_FRAMEBUFFER_BINDING_APPLE
#define GL_DRAW_FRAMEBUFFER GL_DRAW_FRAMEBUFFER_APPLE
#endif

// NB: NEVER leave GL error checking in release builds! Each API will stall the CPU
//     until the GPU catches up... very slow!
#if 0
#define check_gl(cmd, ...) cmd(__VA_ARGS__)
#else
static void checkGlErr(const char* file, int line, const char* cmd) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        app->log("GL ERROR: %s(%d) %s() err=0x%x", file, line, cmd, error);
    }
    // GL_INVALID_VALUE 0x501
}
#define check_gl(cmd, ...) cmd(__VA_ARGS__); checkGlErr(__FILE__, __LINE__, #cmd);
#endif

static GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = check_gl(glCreateShader, shaderType);
    if (shader) {
        string source =
#if TARGET_OS_IOS || defined(ANDROID) || defined(EMSCRIPTEN)
        "precision mediump float;\n";
#else
        "#version 120\n";
#endif
        source.append(pSource);
#if TARGET_OS_OSX || PLATFORM_LINUX
        // Surely to god there's a better way than this...!
        int x;
        while ((x = (int)source.find("highp")) < source.lengthInBytes()) source.erase(x, x+5);
        while ((x = (int)source.find("lowp")) < source.lengthInBytes()) source.erase(x, x+4);
        while ((x = (int)source.find("mediump")) < source.lengthInBytes()) source.erase(x, x+7);
#endif
        
        pSource = source.c_str();
        check_gl(glShaderSource, shader, 1, &pSource, NULL);
        check_gl(glCompileShader, shader);
        GLint compiled = 0;
        check_gl(glGetShaderiv, shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            check_gl(glGetShaderiv, shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    check_gl(glGetShaderInfoLog, shader, infoLen, NULL, buf);
                    printf("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                check_gl(glDeleteShader, shader);
                shader = 0;
            }
        }
    }
    return shader;
}



struct GLShaderState {
    GLuint _program;
    GLuint _vertexConfig;
    vector<GLuint> _uniformLocations;
    vector<bool> _validity;
    bytearray _uniformData;
    bool _uniformsValid;


    GLShaderState(Shader* shader) {
        
        string vs;
        if (shader->_features.textures[0] == Texture::Type::Rect) {
            vs += "#extension GL_ARB_texture_rectangle : require\n";
        }
        if (shader->_features.textures[0] == Texture::Type::OES) {
            vs += "#extension GL_OES_EGL_image_external : require\n";
        }
        
        
        for (auto& attribute : shader->_vertexShaderOutputs) {
            vs += "attribute ";
            vs += sl_getTypeString(attribute.type);
            vs += " ";
            vs += attribute.name;
            vs += ";\n";
            vs += "varying ";
            vs += sl_getTypeString(attribute.type);
            vs += " v_";
            vs += attribute.name;
            vs += ";\n";
        }

        for (auto& uniform : shader->_uniforms) {
            if (Shader::Uniform::Usage::Vertex == uniform.usage) {
                vs += "uniform ";
                vs += sl_getTypeString(uniform.type);
                vs+= " ";
                vs+= uniform.name;
                vs+= ";\n";
            }
        }

        
        vs += "void main() {\n"
              "  gl_Position = mvp * vec4(position,0,1);\n";
        
        // All other attributes
        for (auto& attribute : shader->_vertexShaderOutputs) {
            if (0!=attribute.name.compare("position")) {
                vs += string::format("v_%s = ", attribute.name.c_str());
                if (attribute.outValue.length() > 0) {
                    vs += attribute.outValue;
                } else {
                    vs += attribute.name;
                }
                vs += ";\n";
            }
        }
        
        vs += "}\n";

        // Attributes and uniforms for fragshader
        string fs = "";
        for (auto& attribute : shader->_vertexShaderOutputs) {
            if (0!=attribute.name.compare("position")) {
                fs += "varying ";
                fs += sl_getTypeString(attribute.type);
                fs += " v_";
                fs += attribute.name;
                fs += ";\n";
            }
        }
        for (auto& uniform : shader->_uniforms) {
            if (Shader::Uniform::Usage::Fragment == uniform.usage) {
                fs += "uniform ";
                fs += sl_getTypeString(uniform.type);
                fs+= " ";
                fs+= uniform.name;
                fs+= ";\n";
            }
        }
        
        // Add appropriate sampler
        if (shader->_features.textures[0] != Texture::Type::None) {
            fs += "uniform ";
            if (shader->_features.textures[0] == Texture::Type::Normal) {
                fs += "sampler2D texture;\n";
            }
            else if (shader->_features.textures[0] == Texture::Type::Rect) {
                fs += "sampler2DRect texture;\n";
            }
            else if (shader->_features.textures[0] == Texture::Type::OES) {
                fs += "samplerExternalOES texture;\n";
            }
            else {
                assert(0);
            }
        }

        fs += "void main() {\n"
              SL_HALF4 " c;\n";
        fs += shader->getFragmentSource();
        fs += "}\n";

        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vs.c_str());
        GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fs.c_str());

        _program = check_gl(glCreateProgram);
        
        _vertexConfig = VERTEXATTRIBS_CONFIG_NORMAL;
        check_gl(glBindAttribLocation, _program, VERTEXATTRIB_POSITION, "position");
        check_gl(glBindAttribLocation, _program, VERTEXATTRIB_TEXCOORD, "texcoord");
        check_gl(glBindAttribLocation, _program, VERTEXATTRIB_COLOR, "color");
        
        check_gl(glAttachShader, _program, vertexShader);
        check_gl(glAttachShader, _program, pixelShader);
        
        check_gl(glLinkProgram, _program);
        GLint linkStatus = GL_FALSE;
        check_gl(glGetProgramiv, _program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            check_gl(glGetProgramiv, _program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    check_gl(glGetProgramInfoLog,_program, bufLength, NULL, buf);
                    printf("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            check_gl(glDeleteProgram, _program);
            _program = 0;
        }
        
        if (!_program) {
            printf("Could not create program.");
            return;
        }
        
        _uniformLocations.resize(shader->_uniforms.size());
        _validity.resize(shader->_uniforms.size());
        int i=0;
        int32_t cb = 0;
        for (auto& uniform: shader->_uniforms) {
            uniform.offset = cb;
            cb += uniform.length();
            _uniformLocations[i] = check_gl(glGetUniformLocation, _program, uniform.name.c_str());
            i++;
        }
        _uniformData.resize(cb);
        memset(_uniformData.data(), 0, cb);
    }
    
    ~GLShaderState() {
         check_gl(glDeleteShader, _program);
    }
    
};


GLSurface::GLSurface(Renderer* renderer, bool isPrivate) : Surface(renderer, isPrivate) {
    _texture = new GLTexture(renderer, renderer->_primarySurfaceFormat);
}
GLSurface::~GLSurface() {
    if (_isPrivate && _fb) {
        check_gl(glDeleteFramebuffers, 1, &_fb);
        _fb = 0;
    }
}
void GLSurface::setSize(const SIZE& size) {
    if (size != _size) {
        Surface::setSize(size);
        _fbAttachmentsValid = false; // TODO: not sure this is necessary...
    }
}

    

GLTexture::GLTexture(Renderer* renderer, int format) : Texture(renderer, format) {
    _texTarget = GL_TEXTURE_2D;
}

struct GLTextureFormat {
    int pixelType;
    GLenum format, internalFormat;
};


static GLTextureFormat textureFormatFromPixelFormat(int pixelFormat) {
    switch (pixelFormat) {
        case PIXELFORMAT_RGBA32:
            return {
                .pixelType= GL_UNSIGNED_BYTE,
                .format= GL_RGBA,
                .internalFormat= GL_RGBA
            };

        case PIXELFORMAT_BGRA32:
            return {
                .pixelType= GL_UNSIGNED_BYTE,
                .format= GL_BGRA,
                .internalFormat= GL_RGBA
            };

        case PIXELFORMAT_RGB24:
            return {
                .pixelType = GL_UNSIGNED_BYTE,
                .format = GL_RGB,
                .internalFormat = GL_RGB
            };
            
        case PIXELFORMAT_RGB565:
            return {
                .pixelType = GL_UNSIGNED_SHORT_5_6_5,
                .format = GL_RGB,
                .internalFormat = GL_RGB
            };
            
        case PIXELFORMAT_A8:
            return {
                .pixelType = GL_UNSIGNED_BYTE,
#if PLATFORM_LINUX   // TODO: 'red' vs 'alpha' is probably not a Linux thing, it's more likely to be GL vs GL ES
                .format = GL_RED,
                .internalFormat = GL_RED
#else
                .format = GL_ALPHA,
                .internalFormat = GL_ALPHA
#endif
            };
    }

    assert(0);
}


bool GLTexture::readPixels(RECT rect, bytearray& target) const {
    
    GLuint fb, old_fb;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, (GLint*)&old_fb);
    check_gl(glGenFramebuffers, 1, &fb);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, fb);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texTarget, _textureId, 0);
    int stride = rect.size.width * 4;
    int cp = stride * rect.size.height;
    if (target.size() < cp) {
        target.resize(cp);
    }
    check_gl(glReadPixels, rect.origin.x, rect.origin.y,
                 rect.size.width, rect.size.height, GL_RGBA, GL_UNSIGNED_BYTE, target.data());
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, old_fb);
    check_gl(glDeleteFramebuffers, 1, &fb);
    return true;
}




    
class GLRenderTask : public RenderTask {
public:

    GLuint _unpackAlignment;

    GLRenderTask(Renderer* renderer) : RenderTask(renderer) {
    }
  
    bool bindToNativeSurface(Surface* surface) override {
        GLSurface* glsurf = (GLSurface*)surface;

        check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, (GLint*)&glsurf->_fb);
        

        // GL context init
        if (!glsurf->_doneInit) {
            glsurf->_doneInit = 1;

#if defined(GL_IMPLEMENTATION_COLOR_READ_FORMAT) && !PLATFORM_WEB // No web cos this is broken on Safari 12/13
            GLint pixType, pixFmt;
            check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_TYPE, &pixType);
            assert(GL_UNSIGNED_BYTE==pixType);
            check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_FORMAT, &pixFmt);
            if (pixFmt == GL_RGB) {
                _renderer->_primarySurfaceFormat = PIXELFORMAT_RGB24;
            } else if (pixFmt == GL_RGBA) {
                _renderer->_primarySurfaceFormat = PIXELFORMAT_RGBA32;
            } else if (pixFmt == GL_BGRA) {
                _renderer->_primarySurfaceFormat = PIXELFORMAT_BGRA32;
            } else {
                assert(0); //???
            }
#endif
            check_gl(glDepthMask, GL_TRUE);
            check_gl(glClear, GL_DEPTH_BUFFER_BIT);
            check_gl(glDepthMask, GL_FALSE);
            check_gl(glDisable, GL_DEPTH_TEST);

        }

        check_gl(glActiveTexture, GL_TEXTURE0);
        check_gl(glDisable, GL_SCISSOR_TEST);

        GLRenderer* r = (GLRenderer*)_renderer;
        r->bindVertexBuffer();

        _currentVertexConfig = 123;
        _currentShaderValid = false;
        _currentSurfaceValid = false;
        _blendParamsValid = false;
        _currentClipValid = false;
        _currentTextureValid = false;
        return true;
    }
    
 
    void setUniformData(int16_t uniformIndex, const void* data, int32_t cb) override {
        GLShaderState* state = (GLShaderState*)_currentShader->_shaderState;
        auto& uniform = _currentShader->_uniforms[uniformIndex];
        auto currentData = (state->_uniformData.data() + uniform.offset);
        if (0 == memcmp(currentData, data, cb)) {
            return;
        }
        memcpy(currentData, data, cb);
        state->_validity[uniformIndex] = false;
        state->_uniformsValid = false;
    }
    

    inline static GLint convertGLBlendFactor(BlendFactor blendFactor) {
        switch (blendFactor) {
            case BlendFactor::Zero: return GL_ZERO;
            case BlendFactor::SourceColor: return GL_SRC_COLOR;
            case BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
            case BlendFactor::DestinationColor: return GL_DST_COLOR;
            case BlendFactor::DestinationAlpha: return GL_DST_ALPHA;
            case BlendFactor::One: return GL_ONE;
            case BlendFactor::OneMinusSourceColor: return GL_ONE_MINUS_SRC_COLOR;
            case BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case BlendFactor::OneMinusDestinationColor: return GL_ONE_MINUS_DST_COLOR;
            case BlendFactor::OneMinusDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
        }
        assert(0);
    }

    void prepareTexture(Texture* tex, bool isRenderInput) {
        GLTexture* gltex = (GLTexture*)tex;
        
        // Always need a texture ID!
        if (!gltex->_textureId) {
            check_gl(glGenTextures, 1, &gltex->_textureId);
        }
                
        // Bind
        if (!_currentTextureValid || (_currentTexture != tex)) {
            check_gl(glBindTexture, gltex->_texTarget, gltex->_textureId);
            _currentTexture = tex;
            _currentTextureValid = true;
        }
        
        // Configure texture sampling config
        if (isRenderInput && !gltex->_samplingConfigValid) {
            gltex->_samplingConfigValid = true;

            GLuint minFilter;
            if (gltex->_maxMipMapLevel<=0) {
                minFilter = gltex->_minFilterLinear ? GL_LINEAR : GL_NEAREST;
            } else {
                if (gltex->_minFilterLinear) {
                    minFilter = gltex->_mipFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
                } else {
                    minFilter = gltex->_mipFilterLinear ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
                }
            }
            GLuint maxFilter = gltex->_magFilterLinear ? GL_LINEAR : GL_NEAREST;
            check_gl(glTexParameterf, gltex->_texTarget, GL_TEXTURE_MIN_FILTER, minFilter);
            check_gl(glTexParameterf, gltex->_texTarget, GL_TEXTURE_MAG_FILTER, maxFilter);
            check_gl(glTexParameteri, gltex->_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            check_gl(glTexParameteri, gltex->_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            if (gltex->_maxMipMapLevel > 0) {
        #if !PLATFORM_WEB // todo: should be if !WebGL 1.0.  This works in WebGL 2.
                check_gl(glTexParameteri, gltex->_texTarget, GL_TEXTURE_MAX_LEVEL, gltex->_maxMipMapLevel);
        #endif
            }
        }

        // Allocate and/or upload texture data
        if ((!gltex->_allocd || gltex->_needsUpload) && !gltex->_usesSharedMem) {
        
            GLTextureFormat texFormat = textureFormatFromPixelFormat(gltex->_format);
            
            // If there's no backing bitmap, just allocate texture RAM without a source to copy from
            if (!gltex->_bitmap) {
        #if PLATFORM_WEB
                if (texFormat.format == GL_BGRA) texFormat.format = GL_RGBA; // WebGL doesn't accept BGRA
        #endif
                check_gl(glTexImage2D, gltex->_texTarget, 0, texFormat.internalFormat, gltex->_size.width, gltex->_size.height, 0, texFormat.format, texFormat.pixelType, nullptr);
                gltex->_allocd = true;
                return;
            }

            // If there is a backing bitmap (and the texture RAM either hasn't been allocated or needs updating...)
            if (gltex->_bitmap) {

                // WebGL has some special APIs for fast uploading certain JS objects
        #if PLATFORM_WEB
                BitmapWeb* webBmp = (BitmapWeb*)gltex->_bitmap;
                webBmp->glTexImage2D(width, height); // TODO: move this function here where it belongs
        #else
                
                // Before we glTexImage2D() we have to tell it how rows are aligned
                int unpackAlignment;
                switch (gltex->_format) {
                    case PIXELFORMAT_RGB24: unpackAlignment = 1; break;
                    case PIXELFORMAT_RGB565: unpackAlignment = 2; break;
                    case PIXELFORMAT_A8: unpackAlignment = (gltex->_size.width&3) ? ((gltex->_size.width&1)?1:2) : 4; break;
                    default: unpackAlignment = 4; break;
                }
                if (unpackAlignment != _unpackAlignment) {
                    _unpackAlignment = unpackAlignment;
                    check_gl(glPixelStorei, GL_UNPACK_ALIGNMENT, unpackAlignment);
                }

                // TODO: Move to background thread! IMPORTANT!
                PIXELDATA pixeldata;
                gltex->_bitmap->lock(&pixeldata, false);
                if (!gltex->_allocd) {
                    check_gl(glTexImage2D, gltex->_texTarget, 0, texFormat.internalFormat, gltex->_size.width, gltex->_size.height, 0, texFormat.format, texFormat.pixelType, pixeldata.data);
                    gltex->_allocd = true;
                } else {
                    check_gl(glTexSubImage2D, gltex->_texTarget, 0, 0, 0, gltex->_size.width, gltex->_size.height, texFormat.format, texFormat.pixelType, pixeldata.data);
                }
                gltex->_bitmap->unlock(&pixeldata, false);
                
                gltex->_needsUpload = false;
            
        #endif

            }
        }
    }

    void bindCurrentSurface() {
        if (!_currentSurfaceValid) {
            _currentSurfaceValid = true;
            
            // Bind the surface framebuffer and set the viewport
            GLSurface* glsurface = (GLSurface*)_currentSurface;
            if (glsurface->_isPrivate && !glsurface->_fb) {
                check_gl(glGenFramebuffers, 1, &glsurface->_fb);
                assert(glsurface->_fb > 0);
            }
            check_gl(glBindFramebuffer, GL_FRAMEBUFFER, glsurface->_fb);
            check_gl(glViewport, 0, 0, _currentSurface->_size.width, _currentSurface->_size.height);

            // Check surface is capable of being a render target, i.e. has a framebuffer
            if (glsurface->_isPrivate && !glsurface->_fbAttachmentsValid) {
                GLTexture* gltex = glsurface->_texture.as<GLTexture>();
                prepareTexture(gltex, false);
                check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gltex->_texTarget, gltex->_textureId, 0);
                check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gltex->_texTarget, 0, 0);
                check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, gltex->_texTarget, 0, 0);
                GLenum x = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                assert(x==GL_FRAMEBUFFER_COMPLETE);
                glsurface->_fbAttachmentsValid = true;
            }


            // Clear
            if (_currentSurface->_clearNeeded) {
                float f[4] = {
                    (_currentSurface->_clearColor & 0xFF) / 255.0f,
                    ((_currentSurface->_clearColor & 0xFF00)>>8) / 255.0f,
                    ((_currentSurface->_clearColor & 0xFF0000)>>16) / 255.0f,
                    ((_currentSurface->_clearColor & 0xFF000000)>>24) / 255.0f
                };
                _currentSurface->_clearNeeded = false;
                check_gl(glClearColor, f[0], f[1], f[2], f[3]);
                check_gl(glClear, GL_COLOR_BUFFER_BIT);
            }
            _currentClipValid = false;
        };

    }
    
    void draw(PrimitiveType type, int count, int index) override {

        // If the surface being drawn to is "invalid"
        bindCurrentSurface();


        GLShaderState* state = (GLShaderState*)_currentShader->_shaderState;
        
        // Update shader
        if (!_currentShaderValid) {
            _currentShaderValid = true;
            GLShaderState* shaderState = (GLShaderState*)_currentShader->_shaderState;
            check_gl(glUseProgram, shaderState->_program);
            
            // These are program-specific...
            if (_currentVertexConfig != shaderState->_vertexConfig) {
                _currentVertexConfig = shaderState->_vertexConfig;
                check_gl(glVertexAttribPointer, VERTEXATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
                check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)8);
                //    check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VERTEX), (void*)8);
                check_gl(glVertexAttribPointer, VERTEXATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), (void*)16);
                
                check_gl(glEnableVertexAttribArray, VERTEXATTRIB_POSITION);
                check_gl(glEnableVertexAttribArray, VERTEXATTRIB_TEXCOORD);
                check_gl(glEnableVertexAttribArray, VERTEXATTRIB_COLOR);
            }
        }
        
        // Update blend
        if (!_blendParamsValid) {
            _blendParamsValid = true;

            if (_blendParams.op == BlendOp::None) {
                check_gl(glDisable, GL_BLEND);
            } else {
                GLenum srcRGB = convertGLBlendFactor(_blendParams.srcRGB);
                GLenum srcAlpha = convertGLBlendFactor(_blendParams.srcA);
                GLenum dstRGB = convertGLBlendFactor(_blendParams.dstRGB);
                GLenum dstAlpha = convertGLBlendFactor(_blendParams.dstA);
                check_gl(glBlendFuncSeparate, srcRGB, dstRGB, srcAlpha, dstAlpha);
                check_gl(glBlendEquation,(_blendParams.op == BlendOp::Add) ? GL_FUNC_ADD : GL_FUNC_SUBTRACT);
                check_gl(glEnable, GL_BLEND);
            }
            _currentTextureValid = false;
        }
        


        // Ensure uniforms are up to date
        if (!state->_uniformsValid) {
            state->_uniformsValid = true;
            for (int i=0 ; i<state->_uniformLocations.size() ; i++) {
                if (!state->_validity[i]) {
                    state->_validity[i] = true;
                    auto& uniform = _currentShader->_uniforms[i];
                    auto data = (state->_uniformData.data() + uniform.offset);
                    GLuint loc = state->_uniformLocations[i];
                    switch (uniform.type) {
                        case Shader::VariableType::Color: check_gl(glUniform4fv, loc, 1, (GLfloat*)data); break;
                        case Shader::VariableType::Int1: check_gl(glUniform1iv, loc, 1, (GLint*)data); break;
                        case Shader::VariableType::Float1: check_gl(glUniform1fv, loc, 1, (GLfloat*)data); break;
                        case Shader::VariableType::Float2: check_gl(glUniform2fv, loc, 1, (GLfloat*)data); break;
                        case Shader::VariableType::Float4: check_gl(glUniform4fv, loc, 1, (GLfloat*)data); break;
                        case Shader::VariableType::Matrix4: check_gl(glUniformMatrix4fv, loc, 1, false, (GLfloat*)data); break;
                    }

                }
            }
        }

        // Validate source texture
        if (_currentTexture) {
            GLTexture* gltex = (GLTexture*)_currentTexture;
            prepareTexture(gltex, true);
        }
        
        // Clip
        if (!_currentClipValid) {
            _currentClipValid = true;
            float x = _currentClip.left();
            float y = _currentClip.top();
            float width = _currentClip.size.width;
            float height = _currentClip.size.height;
            if (width<1 || height<1) {
                check_gl(glDisable, GL_SCISSOR_TEST);
            } else {
                check_gl(glEnable, GL_SCISSOR_TEST);
                check_gl(glScissor, x, _currentSurface->_size.height-(y+height), width, height);
            }
        }

        
        //   app->log("Drawing %d quads at once", numQuads);
        if (type == Quad) {
            check_gl(glDrawElements, GL_TRIANGLES, 6 * count, GL_UNSIGNED_SHORT, (void*)((index)*6*sizeof(GLshort)));
        } else if (type == Line) {
            //check_gl(glDrawElements, GL_LINES, 2 * count, GL_UNSIGNED_SHORT, (void*)((index)*4*sizeof(GLshort)));
            check_gl(glDrawArrays, GL_LINES, index, 2 * count);
        } else {
            assert(false);
        }

    }

    
    void copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin) override {
        bindCurrentSurface();
        RECT wrect = rect;
        //wrect.origin.y = 0;//rect.bottom();
        wrect.origin.y += _currentSurface->_size.height - rect.size.height;
        prepareTexture(destTex, true);
        check_gl(glCopyTexSubImage2D, ((GLTexture*)destTex)->_texTarget, 0, destOrigin.x, destOrigin.y, wrect.origin.x, wrect.origin.y, wrect.size.width, wrect.size.height);
    }

    void generateMipmaps(Texture* tex) override {
        GLTexture* gltex = (GLTexture*)tex;
        check_gl(glGenerateMipmap, gltex->_texTarget);
    }


    void commit(std::function<void()> onComplete) override {
        // TODO: implement proper command buffering etc
        check_gl(glFinish);
        if (onComplete) {
            Task::postToMainThread(onComplete);
        }
    }
    
    


};




void* GLRenderer::createShaderState(Shader* shader) {
    return new GLShaderState(shader);
}
void GLRenderer::deleteShaderState(void* state) {
    delete (GLShaderState*)state;
}



GLRenderer::GLRenderer() : Renderer() {
    _quadBuffer._resizeFunc = [=](int oldItemCount, int newItemCount) {
        
        // Realloc
        _quadBuffer._base = (uint8_t*)realloc(_quadBuffer._base, newItemCount*_quadBuffer._itemSize);

        // Grow the index buffer, copying the old one into place
        int cbIndexes = sizeof(GLshort) * 6 * newItemCount;
        GLshort* newIndexes = (GLshort*) malloc(cbIndexes);
        if (oldItemCount) {
            memcpy(newIndexes, _indexes, sizeof(GLshort) * 6 * oldItemCount);
            ::free(_indexes);
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
        _indexes = newIndexes;
        for (int i=oldItemCount ; i<newItemCount ; i++) {
            _indexes[i*6+0] = i*4+0; // A
            _indexes[i*6+1] = i*4+2; // C
            _indexes[i*6+2] = i*4+3; // D
            _indexes[i*6+3] = i*4+0; // A
            _indexes[i*6+4] = i*4+3; // D
            _indexes[i*6+5] = i*4+1; // B
        }
        
        _fullBufferUploadNeeded =true;

    };
    
}

void GLRenderer::initVertexBuffers() {
    
    // As long as we only have one quadbuffer we only need to bind the once so can do
    // that on the first frame and thereafter never again
#ifdef HAS_VAO
    check_gl(glGenVertexArrays, 1, &_vao);
    check_gl(glBindVertexArray, _vao);
#endif
    check_gl(glGenBuffers, 1, &_indexBufferId);
    check_gl(glGenBuffers, 1, &_vertexBufferId);
    check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    check_gl(glBindBuffer, GL_ARRAY_BUFFER, _vertexBufferId);

}

void GLRenderer::bindVertexBuffer() {
#ifndef HAS_VAO
    check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    check_gl(glBindBuffer, GL_ARRAY_BUFFER, _vertexBufferId);
#else
    check_gl(glBindVertexArray, _vao);
#endif
}

RenderTask* GLRenderer::createRenderTask() {
    if (!_indexBufferId) {
        initVertexBuffers();
    }
    return new GLRenderTask(this);
}

Texture* GLRenderer::createTexture(int format) {
    return new GLTexture(this, format);
}
void GLRenderer::releaseTexture(Texture* texture) {
    GLTexture* gltex = (GLTexture*)texture;
     if (gltex->_textureId) {
         check_gl(glDeleteTextures, 1, &gltex->_textureId);
         gltex->_textureId = 0;
     }
    Renderer::releaseTexture(texture);
 }




void GLRenderer::uploadQuad(ItemPool::Alloc* alloc) {
    check_gl(glBufferSubData, GL_ARRAY_BUFFER, alloc->offset*sizeof(QUAD), alloc->count*sizeof(QUAD), alloc->addr());
}

void GLRenderer::flushQuadBuffer() {
    if (_fullBufferUploadNeeded) {
        if (_quadBuffer._itemCount > 0 && _indexBufferId > 0) {

            check_gl(glBufferData, GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * 6 * _quadBuffer._itemCount, _indexes, GL_STATIC_DRAW);
            // TODO: On first use buffer is unused so pass NULL instead of _base
            check_gl(glBufferData, GL_ARRAY_BUFFER, sizeof(QUAD) * _quadBuffer._itemCount, _quadBuffer._base, GL_DYNAMIC_DRAW);
            _fullBufferUploadNeeded = false;
            _quadBufferDirtyLo = NULL;
            _quadBufferDirtyHi = NULL;
        }
    } else {
        if (_quadBufferDirtyLo) {
            check_gl(glBufferSubData, GL_ARRAY_BUFFER, _quadBufferDirtyLo-_quadBuffer._base, _quadBufferDirtyHi-_quadBufferDirtyLo, _quadBufferDirtyLo);
            _quadBufferDirtyLo = NULL;
            _quadBufferDirtyHi = NULL;
        }

    }
}







/**
 A note re. highp usage: Although vertex shader types are supposedly highp by default,
 on the iPhone 6S it turns out that unless you specify it you get wonky vertex arithmetic
 and horrible off-by-half-a-pixel rendering errors everywhere. Vertices must always be highp.
 */






int GLRenderer::getIntProperty(IntProperty property) {
    if (property == MaxVaryingFloats) {
        GLint val = 0;
#if PLATFORM_WEB || PLATFORM_IOS || PLATFORM_ANDROID
        check_gl(glGetIntegerv, GL_MAX_VARYING_VECTORS, &val);
        val*=4;
#else
        check_gl(glGetIntegerv, GL_MAX_VARYING_FLOATS, &val);
#endif
        return val;
    }
    assert(0);
    return 0;
}


/*
 class GLProgramRoundRectComplex : public GLProgramRoundRect {
 public:
 
 void load() override {
 
 loadShaders(
 TEXTURE_VERTEX_SHADER
 ,
 "varying lowp vec4 v_color;\n"
 "uniform mediump float alpha;\n"
 "varying vec2 v_texcoord;\n" // not actually texture coords, this is x-dist and y-dist from quad centre
 "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
 "uniform mediump vec4 radii;\n"
 "uniform lowp vec4 strokeColor;\n"
 
 
 "void main() {\n"
 
 // Branchless selection of radius
 // NB: step(val,edge) ==  ((val < edge) ? 0.0 : 1.0)
 // NB: radii.x=top left, radii.y=top right, radii.z=bot left, radii.w = bot right
 "   float l=step(v_texcoord.x,0.0);"
 "   float r=1.0-l;"
 "   float t=step(v_texcoord.y,0.0);"
 "   float b=1.0-t;"
 "   float radius = t*l*radii.x + t*r*radii.y + b*l*radii.z + b*r*radii.w;\n"
 
 // Round rect distance function
 "    vec2 foo = max(abs(v_texcoord) - (u.xy - vec2(radius)), vec2(0.0)); \n"
 "    float dist = 1.0 - clamp(length(foo) - radius, 0.0, 1.0);"
 
 // Blend stroke & fill (TODO: stroke)
 "    vec4 col = v_color;\n"
 "    col.a *= dist;\n"
 "    col.a *= alpha;\n"
 "    gl_FragColor = col;\n"
 "}\n"
 );
 }
 
 };
 */





// Drop shadows? Refer to http://madebyevan.com/shaders/fast-rounded-rectangle-shadows/



#endif

