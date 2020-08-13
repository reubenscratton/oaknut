//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// TODO: Move these elsewhere
#if RENDERER_METAL
#define SL_SIZEOF_COLOR 8
#define SL_CONST "constant"
#define SL_HALF1 "half"
#define SL_HALF1_DECL "half"
#define SL_HALF2 "half2"
#define SL_HALF2_DECL "half2"
#define SL_HALF3 "half3"
#define SL_HALF3_DECL "half3"
#define SL_HALF4 "half4"
#define SL_HALF4_DECL "half4"
#define SL_FLOAT1 "float"
#define SL_FLOAT2 "float2"
#define SL_FLOAT3 "float3"
#define SL_FLOAT4 "float4"
#define SL_MATRIX4 "float4x4"
#define SL_OUTPIXVAL "c"
#define SL_UNIFORM(x) "uniforms->"#x
#define SL_VERTEX_INPUT(x) "v_in[vid]."#x
#define SL_VERTEX_OUTPUT(x) "in."#x
#define SL_VERTEX_OUTPUT_STR(x) "in."+x
#define SL_TEXSAMPLE_2D(x, coord) "colorTexture.sample(textureSampler," coord ")"
#define SL_TEXSAMPLE_2D_RECT(x, coord) "colorTexture.sample(textureSampler," coord ")"
#define SL_TEXSAMPLE_2D_OES(x, coord) "colorTexture.sample(textureSampler," coord ")"

#elif RENDERER_GL
#define SL_SIZEOF_COLOR 16
#define SL_CONST "const"
#define SL_HALF1 "float"
#define SL_HALF1_DECL "lowp float"
#define SL_HALF2 "vec2"
#define SL_HALF2_DECL "lowp vec2"
#define SL_HALF3 "vec3"
#define SL_HALF3_DECL "lowp vec3"
#define SL_HALF4 "vec4"
#define SL_HALF4_DECL "lowp vec4"
#define SL_FLOAT1 "float"
#define SL_FLOAT2 "vec2"
#define SL_FLOAT3 "vec3"
#define SL_FLOAT4 "vec4"
#define SL_MATRIX4 "highp mat4"
#define SL_OUTPIXVAL "gl_FragColor"
#define SL_UNIFORM(x) #x
#define SL_VERTEX_INPUT(x) #x
#define SL_VERTEX_OUTPUT(x) "v_"#x
#define SL_VERTEX_OUTPUT_STR(x) "v_"+x
#define SL_TEXSAMPLE_2D(x, coord) "texture2D(texture," coord ")"
#define SL_TEXSAMPLE_2D_RECT(x, coord) "texture2DRect(texture," coord ")"
#define SL_TEXSAMPLE_2D_OES(x, coord) "texture2D(texture," coord ")"

#else
#error todo
#endif

// TODO: This is GL-specific, should be moved
#define VERTEXATTRIB_POSITION 0
#define VERTEXATTRIB_TEXCOORD 1
#define VERTEXATTRIB_COLOR 2

#define VERTEXATTRIBS_CONFIG_NORMAL     1 // float x,y; uint16 s,t; uint32 color = 16 bytes

enum BlendOp {
    None,
    Add,
    Subtract
};
enum BlendFactor {
    Zero,
    SourceColor,
    SourceAlpha,
    DestinationColor,
    DestinationAlpha,
    One,
    OneMinusSourceColor,
    OneMinusSourceAlpha,
    OneMinusDestinationColor,
    OneMinusDestinationAlpha
};
struct BlendParams {
    BlendOp op;
    BlendFactor srcRGB;
    BlendFactor srcA;
    BlendFactor dstRGB;
    BlendFactor dstA;
    
    bool operator==(const BlendParams& other) const {
        return op==other.op &&
            srcRGB==other.srcRGB &&
            srcA==other.srcA &&
            dstRGB==other.dstRGB &&
            dstA==other.dstA;
    }
    inline size_t hash() const {
        return (size_t)op
            || (((size_t)srcRGB)<<2)
            || (((size_t)srcA)<<6)
            || (((size_t)dstRGB)<<10)
            || (((size_t)dstA)<<14);
    }
};

