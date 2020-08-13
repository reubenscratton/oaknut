//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
class ShadowRenderOp : public RenderOp {
public:
    
    void setSigma(float sigma);

    void validateShader(RenderTask* r) override;
    void prepareToRender(RenderTask* r, class Surface* surface) override;
    void asQuads(QUAD *quad) override;
    void setRect(const RECT& rect) override;
    void updateRect();
    
protected:
    float _sigma;
    RECT _baseRect;
};
