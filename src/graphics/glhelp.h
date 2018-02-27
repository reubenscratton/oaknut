//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define VERTEXATTRIB_POSITION 0
#define VERTEXATTRIB_TEXCOORD 1
#define VERTEXATTRIB_COLOUR 2

#define VERTEXATTRIBS_CONFIG_NORMAL     1 // float x,y; uint16 s,t; uint32 color = 16 bytes
#define VERTEXATTRIBS_CONFIG_ROUNDRECT  2 // float x,y; uint16 dist_xy; uint32 fillColor; uint32 strokeColor; strokeWidth; radii


// NB: DONT leave error checking in release builds! It's weirdly slow in JS...
#if 0
#define check_gl(cmd, ...) cmd(__VA_ARGS__)
#else
void checkGlErr(const char* file, int line, const char* cmd);
#define check_gl(cmd, ...) cmd(__VA_ARGS__); checkGlErr(__FILE__, __LINE__, #cmd);
#endif



class Uniform {
public:
    GLint position;  // as returned by glGetUniformLocation
    virtual void use()=0;
};

template <class T>
class CachedUniform : public Uniform {
public:
    T* src;
    T lastVal;
    virtual void use() {
        if (*src != lastVal) {
            lastVal = *src;
            load();
        }
    }
    virtual void load()=0;
};

template <class T>
class CachedUniform2 : public Uniform {
public:
    T val;
    bool dirty;
    CachedUniform2() {
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
    virtual void load()=0;
};

class UniformInt : public CachedUniform2<int> {
public:
    void load() {
        check_gl(glUniform1i, position, val);
    }
};

class UniformFloat : public CachedUniform2<float> {
public:
    void load() {
        check_gl(glUniform1f, position, val);
    }
};

class UniformVector2 : public CachedUniform2<Vector2> {
public:
    void load() {
        check_gl(glUniform2f, position, val.x, val.y);
    }
};

class UniformColour : public CachedUniform2<int> {
public:
    void load() {
        float c[4];
        c[3] = ((val&0xff000000)>>24)/255.0f;
        c[2] = (val&0xff)/255.0f;
        c[1] = ((val&0xff00)>>8)/255.0f;
        c[0] = ((val&0xff0000)>>16)/255.0f;
        check_gl(glUniform4f, position, c[0], c[1], c[2], c[3]);
    }
};

class UniformFloat4 : public CachedUniform2<Vector4> {
public:
    void load() {
        check_gl(glUniform4f, position, val.x, val.y, val.z, val.w);
    }
};



class UniformPOINT : public CachedUniform<POINT> {
public:
    void load() {
        check_gl(glUniform2f, position, lastVal.x, lastVal.y);
    }
};

template <class T>
class UniformCounted : public Uniform {
public:
    
    class Value {
    public:
        T val;
        int counter;
    };

    Value* src;
    int counter;
    
    void setSource(Value* src) {
        if (src != this->src) {
            this->src = src;
            counter = src->counter-1;
        }
    }
    
    virtual void use()=0;
    
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
	UniformFloat _alpha;
    UniformInt _sampler;
    Matrix4 _mvp;
    
	virtual void load() = 0;
	virtual void findVariables();
    virtual void use(class Canvas* canvas);
	virtual void setAlpha(float alpha);
    virtual void lazyLoadUniforms();
    virtual void setMvp(const Matrix4& mvp);
    
protected:
	virtual void loadShaders(const char* vertexShader, const char* fragShader);
};

typedef struct {
    GLint pos;
    enum {
        Int1,
        Float1,
        Float4
    } type;
    union {
        int int1;
        float float1;
        float float4[4];
    } value;
} UNIFORM;

class ProgramUsage : public Object {
public:
    GLProgram* _program;
    vector<UNIFORM> _uniforms;
    
};