/**
 Base class for Renderer-allocated objects such as Textures and Shaders. It manages
 a connection to the Renderer that created it, ensuring correct cleanup is performed
 during destruction, thereby freeing rendering code from needing to do explicit cleanup.
 */
class RenderResource : public Object {
public:
    RenderResource(class Renderer* renderer);

    Renderer* _renderer; // should ideally be protected, but derived app-level classes need fast access so meh
protected:
    list<RenderResource*>::iterator _it;
    friend class Renderer;
    friend class RenderBatch;
};

class Texture : public RenderResource {
public:
    Texture(Renderer* renderer, int format);
    ~Texture();
    
    void setSize(SIZEI size) {
        if (size != _size) {
            _size = size;
            _isNativeTextureValid = false;
        }
    }
    virtual bool readPixels(RECT rect, bytearray& target) const =0;

    
    class Bitmap* _bitmap; // NB: can be null
    enum Type {
        None,
        Normal,
        Rect,
        OES
    } _type;
    int _format; // see BitmapFormat
    int _maxMipMapLevel; // defaults to 0, i.e. no mipmaps
    SIZEI _size;

    // Flags
    bool _isNativeTextureValid; // set false when size or format changes
    bool _isRenderTarget;
    bool _needsUpload;
    bool _denormalizedCoords; // usually false, only true for OpenGL's rectangular textures
    bool _minFilterLinear;
    bool _magFilterLinear;
    bool _mipFilterLinear;
};


/**
 StandardShader does the vast majority of rendering. They are constructed on-demand
 for a unique feature set
 */
#define SDF_NONE         0
#define SDF_ROUNDRECT_1  1
#define SDF_ROUNDRECT_4  2



class Shader : public RenderResource  {
public:

    Shader(Renderer* renderer);
    ~Shader();
    
    enum VariableType {
        Color, // a lowp vec4 or half4 in shader code
        Int1,
        Float1,
        Float2,
        Float4,
        Matrix4,
    };
    struct Uniform {
        VariableType type;
        enum Usage {
            Vertex,
            Fragment
        } usage;
        string name;
        int16_t offset;
        int16_t length();
        COLOR cachedColorVal; // exists to avoid a lot of unneccessary COLOR->float[] conversions
    };
    struct VertexShaderOutput {
        bool isVertexAttribute;
        VariableType type;
        string name;
        string outValue;
    };



    vector<VertexShaderOutput> _vertexShaderOutputs;
    vector<Uniform> _uniforms;
    void* _shaderState; // opaque data managed by renderer. Non-null while shader is loaded.

    // Generic shader language APIs. The outer program structure is provided by the renderer.
    virtual string getSupportingSource();
    virtual string getFragmentSource();
    virtual int getTextureCount() { return 0;}
    virtual Texture::Type getTextureType(int index) { return Texture::Type::None; };

    // Uniforms
    int16_t _u_mvp;
    int16_t _u_alpha;
    
protected:
    int16_t declareAttribute(const string& name, VariableType type, string value="");
    int16_t declareUniform(const string& name, VariableType type, typename Uniform::Usage usage=Uniform::Usage::Fragment);
    
};

template <class T>
class ShaderFactory {
public:
    using TFeatures = typename T::Features;
    
    map<TFeatures, T*> _shaders;

    T* get(Renderer* renderer, TFeatures features) {
        auto it = _shaders.find(features);
        if (it == _shaders.end()) {
            it = _shaders.insert(make_pair(features, new T(renderer, features))).first;
        }
        return it->second;
    }
};



string sl_getTypeString(Shader::VariableType type);
   
enum PrimitiveType {
    Point,
    Line,
    Quad
};

/**
 A batch of GPU instructions to be executed asynchronously to the CPU.
 Metal: MTLCommandBuffer
 OpenGL: emulated
 */
class RenderTask : public RenderResource {
public:

