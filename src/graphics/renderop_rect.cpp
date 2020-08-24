//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ShaderFactory<RectShader> s_factory;

RectShader* RectShader::get(Renderer* r, Features features) {
    return s_factory.get(r, features);
}


RectShader::RectShader(Renderer* renderer, Features features) : Shader(renderer), _features(features) {
    bool useTexCoordAttrib = features.tex0 != Texture::Type::None;
    if (features.sdf != SDF_NONE) {
        _u_strokeColor = declareUniform("strokeColor", VariableType::Color);
        _u_u = declareUniform("u", VariableType::Float4);
        switch (features.sdf) {
        case SDF_ROUNDRECT_1:
            _u_cornerRadius = declareUniform("cornerRadius", VariableType::Float1);
            break;
        case SDF_ROUNDRECT_4:
            _u_cornerRadii = declareUniform("cornerRadii", VariableType::Float4);
            break;
        }
        useTexCoordAttrib = true;
    }
    
    if (useTexCoordAttrib) {
        declareAttribute("texcoord", VariableType::Float2);
    }

}

int RectShader::getTextureCount()  {
    return (_features.tex0 == Texture::Type::None) ? 0 : 1;
}
Texture::Type RectShader::getTextureType(int index) {
    return (Texture::Type)_features.tex0;
}

string RectShader::getSupportingSource() {
    Features* features = (Features*)&_features;
    if (features->sdf == SDF_NONE) {
        return "";
    }
    string ss = "";
    // SDF implementations!
    if (features->sdf == SDF_ROUNDRECT_1) {
        ss += SL_FLOAT1 " sdf_roundrect(" SL_FLOAT2 " p, " SL_FLOAT2 " b, " SL_FLOAT1 " r) {\n"
              SL_FLOAT2 " q = abs(p) - b + " SL_FLOAT2 "(r);\n"
              " return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r;\n"
              "}";
    }
    
/*
 float sdRoundedBox( in vec2 p, in vec2 b, in vec4 r )
 {
     r.xy = (p.x>0.0)?r.xy : r.zw;
     r.x  = (p.y>0.0)?r.x  : r.y;
     vec2 q = abs(p)-b+r.x;
     return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r.x;
 }
 */
/*      SL_FLOAT2 " size = " SL_UNIFORM(u) ".xy;\n"
        SL_FLOAT2 " r = " SL_UNIFORM(cornerRadii) ".xw\n;" // TODO: this is specific to left|right config
        "float s=step(" SL_VERTEX_OUTPUT(texcoord) ".x,0.0);\n"
        "float radius = s*r.x + (1.0-s)*r.y;\n"
        "size -= " SL_FLOAT2 "(radius);\n"
        SL_FLOAT2 " d = abs(" SL_VERTEX_OUTPUT(texcoord) ") - size;\n"
        "float dist = min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;\n";
  */

    return ss;
}

string RectShader::getFragmentSource() {
    Features* features = (Features*)&_features;
    bool useTexSampler = false;
    bool useTexCoords = false;
    
    // If shader uses an SDF, we put quad size into texcoord attribute
    // TODO: this is bogus cos it prevents textured SDFs. Needs a dedicated attrib, which requires dynamic vertex structure
    if (features->sdf != SDF_NONE) {
        useTexCoords = true;
    }
    if (features->tex0) {
        useTexSampler = true;
        useTexCoords = true;
        //_sampler.set(0);
    }
    
    
    
    
    string fs = "c = ";
    if (useTexSampler) {
        switch (features->tex0) {
            case Texture::Type::None:
                break;
            case Texture::Type::Normal:
                fs += SL_TEXSAMPLE_2D("texture",  SL_VERTEX_OUTPUT(texcoord));
                break;
            case Texture::Type::Rect:
                fs += SL_TEXSAMPLE_2D_RECT("texture",  SL_VERTEX_OUTPUT(texcoord));
                break;
            case Texture::Type::OES:
                fs += SL_TEXSAMPLE_2D_OES("texture",  SL_VERTEX_OUTPUT(texcoord));
                break;
        }
        fs += ";\n";
        
        if (features->tint) {
            fs += "    c.rgb = " SL_VERTEX_OUTPUT(color) ".rgb;\n";
        }
    } else {
        fs += SL_VERTEX_OUTPUT(color);
        fs += ";\n";

    }
    

    if (features->sdf == SDF_NONE) {
        fs += SL_OUTPIXVAL " = c;\n";
    } else {
        
        // Get two distance values, one for the outer edge and one for the inner edge
        fs += SL_FLOAT2 " p = " SL_VERTEX_OUTPUT(texcoord) ";\n";
        fs += SL_FLOAT2 " b = " SL_UNIFORM(u) ".xy;\n";
        fs += SL_HALF4_DECL " strokeColor(" SL_UNIFORM(strokeColor) ");\n";
        fs += SL_FLOAT1 " dOuter = sdf_roundrect(p, b, " SL_UNIFORM(cornerRadius) ");\n";
        fs += SL_FLOAT1 " strokeWidth = " SL_UNIFORM(u) ".w;\n";
        fs += SL_FLOAT1 " dInner = sdf_roundrect(p, b - strokeWidth, " SL_UNIFORM(cornerRadius) " - strokeWidth);\n";
        
        // Cheap AA
        fs += SL_HALF4_DECL " colOuter = strokeColor;\n"
                            " colOuter.a *= 1.0 - clamp(dOuter+0.5, 0.0, 1.0);\n";
        fs += SL_OUTPIXVAL " = mix(c, colOuter, clamp(dInner+0.5, 0.0, 1.0));\n";
    }
    
    
    if (features->alpha) {
        fs += SL_OUTPIXVAL ".a *= " SL_UNIFORM(alpha) ";\n";
    }

    return fs;
}


