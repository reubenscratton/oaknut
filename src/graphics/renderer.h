//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// TODO: Move these elsewhere
#if RENDERER_METAL
#define SL_SIZEOF_COLOR 8
#define SL_HALF1 "half"
#define SL_HALF4 "half4"
#define SL_FLOAT1 "float"
#define SL_FLOAT2 "float2"
#define SL_FLOAT4 "float4"
#define SL_MATRIX4 "float4x4"
#define SL_OUTPIXVAL "c"
#define SL_UNIFORM(x) "uniforms->"#x
#define SL_ATTRIB(x) "in."#x
#define SL_FLOAT4_TO_OUTPIX(x) "half4("#x ")"
#define SL_TEXSAMPLE_2D(x, coord) "colorTexture.sample(textureSampler," coord ")"
#define SL_TEXSAMPLE_2D_RECT(x, coord) "colorTexture.sample(textureSampler," coord ")"
#define SL_TEXSAMPLE_2D_OES(x, coord) "colorTexture.sample(textureSampler," coord ")"

#elif RENDERER_GL
#define SL_SIZEOF_COLOR 16
#define SL_HALF1 "lowp float"
#define SL_HALF4 "lowp vec4"
#define SL_FLOAT1 "float"
#define SL_FLOAT2 "vec2"
#define SL_FLOAT4 "vec4"
#define SL_MATRIX4 "highp mat4"
#define SL_OUTPIXVAL "gl_FragColor"
#define SL_UNIFORM(x) #x
#define SL_ATTRIB(x) "v_"#x
#define SL_FLOAT4_TO_OUTPIX(x) #x
#define SL_TEXSAMPLE_2D(x, coord) "texture2D(texture," coord ")"
#define SL_TEXSAMPLE_2D_RECT(x, coord) "texture2DRect(texture," coord ")"
#define SL_TEXSAMPLE_2D_OES(x, coord) "texture2D(texture," coord ")"

#else
#error todo
#endif


#define VERTEXATTRIB_POSITION 0
#define VERTEXATTRIB_TEXCOORD 1
#define VERTEXATTRIB_COLOR 2

#define VERTEXATTRIBS_CONFIG_NORMAL     1 // float x,y; uint16 s,t; uint32 color = 16 bytes
#define VERTEXATTRIBS_CONFIG_ROUNDRECT  2 // float x,y; uint16 dist_xy; uint32 fillColor; uint32 strokeColor; strokeWidth; radii

/**
 Base class for Renderer-allocated objects such as Textures and Shaders. It manages
 a connection to the Renderer that created it, ensuring correct cleanup is performed
 during destruction, thereby freeing rendering code from needing to do explicit cleanup.
 */
class RenderResource : public Object {
public:
    RenderResource(class Renderer* renderer);

protected:
    Renderer* _renderer;
    list<RenderResource*>::iterator _it;
    friend class Renderer;
};

class Texture : public RenderResource {
public:
    Texture(Renderer* renderer);
    ~Texture();
    
    virtual void resize(int width, int height)=0;
    
    class Bitmap* _bitmap; // NB: can be null
    enum Type {
        None,
        Normal,
        Rect,
        OES
    } _type;
    int _format; // see BitmapFormat
    bool _needsUpload;
    bool _denormalizedCoords; // usually false, only true for OpenGL's rectangular textures
    int _maxMipMapLevel; // defaults to 0, i.e. no mipmaps
    bool _minFilterLinear;
    bool _magFilterLinear;
    bool _mipFilterLinear;
};


/**
 StandardShader does the vast majority of rendering. They are constructed on-demand
 for a unique feature set
 */
#define SHADER_ROUNDRECT_0  0
#define SHADER_ROUNDRECT_1  1
#define SHADER_ROUNDRECT_2H 2
#define SHADER_ROUNDRECT_2V 3
#define SHADER_ROUNDRECT_4  4

class Shader : public RenderResource  {
public:
    struct Features {
        uint32_t alpha:1;
        uint32_t roundRect:3;
        uint32_t tint:1;
        Texture::Type textures[2];
        
        Features() {
            alpha = 0;
            roundRect = 0;
            tint = 0;
            textures[0] = Texture::Type::None;
            textures[1] = Texture::Type::None;
        }
        
        bool operator==(const Features& other) const {
            if (alpha!=other.alpha) return false;
            if (roundRect!=other.roundRect) return false;
            if (tint!=other.tint) return false;
            if (textures[0]!=other.textures[0]) return false;
            if (textures[1]!=other.textures[1]) return false;
            return true;
        }
        bool operator!=(const Features& other) const {
            if (alpha!=other.alpha) return true;
            if (roundRect!=other.roundRect) return true;
            if (tint!=other.tint) return true;
            if (textures[0]!=other.textures[0]) return true;
            if (textures[1]!=other.textures[1]) return true;
            return false;
        }
        bool operator<(const Features& other) const {
            if (alpha<other.alpha) return true;
            if (roundRect<other.roundRect) return true;
            if (tint<other.tint) return true;
            if (textures[0]<other.textures[0]) return true;
            if (textures[1]<other.textures[1]) return true;
            return false;
        }
    } _features;

