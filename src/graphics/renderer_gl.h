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
    bool _allocd;
    bool _samplingConfigValid;
    bool _usesSharedMem; // i.e. GPU reads directly from memory, no need to glTexture2D() every time pixels change.
    
    GLTexture(Renderer* renderer, int format);

    bool readPixels(RECT rect, bytearray& target) const override;

};

class GLSurface : public Surface {
public:
    GLuint _fb;
    bool _doneInit;
    bool _fbAttachmentsValid;

    GLSurface(Renderer* renderer, bool isPrivate);
    ~GLSurface();
    
    void setSize(const SIZE& size) override;

};


class GLRenderer : public Renderer {
public:
    GLRenderer();

    // Overrides
    Texture* createTexture(int format) override;
    void releaseTexture(Texture* texture) override;
    RenderTask* createRenderTask() override;
    void* createShaderState(Shader* shader) override;
    
    int getIntProperty(IntProperty property) override;
    void deleteShaderState(void* state) override;
    void flushQuadBuffer() override;
    void uploadQuad(ItemPool::Alloc* alloc) override;
    
    void bindVertexBuffer();
    
protected:

    GLuint _indexBufferId;
    GLuint _vertexBufferId;
    GLuint _vao;
    GLshort* _indexes;
  
    void initVertexBuffers();
};

#endif

