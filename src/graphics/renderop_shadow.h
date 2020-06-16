//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
class ShadowRenderOp : public RectRenderOp {
public:
    
    void validateShader(RenderTask* r) override;

    void prepareToRender(RenderTask* r, class Surface* surface) override;
    
    void setSigma(float sigma);

    virtual void setRect(const RECT& rect) override;
    void updateRect();
    
protected:
    float _sigma;
    RECT _baseRect;
};
