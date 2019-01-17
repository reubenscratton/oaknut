//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class BlurShader : public Shader {
public:
    BlurShader(Renderer* renderer, class BlurRenderOp* op);

    string getVertexSource() override;
    string getFragmentSource() override;

    int16_t _u_sampler;
    int16_t _u_texOffset;
    BlurRenderOp* _op;
};

class PostBlurShader : public Shader {
public:
    PostBlurShader(Renderer* renderer);
    string getVertexSource() override;
    string getFragmentSource() override;
};


class BlurRenderOp : public RenderOp {
public:
	BlurRenderOp();
    ~BlurRenderOp();

    // Overrides
    void setRect(const RECT& rect) override;
    void asQuads(QUAD *quad) override;
    void validateShader(Renderer* renderer) override;
    void prepareToRender(Renderer* renderer, Surface* surface) override;
    
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
    
    sp<Texture> _tex1;
    sp<Surface> _surface1;
    sp<Surface> _surface2;
    sp<BlurShader> _blurShader;
    MATRIX4* _pmvp;
    

};


