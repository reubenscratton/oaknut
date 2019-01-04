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
        app.log("GL ERROR: %s(%d) %s() err=0x%x", file, line, cmd, error);
    }
}
#define check_gl(cmd, ...) cmd(__VA_ARGS__); checkGlErr(__FILE__, __LINE__, #cmd);
#endif


class GLSurface : public Surface {
public:
    GLuint _fb;
    GLuint _tex;
    GLint _pixelType;
    GLint _pixelFormat;
    
    GLSurface(bool isPrivate) : Surface(isPrivate) {
        if (isPrivate) {
#ifndef PLATFORM_MACOS
            check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_TYPE, &_pixelType);
            check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_FORMAT, &_pixelFormat);
#endif
            if (_pixelFormat == 0) {
                _pixelFormat = GL_RGBA;
                _pixelType = GL_UNSIGNED_BYTE;
            }
            check_gl(glGenTextures, 1, &_tex);
            check_gl(glGenFramebuffers, 1, &_fb);

        } else {
#if TARGET_OS_IOS || defined(PLATFORM_LINUX)
            _fb = 1;
#else
            _fb = 0;
#endif
        }
    }
    ~GLSurface() {
        if (_tex) {
            check_gl(glDeleteTextures, 1, &_tex);
            _tex = 0;
            check_gl(glDeleteFramebuffers, 1, &_fb);
            _fb = 0;
        }
    }

    void setSize(const SIZE& size) override {
        Surface::setSize(size);
        if (!_isPrivate) {
            return;
        }

        GLint oldFBO, oldTex;
        check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
        check_gl(glGetIntegerv, GL_TEXTURE_BINDING_2D, &oldTex);
        check_gl(glBindTexture, GL_TEXTURE_2D, _tex);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLint pixelFormat = _pixelFormat;
        if (pixelFormat == GL_BGRA) pixelFormat = GL_RGBA; // WebGL no like iOS format
        check_gl(glTexImage2D, GL_TEXTURE_2D, 0, pixelFormat, size.width, size.height, 0, pixelFormat, _pixelType, NULL);
        check_gl(glBindTexture, GL_TEXTURE_2D, oldTex);
        
        check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        //GLenum x = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        //assert(x==GL_FRAMEBUFFER_COMPLETE);
        check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);

    }
    

    
};


Surface* GLRenderer::getPrimarySurface() {
    return new GLSurface(false); // todo: make this a singleton member of GLRenderer
}

Surface* GLRenderer::createPrivateSurface() {
    return new GLSurface(true);
}



GLTexture::GLTexture(Bitmap* bitmap) : Texture(bitmap) {
    _allocdTexData = false;
    _needsUpload = true;
    _texTarget = GL_TEXTURE_2D;
    _textureId = 0;
}



void GLTexture::upload() {
    PIXELDATA pixeldata;
    _bitmap->lock(&pixeldata, false);
    int glPixelType;
    GLenum glFormat, glInternalFormat;

    switch (_bitmap->_format) {
        case BITMAPFORMAT_RGBA32: {
            glPixelType = GL_UNSIGNED_BYTE;
            glFormat = GL_RGBA;
            glInternalFormat = GL_RGBA;
            break;
        }
        case BITMAPFORMAT_BGRA32: {
            glPixelType = GL_UNSIGNED_BYTE;
            glFormat = GL_BGRA;
            glInternalFormat = GL_RGBA;
            break;
        }
        case BITMAPFORMAT_RGB565: {
            glPixelType = GL_UNSIGNED_SHORT_5_6_5;
            glFormat = GL_RGB;
            glInternalFormat = GL_RGB;
            break;
        }
        case BITMAPFORMAT_A8: {
            glPixelType = GL_UNSIGNED_BYTE;
#ifdef PLATFORM_LINUX   // TODO: 'red' vs 'alpha' is probably not a Linux thing, it's more likely to be GL vs GL ES
            glFormat = GL_RED;
            glInternalFormat = GL_RED;
#else
            glFormat = GL_ALPHA;
            glInternalFormat = GL_ALPHA;
#endif
            break;
        }
        default: assert(0);
    }

    if (!_allocdTexData) {
        _allocdTexData = true;
        check_gl(glTexImage2D, _texTarget, 0, glInternalFormat,
                 _bitmap->_width, _bitmap->_height, 0, glFormat, glPixelType, pixeldata.data);
    } else {
        check_gl(glTexSubImage2D, _texTarget, 0, 0, 0, _bitmap->_width, _bitmap->_height, glFormat, glPixelType, pixeldata.data);
    }
    _bitmap->unlock(&pixeldata, false);
}

