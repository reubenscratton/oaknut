//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if RENDERER_GL

class GLTexture : public Texture {
public:
    GLuint _texTarget;       // default GL_TEXTURE_2D
    GLuint _textureId;
    bool _paramsValid = false;
    
    
    GLTexture(Renderer* renderer, int format);
    virtual void updateParams();
    virtual void upload();

    void resize(int width, int height) override;
    
protected:
    void realloc(int width, int height, void* pixelData, bool sizeChanged);
};


class GLRenderer : public Renderer {
public:

    // Overrides
    Surface* getPrimarySurface() override;
    Surface* createPrivateSurface() override;
    void setCurrentSurface(Surface* surface) override;
    void* createShaderState(Shader* shader) override;
    void deleteShaderState(void* state) override;
    void setCurrentTexture(Texture* texture) override;
    void setCurrentBlendMode(int blendMode) override;
    Texture* createTexture(int format) override;
    void releaseTexture(Texture* texture) override;
    void prepareToDraw() override;
    void pushClip(RECT clip) override;
    void popClip() override;
    void flushQuadBuffer() override;
    void uploadQuad(ItemPool::Alloc* alloc) override;
    void draw(PrimitiveType type, int count, int index) override;
    void copyFromCurrent(const RECT& rect, Texture* destTex, const POINT& destOrigin) override;
    void generateMipmaps(Texture* tex) override;
    
    void setColorUniform(int16_t uniformIndex, const float* rgba) override;
    void setUniformData(int16_t uniformIndex, const void* data, int32_t cb) override;
    
    int getIntProperty(IntProperty property) override;
    
    GLuint _unpackAlignment;
protected:
    GLRenderer(Window* window);

    GLfloat _backgroundColor[4]; // TODO: this belongs on GLSurface...
    GLuint _indexBufferId;
    GLuint _vertexBufferId;
    GLuint _vao;
    GLshort* _indexes;
};

#endif

