//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define SHADER_ROUNDRECT_0  0
#define SHADER_ROUNDRECT_1  1
#define SHADER_ROUNDRECT_2H 2
#define SHADER_ROUNDRECT_2V 3
#define SHADER_ROUNDRECT_4  4


union ShaderFeatures {
    uint32_t all;
    struct {
        uint32_t alpha:1;
        uint32_t roundRect:3;
        uint32_t sampler0:4;
        uint32_t sampler1:4;
        uint32_t sampler2:4;
        uint32_t sampler3:4;
        uint32_t tint:1;
    };
    ShaderFeatures() : all(0) {
    }
};


#define VERTEXATTRIB_POSITION 0
#define VERTEXATTRIB_TEXCOORD 1
#define VERTEXATTRIB_COLOR 2

#define VERTEXATTRIBS_CONFIG_NORMAL     1 // float x,y; uint16 s,t; uint32 color = 16 bytes
#define VERTEXATTRIBS_CONFIG_ROUNDRECT  2 // float x,y; uint16 dist_xy; uint32 fillColor; uint32 strokeColor; strokeWidth; radii



class Texture {
public:
    Texture(Bitmap* bitmap);
    virtual ~Texture();
    virtual void bind()=0;
    virtual void upload()=0;
    virtual void unload()=0;
    virtual int getSampler()=0;
    
    Bitmap* _bitmap;
    bool _allocdTexData;
    bool _needsUpload;
    list<Texture*>::iterator _it;
};

class Shader {
public:
    ShaderFeatures _features;

    Shader(ShaderFeatures features);

    virtual void unload() =0;
    virtual void findVariables() =0;
    virtual void lazyLoadUniforms() =0;
    virtual void setMvp(const MATRIX4& mvp) =0;
    virtual void setAlpha(float alpha) =0;
    virtual void configureForRenderOp(class RectRenderOp* op) =0;
    virtual void use(class Renderer* renderer) =0;
};

/**
 Abstract base class for hardware-accelerated rendering.
 */
class Renderer : public Object {
public:
    map<uint32_t, Shader*> _shaders;
    list<Texture*> _textures;
    

    // Render state (used during render loop)
    bool _doneInit;
    GLuint _currentProg;
    int _currentVertexConfig;
    int _blendMode;
    stack<RECT> _clips;
    Texture* _currentTexture;
    int _renderCounter;
    class Surface* _currentSurface;

    
    void reset();
    void setCurrentSurface(Surface* surface);
    ItemPool::Alloc* allocQuads(int num, ItemPool::Alloc* existingAlloc);
    void releaseTexture(Texture* tex);
    void bindBitmap(Bitmap* bitmap);
    void invalidateQuads(ItemPool::Alloc* alloc);

    // To be implemented by GL, Metal, Vulkan, etc
    virtual Surface* getPrimarySurface() =0;
    virtual Surface* createPrivateSurface() =0;
    virtual Shader* getShader(ShaderFeatures features)=0;
    virtual void setVertexConfig(int vertexConfig)=0;
    virtual void pushClip(RECT clip) =0;
    virtual void popClip() =0;
    virtual Texture* createTexture(Bitmap* bitmap)=0;
    virtual void flushQuadBuffer() =0;
    virtual void setBlendMode(int blendMode) =0;
    virtual void drawQuads(int numQuads, int index) =0;
    virtual void prepareToDraw() =0;

    static Renderer* create();
    static Renderer* current;
    
protected:
    Renderer();

    // QuadBuffer data
    ItemPool _quadBuffer;
    bool _fullBufferUploadNeeded;
    uint8_t* _quadBufferDirtyLo;
    uint8_t* _quadBufferDirtyHi;

};
