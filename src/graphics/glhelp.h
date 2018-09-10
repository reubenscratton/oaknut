//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define VERTEXATTRIB_POSITION 0
#define VERTEXATTRIB_TEXCOORD 1
#define VERTEXATTRIB_COLOR 2

#define VERTEXATTRIBS_CONFIG_NORMAL     1 // float x,y; uint16 s,t; uint32 color = 16 bytes
#define VERTEXATTRIBS_CONFIG_ROUNDRECT  2 // float x,y; uint16 dist_xy; uint32 fillColor; uint32 strokeColor; strokeWidth; radii

typedef uint32_t COLOR;


// Gravity
#define GRAVITY_LEFT 0
#define GRAVITY_RIGHT 1
#define GRAVITY_CENTER 2
#define GRAVITY_TOP 0
#define GRAVITY_BOTTOM 1
typedef struct {
    uint8_t horz:4;
    uint8_t vert:4;
} GRAVITY;


// NB: DONT leave error checking in release builds! It's weirdly slow in JS...
#if 1
#define check_gl(cmd, ...) cmd(__VA_ARGS__)
#else
void checkGlErr(const char* file, int line, const char* cmd);
#define check_gl(cmd, ...) cmd(__VA_ARGS__); checkGlErr(__FILE__, __LINE__, #cmd);
#endif



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


extern const char* STANDARD_VERTEX_SHADER;
extern const char* TEXTURE_VERTEX_SHADER;

class GLProgram : public Object {
public:

    bool _loaded;
	GLuint _program;
    GLuint _vertexConfig;

	// Standard uniforms
    GLint _posMvp;
    Uniform<float> _alpha;
    Uniform<int> _sampler;
    Matrix4 _mvp;
    
	virtual void load() = 0;
    virtual void unload();
	virtual void findVariables();
    virtual void use(class Window* window);
	virtual void setAlpha(float alpha);
    virtual void lazyLoadUniforms();
    virtual void setMvp(const Matrix4& mvp);
    
protected:
	virtual void loadShaders(const char* vertexShader, const char* fragShader);
};





