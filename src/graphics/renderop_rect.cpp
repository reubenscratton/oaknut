//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


RectRenderOp::RectRenderOp() : RenderOp() {
    _blendMode = BLENDMODE_NONE;
}
COLOR RectRenderOp::getFillColor() const {
    return _color;
}
void RectRenderOp::setFillColor(COLOR fillColor) {
    setColor(fillColor);
    _blendMode = ((fillColor>>24) < 255) ? BLENDMODE_NORMAL : BLENDMODE_NONE;
}

void RectRenderOp::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        invalidate();
    }
}

void RectRenderOp::setStrokeWidth(float strokeWidth) {
    if (_strokeWidth != strokeWidth) {
        _strokeWidth = strokeWidth;
        invalidate();
    }
}
void RectRenderOp::setStrokeColor(COLOR strokeColor) {
    if (_strokeColor != strokeColor) {
        _strokeColor = strokeColor;
        invalidate();
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
        
        bool singleRadius = (radii[0]==radii[1] && radii[2]==radii[3] && radii[0]==radii[3]);
        if (singleRadius) {
            if (radii[0] == 0.0f) {
                _flags = _flags & ~(OPFLAG_CORNER1|OPFLAG_CORNER4);
            } else {
                _flags |= OPFLAG_CORNER1;
            }
        } else {
            _flags |= OPFLAG_CORNER4;
        }
        invalidate();
    }
}

void RectRenderOp::validateShader(RenderTask* r) {
    Shader::Features features;
    if (_alpha<1.0f) {
        features.alpha = 1;
        _blendMode = BLENDMODE_NORMAL;
    } else {
        _blendMode = ((_color>>24) < 255) ? BLENDMODE_NORMAL : BLENDMODE_NONE;
    }
    if (_strokeWidth>0 && _strokeColor!=0) {
        
    }
    bool singleRadius = (_cornerRadii[0]==_cornerRadii[1] && _cornerRadii[2]==_cornerRadii[3] && _cornerRadii[0]==_cornerRadii[3]);
    if (singleRadius) {
        if (_cornerRadii[0] != 0.0f) {
            features.sdf = SDF_ROUNDRECT_1;
        }
    } else {
        if ((_cornerRadii[0]==_cornerRadii[2]) && (_cornerRadii[1]==_cornerRadii[3])) {
            features.sdf = SDF_ROUNDRECT_2H;
        }
        else {
            features.sdf = SDF_ROUNDRECT_4;

        }
    }
    _blendMode = BLENDMODE_NORMAL;
    _shader = r->_renderer->getStandardShader(features);
}

void RectRenderOp::prepareToRender(RenderTask* r, class Surface* surface) {
    RenderOp::prepareToRender(r, surface);
    if (_shader->_features.alpha) {
        r->setUniform(_shader->_u_alpha, _alpha);
    }
    if (_shader->_features.sdf != SDF_NONE) {
        r->setUniform(_shader->_u_strokeColor, _strokeColor);
        r->setUniform(_shader->_u_u, VECTOR4(_rect.size.width/2, _rect.size.height/2,0, _strokeWidth));
        if (_shader->_features.sdf == SDF_ROUNDRECT_1) {
            r->setUniform(_shader->_u_cornerRadius, _cornerRadii[0]);
        } else {
            r->setUniform(_shader->_u_cornerRadii, _cornerRadii);
        }
    }
}



bool RectRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) {
        return false;
    }
    if (_shader->_features != op->_shader->_features) {
        return false;
    }
    if (_shader->_features.sdf != SDF_NONE) {
        return _rect.size.width == ((const RectRenderOp*)op)->_rect.size.width
        && _rect.size.height == ((const RectRenderOp*)op)->_rect.size.height
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
    if (_shader->_features.sdf != SDF_NONE) {
        // Put the quad size into the texture coords so the shader
        // can trivially calc distance to quad center
        quad->tl.s = quad->bl.s = -_rect.size.width/2;
        quad->tl.t = quad->tr.t = -_rect.size.height/2;
        quad->tr.s = quad->br.s = _rect.size.width/2;
        quad->bl.t = quad->br.t = _rect.size.height/2;
    }
}






