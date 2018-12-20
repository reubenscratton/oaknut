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
    
    
    GLTexture(Bitmap* bitmap);
    
    void unload() override;
    void bind() override;
    void upload() override;
    int getSampler() override;
    
};


// GL Sampler types. There are 11 of these in the GLSL spec, see:
// https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Sampler_types

#define GLSAMPLER_NONE 0
#define GLSAMPLER_TEXTURE_2D 1
#define GLSAMPLER_TEXTURE_EXT_OES 2


class GLRenderer : public Renderer {
public:

    // Overrides
    Surface* getPrimarySurface() override;
    Surface* createPrivateSurface() override;
    Shader* getShader(ShaderFeatures features) override;
    void setVertexConfig(int vertexConfig) override;
    void pushClip(RECT clip) override;
    void popClip() override;
    Texture* createTexture(Bitmap* bitmap) override;
    void flushQuadBuffer() override;
    void setBlendMode(int blendMode) override;
    void drawQuads(int numQuads, int index) override;
    void prepareToDraw() override;

    // Helpers
    void convertTexture(GLTexture* texture, int width, int height);
    
protected:
    GLRenderer();
    
    GLfloat _backgroundColor[4]; // TODO: this belongs on GLSurface...
    GLuint _indexBufferId;
    GLuint _vertexBufferId;
    GLuint _vao;
    GLshort* _indexes;
};

#endif

