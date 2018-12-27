//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class BlurRenderOp : public RenderOp {
public:
	BlurRenderOp();
    ~BlurRenderOp();

    // These are also in PrivateSurfaceRenderOp...
    bool _dirty;
    ItemPool::Alloc* _alloc;
    
    SIZEI _fullSizePow2;
    SIZEI _downsampledSize;
    MATRIX4 _mvp;
    int _blurRadius;
    float _sigma;
    vector<float> _standardGaussianWeights;
    vector<float> _optimizedOffsets;
#if RENDERER_GL
    GLuint _textureIds[3];
    GLuint _fb[2];
#endif
    sp<Shader> _blurShader;
    MATRIX4* _pmvp;
    
    // Overrides
    void setRect(const RECT& rect) override;
    void asQuads(QUAD *quad) override;
    void validateShader(Renderer* renderer) override;
    void prepareToRender(Renderer* renderer, Surface* surface) override;
    void render(Renderer* renderer, int numQuads, int vboOffset) override;

private:
    // Renderer-specific helpers
    void rendererLoad(Renderer* renderer);
    void rendererResize(Renderer* renderer);
    void rendererUnload(Renderer* renderer);

};