void GLTexture::unload() {
    if (_textureId) {
        _allocdTexData = false;
        _needsUpload = true;
        check_gl(glDeleteTextures, 1, &_textureId);
        _textureId = 0;
    }
}

void GLTexture::bind() {
    if (!_textureId) {
        check_gl(glGenTextures, 1, &_textureId);
        assert(_textureId>0);
    }
    check_gl(glBindTexture, _texTarget, _textureId);
    if (!_paramsValid) {
        GLuint method = _bitmap->_sampleNearest ? GL_NEAREST : GL_LINEAR;
        check_gl(glTexParameterf, _texTarget, GL_TEXTURE_MIN_FILTER, method);
        check_gl(glTexParameterf, _texTarget, GL_TEXTURE_MAG_FILTER, method);
        check_gl(glTexParameteri, _texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, _texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _paramsValid = true;
    }
    if (_needsUpload) {
        _needsUpload = false;
        upload();
    }
}

int GLTexture::getSampler() {
    if (_texTarget == GL_TEXTURE_EXTERNAL_OES) {
        return GLSAMPLER_TEXTURE_EXT_OES;
    }
    return GLSAMPLER_TEXTURE_2D;
}




template <class T>
class Uniform {
public:
    GLint position;  // as returned by glGetUniformLocation
    
    T val;
    bool dirty;
    Uniform() {
        dirty = true;
    }
    virtual void set(T val) {
        if (val != this->val) {
            this->val = val;
            dirty = true;
        }
    }
    virtual void use() {
        if (dirty) {
            load();
            dirty = false;
        }
    }
    void load();
};

template<>
void Uniform<int>::load() {
    check_gl(glUniform1i, position, val);
}

template<>
void Uniform<float>::load() {
    check_gl(glUniform1f, position, val);
};

template<>
void Uniform<VECTOR2>::load() {
    check_gl(glUniform2f, position, val.x, val.y);
}

template<>
void Uniform<COLOR>::load() {
    float c[4];
    c[3] = ((val&0xff000000)>>24)/255.0f;
    c[2] = (val&0xff)/255.0f;
    c[1] = ((val&0xff00)>>8)/255.0f;
    c[0] = ((val&0xff0000)>>16)/255.0f;
    check_gl(glUniform4f, position, c[0], c[1], c[2], c[3]);
}

template<>
void Uniform<VECTOR4>::load() {
    check_gl(glUniform4f, position, val.x, val.y, val.z, val.w);
}

template<>
void Uniform<MATRIX4>::load() {
    check_gl(glUniformMatrix4fv, position, 1, 0, val.get());
}


template<>
void Uniform<POINT>::load() {
    check_gl(glUniform2f, position, val.x, val.y);
}





class GLShaderBase : public Shader {
public:
    bool _loaded;

    GLuint _program;
    GLuint _vertexConfig;
    Uniform<MATRIX4> _mvp;
    Uniform<float> _alpha;

    GLShaderBase(ShaderFeatures features) : Shader(features) {
    }
    
    virtual void findVariables() {
        _mvp.position = check_gl(glGetUniformLocation, _program, "mvp");
    }
    
    void configureForRenderOp(RenderOp* op, const MATRIX4& mvp) override {
        _mvp.set(mvp);
        _mvp.use();
        if (_features.alpha) {
            _alpha.set(op->_alpha);
            _alpha.use();
        }
    }
    
    void unload() override {
        if (_loaded) {
            _loaded = false;
            check_gl(glDeleteShader, _program);
            _mvp.dirty = true;
        }
    }


protected:
    
    void loadShaders(const char* szVertexShader, const char* szFragShader) {
        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, szVertexShader);
        GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, szFragShader);
        _program = check_gl(glCreateProgram);
        
        _vertexConfig = VERTEXATTRIBS_CONFIG_NORMAL;
        check_gl(glBindAttribLocation, _program, VERTEXATTRIB_POSITION, "vPosition");
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
        findVariables();
    }
    GLuint loadShader(GLenum shaderType, const char* pSource) {
        GLuint shader = check_gl(glCreateShader, shaderType);
        if (shader) {
            string source =
#if TARGET_OS_IOS || defined(ANDROID) || defined(EMSCRIPTEN)
            "precision mediump float;\n";
            //"precision highp vec2;\n"
            //"precision highp mat4;\n"
            //"precision lowp vec4;\n";
#else
            "#version 120\n";
#endif
            source.append(pSource);
#if TARGET_OS_OSX || PLATFORM_LINUX
            // Surely to god there's a better way than this...!
            int x;
            while ((x = (int)source.find("highp")) >= 0) source.erase(x, x+5);
            while ((x = (int)source.find("lowp")) >= 0) source.erase(x, x+4);
            while ((x = (int)source.find("mediump")) >= 0) source.erase(x, x+7);
#endif
            
            pSource = source.data();
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


};

class GLShader : public GLShaderBase {
public:
    
    Uniform<int> _sampler;
    Uniform<COLOR> _strokeColor;
    Uniform<VECTOR4> _u;
    Uniform<float> _radius;
    Uniform<VECTOR4> _radii;
    
    GLShader(ShaderFeatures features) : GLShaderBase(features) {
    }

    void load() override {
        bool useTexCoords = false;
        bool useTexSampler = false;
        int roundRect = _features.roundRect;
        if (roundRect) {
            useTexCoords = true; // we don't use the sampler, we just want the texcoord attributes, which are
                                 // not actually texture coords, v_texcoords is x-dist and y-dist from quad centre
        }
        if (_features.sampler0 != GLSAMPLER_NONE) {
            useTexSampler = true;
            useTexCoords = true;
            _sampler.set(0);
        }
        
        string vs;
        
        if (_features.sampler0 == GLSAMPLER_TEXTURE_EXT_OES) {
            vs += "#extension GL_OES_EGL_image_external : require\n";
        }
        vs += "attribute highp vec2 vPosition;\n"     // the 'highp' qualifier is VERY IMPORTANT! See above
        "uniform highp mat4 mvp;\n"
        "attribute lowp vec4 color;\n"
        "varying lowp vec4 v_color;\n";
        if (useTexCoords) {
            vs += "attribute vec2 texcoord;\n"
            "varying vec2 v_texcoord;\n";
        }
        
        vs += "void main() {\n"
        "  gl_Position = mvp * vec4(vPosition,0,1);\n"
        "  v_color=color;\n";
        if (useTexCoords) {
            vs += "  v_texcoord = texcoord;\n";
        }
        vs += "}\n";
        
        
        
        
        string fs = "varying lowp vec4 v_color;\n";
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
        if (useTexCoords) {
            fs += "varying vec2 v_texcoord;\n";
        }

        if (_features.sampler0 == GLSAMPLER_TEXTURE_2D) {
            fs += "uniform sampler2D texture;\n";
        }
        if (_features.sampler0 == GLSAMPLER_TEXTURE_EXT_OES) {
            fs += "uniform samplerExternalOES texture;\n";
        }
        
        
        
        
        fs += "void main() {\n";
        
        string color_src = "v_color";
        if (useTexSampler) {
            fs += "    vec4 color = texture2D(texture, v_texcoord);\n";
            if (_features.tint) {
                fs += "    color.rgb = v_color.rgb;\n";
            }
            color_src = "color";
        }
        
        
        
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
        
        
        loadShaders(vs.data(), fs.data());
    }
    

    void unload() override {
        GLShaderBase::unload();
        _sampler.dirty = true;
        _alpha.dirty = true;
        if (_features.roundRect) {
            _strokeColor.dirty = true;
            _u.dirty = true;
            if (_features.roundRect == SHADER_ROUNDRECT_1) {
                _radius.dirty = true;
            } else {
                _radii.dirty = true;
            }
        }
    }

    void findVariables() override {
        GLShaderBase::findVariables();
        if (_features.sampler0 != GLSAMPLER_NONE) {
            _sampler.position = check_gl(glGetUniformLocation, _program, "texture");
        }
        if (_features.alpha) {
            _alpha.position = check_gl(glGetUniformLocation, _program, "alpha");
        }
        if (_features.roundRect) {
            _strokeColor.position = check_gl(glGetUniformLocation, _program, "strokeColor");
            _u.position = check_gl(glGetUniformLocation, _program, "u");
            if (_features.roundRect == SHADER_ROUNDRECT_1) {
                _radius.position = check_gl(glGetUniformLocation, _program, "radius");
            } else {
                _radii.position = check_gl(glGetUniformLocation, _program, "radii");
            }
        }
    }
    
    
    
    void configureForRenderOp(RenderOp* op, const MATRIX4& mvp) override {
        GLShaderBase::configureForRenderOp(op, mvp);
        if (_features.alpha) {
            _alpha.set(op->_alpha);
        }
        if (_features.sampler0) {
            _sampler.use();
        }
        if (_features.roundRect) {
            RectRenderOp* rectOp = (RectRenderOp*)op;
            _strokeColor.set(rectOp->_strokeColor);
            _strokeColor.use();
            _u.set(VECTOR4(op->_rect.size.width/2,op->_rect.size.height/2,0,rectOp->_strokeWidth));
            _u.use();
            if (_features.roundRect == SHADER_ROUNDRECT_1) {
                _radius.set(rectOp->_radii[0]);
                _radius.use();
            } else {
                _radii.set(rectOp->_radii);
                _radii.use();
            }
        }
    }
    
    
};

void GLRenderer::setCurrentSurface(Surface* surface) {
    if (surface != _currentSurface) {
        _currentSurface = surface;
        GLSurface* glsurface = (GLSurface*)surface;
        check_gl(glBindFramebuffer, GL_FRAMEBUFFER, glsurface->_fb);
        check_gl(glViewport, 0, 0, surface->_size.width, surface->_size.height);
    }
}


void GLRenderer::setActiveShader(Shader* shader) {
    GLShaderBase* glshader = (GLShaderBase*)shader;
    if (!glshader->_loaded) {
        glshader->_loaded = true;
        glshader->load();
    }
    if (_currentShader != shader) {
        _currentShader = shader;
        check_gl(glUseProgram, glshader->_program);
    }
    
    // These are program-specific...
    if (_currentVertexConfig != glshader->_vertexConfig) {
        _currentVertexConfig = glshader->_vertexConfig;
        check_gl(glVertexAttribPointer, VERTEXATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
        check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)8);
        //    check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VERTEX), (void*)8);
        check_gl(glVertexAttribPointer, VERTEXATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), (void*)16);
        
        check_gl(glEnableVertexAttribArray, VERTEXATTRIB_POSITION);
        check_gl(glEnableVertexAttribArray, VERTEXATTRIB_TEXCOORD);
        check_gl(glEnableVertexAttribArray, VERTEXATTRIB_COLOR);
    }
}