    RenderTask(Renderer* renderer);
    void bindBitmap(Bitmap* bitmap);
    virtual bool bindToNativeSurface(class Surface* surface)=0; // TODO: rename
    void setCurrentSurface(Surface* surface);
    void setCurrentTexture(Texture* texture);
    inline void setBlendNone() {
        setBlendParams({
            .op     = None,
            .srcRGB = SourceColor,
            .srcA   = SourceAlpha,
            .dstRGB = Zero,
            .dstA   = Zero
       });
    }
    inline void setBlendNormal() {
         setBlendParams({
             .op     = Add,
             .srcRGB = SourceAlpha,
             .srcA   = SourceAlpha,
             .dstRGB = OneMinusSourceAlpha,
             .dstA   = OneMinusSourceAlpha
        });
    }
    inline void setBlendPremultiplied() {
         setBlendParams({
             .op     = Add,
             .srcRGB = One,
             .srcA   = One,
             .dstRGB = OneMinusSourceAlpha,
             .dstA   = OneMinusSourceAlpha
        });
    }
    inline void setBlendAccumulate() {
         setBlendParams({
             .op     = Add,
             .srcRGB = One,
             .srcA   = One,
             .dstRGB = One,
             .dstA   = One
        });
    }
    void setBlendParams(const BlendParams& blendParams) {
        if (_blendParams.op==blendParams.op &&
            _blendParams.srcRGB==blendParams.srcRGB &&
            _blendParams.srcA==blendParams.srcA &&
            _blendParams.dstRGB==blendParams.dstRGB &&
            _blendParams.dstA==blendParams.dstA) {
            return;
        }
        _blendParams = blendParams;
        _blendParamsValid = false;
    }
    virtual void setUniformData(int16_t uniformIndex, const void* data, int32_t cb) =0;
    void setCurrentShader(Shader* shader);
    template<class T>
    void setUniform(int16_t uniformIndex, const T& val);
    void pushClip(const RECT& clip);
    void popClip();
    virtual void draw(PrimitiveType type, int num, int index) =0;
    virtual void copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin)=0;
    virtual void generateMipmaps(Texture* tex) =0;
    virtual void commit(std::function<void()> onComplete)=0;

protected:

    
    Shader* _currentShader;
    bool _currentShaderValid;
    int _currentVertexConfig;
    BlendParams _blendParams;
    bool _blendParamsValid;
    Texture* _currentTexture;
    bool _currentTextureValid;
    Surface* _currentSurface;
    bool _currentSurfaceValid;
    stack<RECT> _clips;
    RECT _currentClip;
    bool _currentClipValid;
    
    friend class RenderBatch;
};

/**
 Abstract base class for hardware-accelerated rendering.
 */
class Renderer : public Object {
public:
    list<RenderResource*> _textures;
    list<RenderResource*> _shaders;
    int _primarySurfaceFormat;

    void reset();
    ItemPool::Alloc* allocQuads(int num, ItemPool::Alloc* existingAlloc);
    virtual void createTextureForBitmap(Bitmap* bitmap);
    virtual void releaseTexture(Texture* tex);
    virtual void releaseShader(Shader* shader);

    void invalidateQuads(ItemPool::Alloc* alloc);
    

    // Non-drawing interface, ie use these outside the render loop
    virtual Surface* createSurface(bool isPrivate) =0;
    virtual Texture* createTexture(int format) =0;
    enum IntProperty {
        MaxVaryingFloats,
    };
    virtual int getIntProperty(IntProperty property) =0;

    // Drawing interface, i.e. for use of RenderOps
    virtual RenderTask* createRenderTask() =0;
    virtual void flushQuadBuffer() =0;
    virtual void uploadQuad(ItemPool::Alloc* alloc) =0;
    
    

    static Renderer* create();
    
protected:
    Renderer();

    virtual void* createShaderState(Shader* shader) =0;
    virtual void deleteShaderState(void* state) =0;


    // QuadBuffer data
    ItemPool _quadBuffer;
    bool _fullBufferUploadNeeded;
    uint8_t* _quadBufferDirtyLo;
    uint8_t* _quadBufferDirtyHi;

    friend class RenderTask;
};
