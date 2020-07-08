//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

class InkShader : public Shader {
public:
    InkShader(Renderer* renderer) : Shader(renderer, Features(true, SDF_ROUNDRECT_1, true, Texture::Type::None)) {
        _u_origin = declareUniform("origin", VariableType::Float2, Uniform::Usage::Fragment);
        _u_radius = declareUniform("radius", VariableType::Float1, Uniform::Usage::Fragment);
    }
    
    string getFragmentSource() override {
        return
        SL_FLOAT2 " p=" SL_VERTEX_OUTPUT(texcoord) ";\n"
        
        // Simple circle SDF
        SL_FLOAT1 " dist = length(p - " SL_UNIFORM(origin) ") - " SL_UNIFORM(radius) ";\n"
        
        // Union with button background SDF (ie roundrect or nothing)
        SL_FLOAT2 " b = " SL_UNIFORM(u) ".xy - " SL_FLOAT2 "(" SL_UNIFORM(cornerRadius) ");\n"
        "dist = max(dist, length(max(abs(p)-b, 0.0)) - " SL_UNIFORM(cornerRadius) "  - 0.5);\n"

        // Output vertex color where alpha is mixed with SDF alpha and uniform alpha
        SL_OUTPIXVAL "=" SL_VERTEX_OUTPUT(color) ";\n"
        SL_OUTPIXVAL ".a *= sign(-dist) * " SL_UNIFORM(alpha) ";\n";
    }
    
    int16_t _u_origin;
    int16_t _u_radius;
};


InkRenderOp::InkRenderOp() : RectRenderOp() {
    _blendMode = BLENDMODE_NORMAL;
}

void InkRenderOp::validateShader(RenderTask* r) {
    _shader = new InkShader(r->_renderer);
}

void InkRenderOp::setOrigin(POINT origin) {
    if (origin != _origin) {
        _origin = origin;
        invalidate();
    }
}

void InkRenderOp::setRadius(float radius) {
    if (radius != _radius) {
        _radius = radius;
        invalidate();
    }
}

void InkRenderOp::prepareToRender(RenderTask* r, class Surface* surface) {
    RectRenderOp::prepareToRender(r, surface);
    InkShader* shader = _shader.as<InkShader>();
    r->setUniform(shader->_u_origin, _origin);
    r->setUniform(shader->_u_radius, _radius);
}