    Shader(Renderer* renderer);
    Shader(Renderer* renderer, Features features);

    ~Shader();



    enum VariableType {
        Color, // a lowp vec4 or half4 in shader code
        Int1,
        Float1,
        Float2,
        Float4,
        Matrix4,
    };

    struct Attribute {
        VariableType type;
        const char* name;
        int16_t inOffset;
    };
    vector<Attribute> _attributes;
    
    


    
    struct Uniform {
        VariableType type;
        enum Usage {
            Vertex,
            Fragment
        } usage;
        const char* name;
        int16_t offset;
        int16_t length();
        COLOR cachedColorVal; // exists to avoid a lot of unneccessary COLOR->float[] conversions
    };
    vector<Uniform> _uniforms;

    void* _shaderState; // opaque data managed by renderer. Non-null while shader is loaded.

    // Generic shader language APIs. The outer program structure is provided by the renderer.
    virtual string getVertexSource();
    virtual string getFragmentSource();

    // Uniforms
    int16_t _u_mvp;
    int16_t _u_alpha;
    int16_t _u_sampler;
    int16_t _u_strokeColor;
    int16_t _u_u;
    int16_t _u_radius;
    int16_t _u_radii;

    
protected:
    int16_t declareAttribute(const char* name, VariableType type);
    int16_t declareUniform(const char* name, VariableType type, Uniform::Usage usage=Uniform::Usage::Fragment);
};


string sl_getTypeString(Shader::VariableType type);
    



/**
 Abstract base class for hardware-accelerated rendering.
 */
class Renderer : public Object {
public:
    class Window* _window;
    list<RenderResource*> _textures;
    list<RenderResource*> _shaders;
    map<Shader::Features, sp<Shader>> _standardShaders; // standard shaders, not custom ones
    

    // Render state (used during render loop)
    bool _doneInit;
    Shader* _currentShader;
    int _currentVertexConfig;
    int _blendMode;
    stack<RECT> _clips;
    Texture* _currentTexture;
    int _renderCounter;
    class Surface* _currentSurface;

    void reset();
    ItemPool::Alloc* allocQuads(int num, ItemPool::Alloc* existingAlloc);
    virtual void createTextureForBitmap(Bitmap* bitmap);
    Shader* getStandardShader(Shader::Features features);
    void bindBitmap(Bitmap* bitmap);
    virtual void releaseTexture(Texture* tex);
    virtual void releaseShader(Shader* shader);

    void setCurrentShader(Shader* shader);
    void invalidateQuads(ItemPool::Alloc* alloc);

    template<class T>
    void setUniform(int16_t uniformIndex, const T& val);
    

    // Non-drawing interface, ie use these outside the render loop
    virtual void bindToNativeWindow(long nativeWindowHandle) =0;
    virtual Surface* getPrimarySurface() =0;
    virtual Surface* createPrivateSurface() =0;
    virtual Texture* createTexture() =0;
    enum IntProperty {
        MaxVaryingFloats,
    };
    virtual int getIntProperty(IntProperty property) =0;

    // Drawing interface, i.e. for use of RenderOps
    virtual void prepareToDraw() =0;
    virtual void setCurrentSurface(Surface* surface)=0;
    virtual void setCurrentTexture(Texture* texture)=0;
    virtual void setCurrentBlendMode(int blendMode)=0;
    virtual void setUniformData(int16_t uniformIndex, const void* data, int32_t cb) =0;
    virtual void setColorUniform(int16_t uniformIndex, const float* rgba) =0;
    virtual void copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin)=0;
    virtual void generateMipmaps(Texture* tex) =0;
    virtual void pushClip(RECT clip) =0;
    virtual void popClip() =0;
    virtual void flushQuadBuffer() =0;
    virtual void uploadQuad(ItemPool::Alloc* alloc) =0;
    virtual void drawQuads(int numQuads, int index) =0;
    virtual void commit() =0;
    
    

    static Renderer* create(Window* window);
    
protected:
    Renderer(Window* window);

    virtual void* createShaderState(Shader* shader) =0;
    virtual void deleteShaderState(void* state) =0;
    virtual void bindCurrentShader() =0;


    // QuadBuffer data
    ItemPool _quadBuffer;
    bool _fullBufferUploadNeeded;
    uint8_t* _quadBufferDirtyLo;
    uint8_t* _quadBufferDirtyHi;

};