GLRenderer::GLRenderer(Window* window) : Renderer(window) {
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

Texture* GLRenderer::createTexture(Bitmap *bitmap) {
    return new GLTexture(bitmap);
}

void GLRenderer::setBlendMode(int blendMode) {
    if (blendMode != _blendMode) {
        if (blendMode == BLENDMODE_NONE) {
            check_gl(glDisable, GL_BLEND);
        } else {
            if (blendMode == BLENDMODE_NORMAL) {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // normal alpha blend
            } else {
                glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
            }
            if (_blendMode == BLENDMODE_NONE) {
                check_gl(glEnable, GL_BLEND);
            }
        }
        _blendMode = blendMode;
    }
}

void GLRenderer::pushClip(RECT clip) {
    bool firstClip = _clips.size()==0;
    if (!firstClip) {
        clip.intersectWith(_clips.top());
    } else {
        check_gl(glEnable, GL_SCISSOR_TEST);
    }
    _clips.push(clip);
    glScissor(clip.left(), clip.top(), clip.size.width, clip.size.height);
}
void GLRenderer::popClip() {
    assert(_clips.size()>0);
    RECT clip = _clips.top();
    _clips.pop();
    glScissor(clip.left(), clip.top(), clip.size.width, clip.size.height);
    if (_clips.size() == 0) {
        check_gl(glDisable, GL_SCISSOR_TEST);
    }
}

void GLRenderer::uploadQuad(ItemPool::Alloc* alloc) {
    check_gl(glBufferSubData, GL_ARRAY_BUFFER, alloc->offset*sizeof(QUAD), alloc->count*sizeof(QUAD), alloc->addr());
}

void GLRenderer::renderPrivateSurface(Surface* privateSurface, ItemPool::Alloc* alloc) {
    GLSurface* surface = (GLSurface*)privateSurface;
    check_gl(glBindTexture, GL_TEXTURE_2D, surface->_tex);
    check_gl(glDrawElements, GL_TRIANGLES, 6 * 1, GL_UNSIGNED_SHORT, (void*)((alloc->offset)*6*sizeof(GLshort)));
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

void GLRenderer::drawQuads(int numQuads, int index) {
    //   app.log("Drawing %d quads at once", numQuads);
    check_gl(glDrawElements, GL_TRIANGLES, 6 * numQuads, GL_UNSIGNED_SHORT, (void*)((index)*6*sizeof(GLshort)));
}

void GLRenderer::prepareToDraw() {
    // GL context init
    if (!_doneInit) {
        _doneInit = 1;
        check_gl(glDepthMask, GL_TRUE);
        check_gl(glClear, GL_DEPTH_BUFFER_BIT);
        check_gl(glDepthMask, GL_FALSE);
        check_gl(glDisable, GL_DEPTH_TEST);
        check_gl(glActiveTexture, GL_TEXTURE0);
        _blendMode = BLENDMODE_NONE;
        check_gl(glDisable, GL_BLEND);
        //_enabledFlags.scissorTest = 0;
        check_gl(glDisable, GL_SCISSOR_TEST);
        
        // As long as we only have one quadbuffer we only need to bind the once so can do
        // that on the first frame and thereafter never again
        if (_indexBufferId == 0) {
#ifdef HAS_VAO
            check_gl(glGenVertexArrays, 1, &_vao);
            check_gl(glBindVertexArray, _vao);
#endif
            check_gl(glGenBuffers, 1, &_indexBufferId);
            check_gl(glGenBuffers, 1, &_vertexBufferId);
            check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
            check_gl(glBindBuffer, GL_ARRAY_BUFFER, _vertexBufferId);
        }
#ifndef HAS_VAO
        check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        check_gl(glBindBuffer, GL_ARRAY_BUFFER, _vertexBufferId);
#else
        check_gl(glBindVertexArray, _vao);
#endif
        _currentVertexConfig = 0;
        
        COLOR windowBackgroundColor = app.getStyleColor("window.background-color");
        _backgroundColor[0] = (windowBackgroundColor & 0xFF) / 255.0f;
        _backgroundColor[1] = ((windowBackgroundColor & 0xFF00)>>8) / 255.0f;
        _backgroundColor[2] = ((windowBackgroundColor & 0xFF0000)>>16) / 255.0f;
        _backgroundColor[3] = ((windowBackgroundColor & 0xFF000000)>>24) / 255.0f;
        check_gl(glClearColor, _backgroundColor[0],_backgroundColor[1],_backgroundColor[2],_backgroundColor[3]);
    }
    if (_backgroundColor[3] > 0.f) {
        check_gl(glClear, GL_COLOR_BUFFER_BIT);
    }
    
    _renderCounter++;
    _currentSurface = NULL;
    _currentTexture = NULL;
    _currentShader = NULL;
}




void GLRenderer::convertTexture(GLTexture* texture, int width, int height) {
    
    // TODO: glGets are BAD IDEA and kill performance. Need a GLContext structure that caches all GL properties
    GLint oldFBO, oldFBOread, oldTex;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
    check_gl(glGetIntegerv, GL_READ_FRAMEBUFFER_BINDING, &oldFBOread);
    check_gl(glGetIntegerv, GL_TEXTURE_BINDING_2D, &oldTex);
    
    // Set up the camera texture for framebuffer read
    GLuint fbr = 0;
    check_gl(glGenFramebuffers, 1, &fbr);
    check_gl(glBindFramebuffer, GL_READ_FRAMEBUFFER, fbr);
    check_gl(glFramebufferTexture2D, GL_READ_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,texture->_texTarget,texture->_textureId,0);
#if !PLATFORM_ANDROID
    check_gl(glReadBuffer, GL_COLOR_ATTACHMENT0);
#endif

    // Create a new GL_TEXTURE_2D texture for framebuffer write
    GLuint fb = 0;
    check_gl(glGenFramebuffers, 1, &fb);
    check_gl(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, fb);
    GLuint texId2 = 0;
    check_gl(glGenTextures, 1, &texId2);
    check_gl(glBindTexture, GL_TEXTURE_2D, texId2);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    check_gl(glFramebufferTexture2D, GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texId2,0);
    
    // Copy
    check_gl(glCopyTexSubImage2D, GL_TEXTURE_2D,0,0,0,0,0,width,height);
    texture->_texTarget = GL_TEXTURE_2D;
    texture->_textureId = texId2;
    
    check_gl(glBindTexture, GL_TEXTURE_2D, oldTex);
    check_gl(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, oldFBO);
    check_gl(glBindFramebuffer, GL_READ_FRAMEBUFFER, oldFBOread);
    check_gl(glDeleteFramebuffers, 1, &fbr);
    check_gl(glDeleteFramebuffers, 1, &fb);
}


/**
 A note re. highp usage: Although vertex shader types are supposedly highp by default,
 on the iPhone 6S it turns out that unless you specify it you get wonky vertex arithmetic
 and horrible off-by-half-a-pixel rendering errors everywhere. Vertices must always be highp.
 */

const char* TEXTURE_VERTEX_SHADER =
"attribute highp vec2 vPosition;\n"
"uniform highp mat4 mvp;\n"
"attribute lowp vec4 color;\n"
"varying lowp vec4 v_color;\n"
"attribute vec2 texcoord;\n"
"varying vec2 v_texcoord;\n"
"void main() {\n"
"  gl_Position = mvp * vec4(vPosition,0,1);\n"
"  v_texcoord = texcoord;\n"
"  v_color=color;\n"
"}\n";





Shader* GLRenderer::getShader(ShaderFeatures features) {
    Shader* shader = _shaders[features.all];
    if (!shader) {
        shader = new GLShader(features);
        _shaders[features.all] = shader;
    }
    return shader;
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

/*
 // TODO: Desaturation should be an optional shader attribute once we have shader cache done
 void GLProgramTextureDesaturated::load()  {
 loadShaders(TEXTURE_VERTEX_SHADER,
 "varying vec2 v_texcoord;\n"
 "uniform sampler2D texture;\n"
 "void main() {\n"
 "    vec4 color = texture2D(texture, v_texcoord);\n"
 "    float lum = color.r * 0.299 + color.g * 0.587 + color.b * 0.144;\n"
 "    gl_FragColor = mix(color, vec4(lum,lum,lum, 1.0), 0.0);\n"
 "}\n"
 );
 }
 */


class GLBlurShader : public GLShaderBase {
public:
    
    GLuint _posTexOffset;
    BlurRenderOp* _op;
    
    GLBlurShader(BlurRenderOp* op) : GLShaderBase(ShaderFeatures()), _op(op) {
    }

    void load() override {
    
        
        int numOptimizedOffsets = (int)_op->_optimizedOffsets.size();
        
        char ach[128];
        
        string vertexShader =
        "attribute highp vec2 vPosition;\n"
        "uniform highp mat4 mvp;\n"
        "attribute vec2 texcoord;\n"
        "uniform vec2 texOffset;\n";
        
        sprintf(ach, "varying vec2 blurCoordinates[%d];\n", 1 + numOptimizedOffsets * 2);
        vertexShader.append(ach);
        
        vertexShader.append("void main() {\n"
                            "   gl_Position = mvp * vec4(vPosition,0,1);\n");
        vertexShader.append("   blurCoordinates[0] = texcoord.xy;\n");
        for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
            sprintf(ach,
                    "   blurCoordinates[%lu] = texcoord.xy + texOffset * %f;\n"
                    "   blurCoordinates[%lu] = texcoord.xy - texOffset * %f;\n",
                    (unsigned long)((i * 2) + 1), _op->_optimizedOffsets[i],
                    (unsigned long)((i * 2) + 2), _op->_optimizedOffsets[i]);
            vertexShader.append(ach);
        }
        
        vertexShader.append("}\n");
        
        
        
        string fragShader =
        "uniform sampler2D texture;\n"
        "uniform vec2 texOffset;\n";
        sprintf(ach,
                "varying highp vec2 blurCoordinates[%d];\n", 1 + numOptimizedOffsets * 2);
        fragShader.append(ach);
        
        fragShader.append("void main() {\n");
        
        // Inner texture loop
        sprintf(ach, "lowp vec4 c = texture2D(texture, blurCoordinates[0]) * %f;\n", _op->_standardGaussianWeights[0]);
        fragShader.append(ach);
        for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
            GLfloat firstWeight = _op->_standardGaussianWeights[i * 2 + 1];
            GLfloat secondWeight = _op->_standardGaussianWeights[i * 2 + 2];
            GLfloat optimizedWeight = firstWeight + secondWeight;
            sprintf(ach, "c += texture2D(texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((i * 2) + 1), optimizedWeight);
            fragShader.append(ach);
            sprintf(ach, "c += texture2D(texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((i * 2) + 2), optimizedWeight);
            fragShader.append(ach);
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
                
                sprintf(ach, "c += texture2D(texture, blurCoordinates[0] + texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
                fragShader.append(ach);
                sprintf(ach, "c += texture2D(texture, blurCoordinates[0] - texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
                fragShader.append(ach);
            }
        }
        
        fragShader.append("   gl_FragColor = c;\n"
                          "}\n");
        
        loadShaders(vertexShader.data(), fragShader.data());
        
        _posTexOffset = glGetUniformLocation(_program, "texOffset");
    }
 
};



class GLPostBlurShader : public GLShaderBase {
public:
    GLPostBlurShader() : GLShaderBase(ShaderFeatures()) {
    }
    
    void load() override {
        loadShaders(TEXTURE_VERTEX_SHADER,
                    "varying vec2 v_texcoord;\n"
                    "varying lowp vec4 v_color;\n"
                    "uniform sampler2D texture;\n"
                    "const lowp vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);\n"
                    "void main() {\n"
                    // Desaturate
                    "   lowp vec4 c = mix(texture2D(texture, v_texcoord), v_color, 0.9);\n"
                    "   lowp float lum = dot(c.rgb, luminanceWeighting);\n"
                    "   lowp float lumRatio = ((0.5 - lum) * 0.1);\n"
                    "   gl_FragColor = vec4(mix(vec3(lum), c.rgb, 0.8) + lumRatio, 1.0);\n"
                    "}\n");
    }
    
};





void BlurRenderOp::rendererLoad(Renderer* renderer) {
    check_gl(glGenTextures, 3, _textureIds);
    for (int i=0 ; i<3 ; i++) {
        check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[i]);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    
    // Set up a two private FBOs and bind textures 1 and 2 to them
    GLint oldFBO;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
    check_gl(glGenFramebuffers, 2, _fb);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[0]);
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[1]);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureIds[1], 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[1]);
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[2]);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureIds[2], 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
    
    _blurShader = new GLBlurShader(this);
    _shader = new GLPostBlurShader();
}

