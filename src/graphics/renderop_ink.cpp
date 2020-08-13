//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

class InkShader : public Shader {
public:
    struct Features {
        uint32_t _;
        bool operator<(Features rhs) const {
            return _<rhs._;
        }
    } _features;
    
    InkShader(Renderer* renderer, Features features) : Shader(renderer), _features(features) {
        declareAttribute("texcoord", VariableType::Float2);
        _u_origin = declareUniform("origin", VariableType::Float2, Uniform::Usage::Fragment);
        _u_radius = declareUniform("radius", VariableType::Float1, Uniform::Usage::Fragment);
        _u_halfSize = declareUniform("halfSize", VariableType::Float2, Uniform::Usage::Fragment);
        _u_cornerRadius = declareUniform("cornerRadius", VariableType::Float1, Uniform::Usage::Fragment);
    }
    
    string getFragmentSource() override {
        return
        SL_FLOAT2 " p=" SL_VERTEX_OUTPUT(texcoord) ";\n"
        
        // Simple circle SDF
        SL_FLOAT1 " dist = length(p - " SL_UNIFORM(origin) ") - " SL_UNIFORM(radius) ";\n"
        
        // Union with button background SDF (ie roundrect or nothing)
        SL_FLOAT2 " b = " SL_UNIFORM(halfSize) " - " SL_FLOAT2 "(" SL_UNIFORM(cornerRadius) ");\n"
        "dist = max(dist, length(max(abs(p)-b, 0.0)) - " SL_UNIFORM(cornerRadius) "  - 0.5);\n"

        // Output vertex color where alpha is mixed with SDF alpha and uniform alpha
        SL_OUTPIXVAL "=" SL_VERTEX_OUTPUT(color) ";\n"
        SL_OUTPIXVAL ".a *= sign(-dist) * " SL_UNIFORM(alpha) ";\n";
    }
    
    int16_t _u_origin;
    int16_t _u_radius;
    int16_t _u_halfSize;
    int16_t _u_cornerRadius;
};

static ShaderFactory<InkShader> s_factory;

InkRenderOp::InkRenderOp() : RenderOp() {
    _blendMode = BLENDMODE_NORMAL;
}

void InkRenderOp::validateShader(RenderTask* r) {
    _shader = s_factory.get(r->_renderer, {});
}

bool InkRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) {
        return false;
    }
    InkRenderOp* other = (InkRenderOp*)op;
    return _rect.size == other->_rect.size;
}

void InkRenderOp::setOrigin(POINT origin) {
    if (origin != _origin) {
        _origin = origin;
        invalidateBatch();
    }
}

void InkRenderOp::setRadius(float radius) {
    if (radius != _radius) {
        _radius = radius;
        invalidateBatch();
    }
}
void InkRenderOp::setCornerRadius(float cornerRadius) {
    if (cornerRadius != _cornerRadius) {
        _cornerRadius = cornerRadius;
        invalidateBatch();
    }
}


void InkRenderOp::prepareToRender(RenderTask* r, class Surface* surface) {
    RenderOp::prepareToRender(r, surface);
    InkShader* shader = _shader.as<InkShader>();
    r->setUniform(shader->_u_halfSize, VECTOR2(_rect.size.width/2, _rect.size.height/2));
    r->setUniform(shader->_u_cornerRadius, _cornerRadius);
    r->setUniform(shader->_u_origin, _origin);
    r->setUniform(shader->_u_radius, _radius);
    r->setUniform(shader->_u_alpha, _alpha);
}

void InkRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    // Put the quad size into the texture coords so the shader
    // can trivially calc distance to quad center
    quad->tl.s = quad->bl.s = -_rect.size.width/2;
    quad->tl.t = quad->tr.t = -_rect.size.height/2;
    quad->tr.s = quad->br.s = _rect.size.width/2;
    quad->bl.t = quad->br.t = _rect.size.height/2;
}

