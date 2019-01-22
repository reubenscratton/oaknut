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

static GLuint loadShader(GLenum shaderType, const char* pSource) {
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



struct GLShaderState {
    GLuint _program;
    GLuint _vertexConfig;
    vector<GLuint> _uniformLocations;
    bytearray _uniformData;

    GLShaderState(Shader* shader) {
        string vertexSource = shader->getVertexSource();
        string fragmentSource = shader->getFragmentSource();
        
        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource.data());
        GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource.data());

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
        
        _uniformLocations.resize(shader->_uniforms.size());
        int i=0;
        int32_t cb = 0;
        for (auto& uniform: shader->_uniforms) {
            uniform.offset = cb;
            cb += uniform.length();
            _uniformLocations[i] = check_gl(glGetUniformLocation, _program, uniform.name);
            i++;
        }
        _uniformData.resize(cb);
    }
    
    ~GLShaderState() {
         check_gl(glDeleteShader, _program);
    }
    
};


class GLSurface : public Surface {
public:
    GLuint _fb;
    
    GLSurface(Renderer* renderer, bool isPrivate) : Surface(renderer, isPrivate) {
        if (isPrivate) {
            check_gl(glGenFramebuffers, 1, &_fb);
            assert(_fb > 0);
        }
    }
    ~GLSurface() {
        if (_isPrivate && _fb) {
            check_gl(glDeleteFramebuffers, 1, &_fb);
            _fb = 0;
        }
    }

    void setSize(const SIZE& size) override {
        Surface::setSize(size);
        if (!_isPrivate) {
            return;
        }
        _texture->resize(size.width, size.height);

        check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ((GLTexture*)_texture._obj)->_textureId, 0);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        //GLenum x = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        //assert(x==GL_FRAMEBUFFER_COMPLETE);

        GLSurface* currentGlSurface = (GLSurface*)_renderer->_currentSurface;
        if (currentGlSurface) {
            check_gl(glBindFramebuffer, GL_FRAMEBUFFER, currentGlSurface->_fb);
        }
    }
    

    
};


Surface* GLRenderer::getPrimarySurface() {
    return _primarySurface; 
}

Surface* GLRenderer::createPrivateSurface() {
    GLSurface* surface = new GLSurface(this, true);
    surface->_texture = new GLTexture(this);
    return surface;

}



GLTexture::GLTexture(Renderer* renderer) : Texture(renderer) {
    _texTarget = GL_TEXTURE_2D;
    check_gl(glGenTextures, 1, &_textureId);
    _format = BITMAPFORMAT_RGBA32;
}

void GLTexture::resize(int width, int height) {
    realloc(width, height, NULL, true);
}