void BlurRenderOp::rendererUnload(Renderer* renderer) {
    if (_textureIds[0]) {
        check_gl(glDeleteTextures, 3, _textureIds);
        check_gl(glDeleteFramebuffers, 2, _fb);
    }
}


void BlurRenderOp::rendererResize(Renderer* renderer) {
    
    
    GLint otex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &otex);
    

    //
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[0]);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _fullSizePow2.width, _fullSizePow2.height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
    
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[1]);
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _downsampledSize.width, _downsampledSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[2]);
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _downsampledSize.width, _downsampledSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, otex);
}


void BlurRenderOp::render(Renderer* renderer, int numQuads, int vboOffset) {
    
    _blurShader->configureForRenderOp(this, _mvp);
    
    GLint viewport[4];
    check_gl(glGetIntegerv, GL_VIEWPORT, viewport);
    
    // Copy the area of framebuffer to be blurred to a private pow2 texture and
    // generate 2 levels of mipmap (ie 1/2 and 1/4 sizes)
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[0]);
    RECT wrect = _rect;
    wrect.origin.y += viewport[3] - _rect.size.height;
    check_gl(glCopyTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, wrect.origin.x, wrect.origin.y, wrect.size.width, wrect.size.height);
    check_gl(glGenerateMipmap, GL_TEXTURE_2D);
    
    // Switch to private FBO
    GLint oldFBO;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
    
    // 1D blur horizontally, source is 1/4-sized mipmap and destination is fb0
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[0]);
    GLBlurShader* blurShader = (GLBlurShader*)_blurShader._obj;
    check_gl(glUniform2f, blurShader->_posTexOffset, 0, 1.f/_downsampledSize.height);
    glViewport(0, 0, _downsampledSize.width, _downsampledSize.height);
    check_gl(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)((_alloc->offset)*6*sizeof(GLshort)));
    
    // 1D blur vertically
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[1]);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[1]);
    check_gl(glUniform2f, blurShader->_posTexOffset, 1.f/_downsampledSize.width, 0);
    check_gl(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)((_alloc->offset)*6*sizeof(GLshort)));
    
    // Switch back to rendering to the backbuffer
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
    check_gl(glViewport, viewport[0], viewport[1], viewport[2], viewport[3]);
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[2]);
    
    // Normal prepareRender using the post-blur shader
    renderer->setBlendMode(_blendMode);
    renderer->setActiveShader(_shader);
    _shader->configureForRenderOp(this, *_pmvp);

    RenderOp::render(renderer, numQuads, vboOffset);

}



// Drop shadows? Refer to http://madebyevan.com/shaders/fast-rounded-rectangle-shadows/



#endif