RectRenderOp::RectRenderOp() : RenderOp() {
    _blendMode = BLENDMODE_NONE;
}
COLOR RectRenderOp::getFillColor() const {
    COLOR c = _color;
    return c;
}
void RectRenderOp::setFillColor(COLOR fillColor) {
    setColor(fillColor);
    //invalidate();
}

void RectRenderOp::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        invalidateBatch();
    }
}

void RectRenderOp::setStrokeWidth(float strokeWidth) {
    if (_strokeWidth != strokeWidth) {
        _strokeWidth = strokeWidth;
        invalidateBatch();
    }
}
void RectRenderOp::setStrokeColor(COLOR strokeColor) {
    if (_strokeColor != strokeColor) {
        _strokeColor = strokeColor;
        invalidateBatch();
    }
}
VECTOR4 RectRenderOp::getCornerRadii() const {
    return _cornerRadii;
}
void RectRenderOp::setCornerRadius(float radius) {
    setCornerRadii({radius, radius, radius, radius});
}
void RectRenderOp::setCornerRadii(const VECTOR4& radii) {
    if (_cornerRadii != radii) {
        _cornerRadii = radii;
        invalidateBatch();
    }
}


void RectRenderOp::validateShader(RenderTask* r) {
    _blendMode = BLENDMODE_NORMAL;

    RectShader::Features features;
    if (_alpha<1.0f || (_color & 0xFF000000)<0xFF000000) {
        features.alpha = 1;
    } else {
        features.alpha = 0;
    }
//    if (_strokeWidth>0 && _strokeColor!=0) {
//    }
    bool singleRadius = (_cornerRadii[0]==_cornerRadii[1] && _cornerRadii[2]==_cornerRadii[3] && _cornerRadii[0]==_cornerRadii[3]);
    if (singleRadius) {
        if (_cornerRadii[0] != 0.0f) {
            features.sdf = SDF_ROUNDRECT_1;
        }
    } else {
        features.sdf = SDF_ROUNDRECT_4;
    }

    _shader = s_factory.get(r->_renderer, features);
}

void RectRenderOp::prepareToRender(RenderTask* r, class Surface* surface) {
    RenderOp::prepareToRender(r, surface);
    RectShader* shader = _shader.as<RectShader>();
    RectShader::Features* features = (RectShader::Features*)&shader->_features;

    if (features->alpha) {
        r->setUniform(_shader->_u_alpha, _alpha);
    }
    if (features->sdf != SDF_NONE) {
        r->setUniform(shader->_u_strokeColor, _strokeColor);
        r->setUniform(shader->_u_u, VECTOR4(_rect.size.width/2, _rect.size.height/2,0, _strokeWidth));
        if (features->sdf == SDF_ROUNDRECT_1) {
            r->setUniform(shader->_u_cornerRadius, _cornerRadii[0]);
        } else {
            r->setUniform(shader->_u_cornerRadii, _cornerRadii);
        }
    }
}



bool RectRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) {
        return false;
    }
    RectShader* shader = _shader.as<RectShader>();
    RectShader::Features* features = (RectShader::Features*)&shader->_features;
    if (features->sdf != SDF_NONE) {
        return _rect.size == ((const RectRenderOp*)op)->_rect.size
        && _strokeColor==((const RectRenderOp*)op)->_strokeColor
        && _strokeWidth==((const RectRenderOp*)op)->_strokeWidth
        && _cornerRadii[0] ==((const RectRenderOp*)op)->_cornerRadii[0]
        && _cornerRadii[1] ==((const RectRenderOp*)op)->_cornerRadii[1]
        && _cornerRadii[2] ==((const RectRenderOp*)op)->_cornerRadii[2]
        && _cornerRadii[3] ==((const RectRenderOp*)op)->_cornerRadii[3];
    }
    return true;
}



void RectRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    RectShader* shader = _shader.as<RectShader>();
    RectShader::Features* features = (RectShader::Features*)&shader->_features;
    if (features->sdf != SDF_NONE) {
        // Put the quad size into the texture coords so the shader
        // can trivially calc distance to quad center
        quad->tl.s = quad->bl.s = -_rect.size.width/2;
        quad->tl.t = quad->tr.t = -_rect.size.height/2;
        quad->tr.s = quad->br.s = _rect.size.width/2;
        quad->bl.t = quad->br.t = _rect.size.height/2;
    }
}