void GLTexture::realloc(int width, int height, void* pixelData, bool sizeChanged) {
    int pixelType;
    GLenum format, internalFormat;
    
    switch (_format) {
        case BITMAPFORMAT_RGBA32: {
            pixelType = GL_UNSIGNED_BYTE;
            format = GL_RGBA;
            internalFormat = GL_RGBA;
            break;
        }
        case BITMAPFORMAT_BGRA32: {
            pixelType = GL_UNSIGNED_BYTE;
            format = GL_BGRA;
            internalFormat = GL_RGBA;
            break;
        }
        case BITMAPFORMAT_RGB24: {
            pixelType = GL_UNSIGNED_BYTE;
            format = GL_RGB;
            internalFormat = GL_RGB;
            break;
        }
        case BITMAPFORMAT_RGB565: {
            pixelType = GL_UNSIGNED_SHORT_5_6_5;
            format = GL_RGB;
            internalFormat = GL_RGB;
            break;
        }
        case BITMAPFORMAT_A8: {
            pixelType = GL_UNSIGNED_BYTE;
#ifdef PLATFORM_LINUX   // TODO: 'red' vs 'alpha' is probably not a Linux thing, it's more likely to be GL vs GL ES
            format = GL_RED;
            internalFormat = GL_RED;
#else
            format = GL_ALPHA;
            internalFormat = GL_ALPHA;
#endif
            break;
        }
        default: assert(0);
    }
#if PLATFORM_WEB
    if (pixelFormat == GL_BGRA) pixelFormat = GL_RGBA; // WebGL no like iOS format
#endif

    
    check_gl(glBindTexture, _texTarget, _textureId);
    check_gl(glTexParameteri, _texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    check_gl(glTexParameteri, _texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    check_gl(glTexParameteri, _texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check_gl(glTexParameteri, _texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (sizeChanged || !pixelData) {
        check_gl(glTexImage2D, _texTarget, 0, internalFormat, width, height, 0, format, pixelType, pixelData);
    } else {
        check_gl(glTexSubImage2D, _texTarget, 0, 0, 0, width, height, format, pixelType, pixelData);
    }
    if (_renderer->_currentTexture && _renderer->_currentTexture != this) {
        check_gl(glBindTexture, GL_TEXTURE_2D, ((GLTexture*)_renderer->_currentTexture)->_textureId);
    }
}


void GLTexture::upload() {
    PIXELDATA pixeldata;
    _bitmap->lock(&pixeldata, false);
    realloc(_bitmap->_width, _bitmap->_height, pixeldata.data, false);
    _bitmap->unlock(&pixeldata, false);
}


int GLTexture::getSampler() {
    if (_texTarget == GL_TEXTURE_EXTERNAL_OES) {
        return GLSAMPLER_TEXTURE_EXT_OES;
    }
    return GLSAMPLER_TEXTURE_2D;
}


    

string StandardShader::getVertexSource() {
    
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
        //_sampler.set(0);
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
    return vs;
}


string StandardShader::getFragmentSource() {
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
        //_sampler.set(0);
    }
    
    string fs = "varying lowp vec4 v_color;\n";
    if (_features.alpha) {
        fs += "uniform mediump float alpha;\n";
    }
    if (_features.roundRect) {
        fs += "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
        "uniform lowp vec4 strokeColor;\n";
        if (_features.roundRect == SHADER_ROUNDRECT_1) {
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
    
    return fs;
}
    

    /*void unload() override {
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
    }*/

    

    
    

void GLRenderer::setCurrentSurface(Surface* surface) {
    GLSurface* glsurface = (GLSurface*)surface;
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, glsurface->_fb);
    check_gl(glViewport, 0, 0, surface->_size.width, surface->_size.height);
}


void GLRenderer::setCurrentTexture(Texture* texture) {
    if (_currentTexture == texture) {
        return;
    }
    _currentTexture = texture;
    GLTexture* gltex = (GLTexture*)texture;
    check_gl(glBindTexture, gltex->_texTarget, gltex->_textureId);
    if (!gltex->_paramsValid) {
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
            check_gl(glTexParameteri, gltex->_texTarget, GL_TEXTURE_MAX_LEVEL, gltex->_maxMipMapLevel);
        }
        gltex->_paramsValid = true;
    }
    if (gltex->_needsUpload) {
        gltex->_needsUpload = false;
        gltex->upload();
    }
}

void* GLRenderer::createShaderState(Shader* shader) {
    return new GLShaderState(shader);
}
void GLRenderer::deleteShaderState(void* state) {
    delete (GLShaderState*)state;
}

void GLRenderer::bindCurrentShader() {
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

GLRenderer::GLRenderer(Window* window) : Renderer(window) {
    _primarySurface = new GLSurface(this, false);
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

Texture* GLRenderer::createTexture() {
    return new GLTexture(this);
}
void GLRenderer::releaseTexture(Texture* texture) {
    GLTexture* gltex = (GLTexture*)texture;
     if (gltex->_textureId) {
         check_gl(glDeleteTextures, 1, &gltex->_textureId);
         gltex->_textureId = 0;
     }
    Renderer::releaseTexture(texture);
 }


void GLRenderer::setCurrentBlendMode(int blendMode) {
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

/*void GLRenderer::renderPrivateSurface(Surface* privateSurface, ItemPool::Alloc* alloc) {
    GLSurface* surface = (GLSurface*)privateSurface;
    check_gl(glBindTexture, GL_TEXTURE_2D, surface->_tex);
    check_gl(glDrawElements, GL_TRIANGLES, 6 * 1, GL_UNSIGNED_SHORT, (void*)((alloc->offset)*6*sizeof(GLshort)));
}*/

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

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&((GLSurface*)_primarySurface)->_fb);

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



void GLRenderer::copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin) {
    RECT wrect = rect;
    wrect.origin.y += _currentSurface->_size.height - rect.size.height;
    Texture* prevTex = _currentTexture;
    if (_currentTexture != destTex) {
        setCurrentTexture(destTex);
    }
    check_gl(glCopyTexSubImage2D, GL_TEXTURE_2D, 0, destOrigin.x, destOrigin.y, wrect.origin.x, wrect.origin.y, wrect.size.width, wrect.size.height);
    if (prevTex && _currentTexture != prevTex) {
        setCurrentTexture(prevTex);
    }
}

void GLRenderer::generateMipmaps(Texture* tex) {
    GLTexture* gltex = (GLTexture*)tex;
    check_gl(glGenerateMipmap, gltex->_texTarget);
}


void GLRenderer::setUniformData(int16_t uniformIndex, const void* data, int32_t cb) {
    GLShaderState* state = (GLShaderState*)_currentShader->_shaderState;
    auto& uniform = _currentShader->_uniforms[uniformIndex];
    auto currentData = (state->_uniformData.data() + uniform.offset);
    if (0 == memcmp(currentData, data, cb)) {
        return;
    }
    memcpy(currentData, data, cb);

    GLuint loc = state->_uniformLocations[uniformIndex];
    assert(loc >= 0);
    switch (uniform.type) {
        case Shader::Uniform::Int1: check_gl(glUniform1iv, loc, 1, (GLint*)data); break;
        case Shader::Uniform::Float1: check_gl(glUniform1fv, loc, 1, (GLfloat*)data); break;
        case Shader::Uniform::Float2: check_gl(glUniform2fv, loc, 1, (GLfloat*)data); break;
        case Shader::Uniform::Float4: check_gl(glUniform4fv, loc, 1, (GLfloat*)data); break;
        case Shader::Uniform::Matrix4: check_gl(glUniformMatrix4fv, loc, 1, false, (GLfloat*)data); break;
    }
}

int GLRenderer::getIntProperty(IntProperty property) {
    if (property == MaxVaryingFloats) {
        GLint val = 0;
        check_gl(glGetIntegerv, GL_MAX_VARYING_FLOATS, &val);
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






string BlurShader::getVertexSource() {
    int numOptimizedOffsets = (int)_op->_optimizedOffsets.size();
    
    char ach[128];
    
    string vs =
    "attribute highp vec2 vPosition;\n"
    "uniform highp mat4 mvp;\n"
    "attribute vec2 texcoord;\n"
    "uniform vec2 texOffset;\n";
    
    sprintf(ach, "varying vec2 blurCoordinates[%d];\n", 1 + numOptimizedOffsets * 2);
    vs.append(ach);
    
    vs.append("void main() {\n"
                        "   gl_Position = mvp * vec4(vPosition,0,1);\n");
    vs.append("   blurCoordinates[0] = texcoord.xy;\n");
    for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
        sprintf(ach,
                "   blurCoordinates[%lu] = texcoord.xy + texOffset * %f;\n"
                "   blurCoordinates[%lu] = texcoord.xy - texOffset * %f;\n",
                (unsigned long)((i * 2) + 1), _op->_optimizedOffsets[i],
                (unsigned long)((i * 2) + 2), _op->_optimizedOffsets[i]);
        vs.append(ach);
    }
    
    vs.append("}\n");
    return vs;
}
    
string BlurShader::getFragmentSource() {
    int numOptimizedOffsets = (int)_op->_optimizedOffsets.size();
    char ach[128];

    string fs =
    "uniform sampler2D texture;\n"
    "uniform vec2 texOffset;\n";
    sprintf(ach,
            "varying highp vec2 blurCoordinates[%d];\n", 1 + numOptimizedOffsets * 2);
    fs.append(ach);
    
    fs.append("void main() {\n");
    
    // Inner texture loop
    sprintf(ach, "lowp vec4 c = texture2D(texture, blurCoordinates[0]) * %f;\n", _op->_standardGaussianWeights[0]);
    fs.append(ach);
    for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
        GLfloat firstWeight = _op->_standardGaussianWeights[i * 2 + 1];
        GLfloat secondWeight = _op->_standardGaussianWeights[i * 2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
        sprintf(ach, "c += texture2D(texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((i * 2) + 1), optimizedWeight);
        fs.append(ach);
        sprintf(ach, "c += texture2D(texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((i * 2) + 2), optimizedWeight);
        fs.append(ach);
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
            fs.append(ach);
            sprintf(ach, "c += texture2D(texture, blurCoordinates[0] - texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
            fs.append(ach);
        }
    }
    
    fs.append("   gl_FragColor = c;\n"
                      "}\n");
    return fs;
    
}

string PostBlurShader::getVertexSource() {
    return
    "attribute highp vec2 vPosition;\n"
    "attribute lowp vec4 color;\n"
    "attribute vec2 texcoord;\n"
    "uniform highp mat4 mvp;\n"
    "varying lowp vec4 v_color;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "  gl_Position = mvp * vec4(vPosition,0,1);\n"
    "  v_texcoord = texcoord;\n"
    "  v_color=color;\n"
    "}\n";
;
}
string PostBlurShader::getFragmentSource() {
    return
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
    "}\n";
}





// Drop shadows? Refer to http://madebyevan.com/shaders/fast-rounded-rectangle-shadows/



#endif

