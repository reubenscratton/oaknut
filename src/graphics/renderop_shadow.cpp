//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class ShadowShader : public Shader {
public:
    ShadowShader(Renderer* renderer) : Shader(renderer, Features(true, SDF_ROUNDRECT_1, false, Texture::Type::None)) {
        _u_sigma = declareUniform("sigma", VariableType::Float1, Uniform::Usage::Fragment);
    }
    string getFragmentSource() override {
        return
        SL_FLOAT1 " sigma=" SL_UNIFORM(sigma) ";\n"
        SL_FLOAT2 " size = " SL_UNIFORM(u) ".xy - sigma*2;\n"
        SL_FLOAT2 " lower = -size;\n"
        SL_FLOAT2 " upper = size;\n"
        // texcoord is fragment xy in surface coords
        SL_FLOAT2 " p=" SL_VERTEX_OUTPUT(texcoord) ";\n"
        SL_FLOAT4 " query = " SL_FLOAT4 "(p - lower, p - upper);\n"
        SL_FLOAT4 " x = query * (sqrt(0.5) / sigma);\n"
        SL_FLOAT4 " s = sign(x), a = abs(x);\n"
        "x = 1.0 + (0.278393 + (0.230389 + 0.078108 * (a * a)) * a) * a;\n"
        "x *= x;\n"
        SL_FLOAT4 " erf = s - s / (x * x);\n"
        SL_FLOAT4 " integral = 0.5 + 0.5 * erf;\n"
        SL_FLOAT1 " shadow = (integral.z - integral.x) * (integral.w - integral.y);\n"
        SL_OUTPIXVAL " = " SL_HALF4 "(0.0, 0.0, 0.0, 0.5 * shadow);\n";
    }
    
    int16_t _u_sigma;
};


void ShadowRenderOp::validateShader(RenderTask* r) {
    _shader = new ShadowShader(r->_renderer);
    _blendMode = BLENDMODE_NORMAL;
}

void ShadowRenderOp::prepareToRender(RenderTask* r, class Surface* surface) {
    RectRenderOp::prepareToRender(r, surface);
    ShadowShader* shader = _shader.as<ShadowShader>();
    r->setUniform(shader->_u_sigma, _sigma);
    //r->setUniform(shader->_u_origin, POINT(0, 0));
}

void ShadowRenderOp::setSigma(float sigma) {
    if (_sigma != sigma) {
        _sigma = sigma;
        invalidate();
        updateRect();
    }
}
void ShadowRenderOp::setRect(const RECT& rect) {
    _baseRect = rect;
    updateRect();
}

void ShadowRenderOp::updateRect() {
    RECT rect = _baseRect;
    rect.inset(-_sigma*2, -_sigma*2);
    rect.origin.y += _sigma;
    RectRenderOp::setRect(rect);
}


