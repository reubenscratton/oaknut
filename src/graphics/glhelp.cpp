//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


COLOR COLOR::interpolate(COLOR start, COLOR end, float val) {
    uint8_t* c1 = (uint8_t*)&start;
    uint8_t* c2 = (uint8_t*)&end;
    uint8_t c3[4];
    c3[0] = c1[0] + (uint8_t)((c2[0]-c1[0]) * val);
    c3[1] = c1[1] + (uint8_t)((c2[1]-c1[1]) * val);
    c3[2] = c1[2] + (uint8_t)((c2[2]-c1[2]) * val);
    c3[3] = c1[3] + (uint8_t)((c2[3]-c1[3]) * val);
    return *(COLOR*)c3;
}

template<>
void Uniform<int>::load() {
    check_gl(glUniform1i, position, val);
}

template<>
void Uniform<float>::load() {
    check_gl(glUniform1f, position, val);
};

template<>
void Uniform<Vector2>::load() {
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
void Uniform<Vector4>::load() {
    check_gl(glUniform4f, position, val.x, val.y, val.z, val.w);
}


template<>
void Uniform<POINT>::load() {
    check_gl(glUniform2f, position, val.x, val.y);
}




static GLuint loadShader(GLenum shaderType, const char* pSource, const char* preprocs=NULL) {
    GLuint shader = check_gl(glCreateShader, shaderType);
    if (shader) {
		string source;
		if (preprocs) {
		    source = preprocs;
		}
		source +=
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

/**
 A note re. highp usage: Although vertex shader types are supposedly highp by default,
 on the iPhone 6S it turns out that unless you specify it you get wonky vertex arithmetic
 and horrible off-by-half-a-pixel rendering errors everywhere. Vertices must always be highp.
 */
const char* STANDARD_VERTEX_SHADER =
    "attribute highp vec2 vPosition;\n"     // the 'highp' qualifier is VERY IMPORTANT! See above
    "uniform highp mat4 mvp;\n"
    "attribute lowp vec4 color;\n"
    "varying lowp vec4 v_color;\n"
		"void main() {\n"
		"  gl_Position = mvp * vec4(vPosition,0,1);\n"
        "  v_color=color;\n"
		"}\n";

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

void GLProgram::loadShaders(const char *szVertexShader, const char *szFragShader, const char* szPreprocs/*=NULL*/) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, szVertexShader, NULL);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, szFragShader, szPreprocs);
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
    glGetError();
}


void GLProgram::findVariables() {
	_posMvp = check_gl(glGetUniformLocation, _program, "mvp");
	_sampler.position = check_gl(glGetUniformLocation, _program, "texture");
	_alpha.position = check_gl(glGetUniformLocation, _program, "alpha");
}

void GLProgram::unload() {
    check_gl(glDeleteShader, _program);
    _loaded = false;
    _sampler.dirty = true;
    _alpha.dirty = true;
    memset(&_mvp, 0, sizeof(_mvp));
}
void GLProgram::use(Window* window) {
    if (!_loaded) {
        _loaded = true;
        load();
        window->_loadedProgs.push_back(this);
    }
    if (window->_currentProg != _program) {
        window->_currentProg = _program;
        check_gl(glUseProgram,_program);
    }
    
    // These are program-specific...
    if (window->_currentVertexConfig != _vertexConfig) {
        window->setVertexConfig(_vertexConfig);
    }
}

void GLProgram::setMvp(const Matrix4& mvp) {
    if (0!=memcmp(mvp.get(), _mvp.get(), 16*sizeof(float))) {
        _mvp = mvp;
        check_gl(glUniformMatrix4fv, _posMvp, 1, 0, mvp.get());
    }
}

void GLProgram::lazyLoadUniforms() {
    _sampler.use();
    if (_alpha.position>=0) {
        _alpha.use();
    }
}

void GLProgram::setAlpha(float alpha) {
	if (_alpha.position>=0) {
        _alpha.set(alpha);
	}
}

void checkGlErr(const char* file, int line, const char* cmd) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        app.log("GL ERROR: %s(%d) %s() err=0x%x", file, line, cmd, error);
    }
}


// Drop shadows? Refer to http://madebyevan.com/shaders/fast-rounded-rectangle-shadows/




