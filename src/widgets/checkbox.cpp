//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(Checkbox);


/*
Vertexes for the checkbox tick, as per https://material.io/develop/web/components/input-controls/checkboxes
 
  1.73, 12.91
  8.10, 19.28
 22.79,  4.59    (relative to a 24x24 grid)
 
 */
class TickShader : public Shader {
public:
    struct Features {
        uint32_t _;
        bool operator<(const struct Features &rhs) const {
            return _ < rhs._;
        }
    } _features;
    
    TickShader(Renderer* renderer, Features features) : Shader(renderer), _features(features) {
        declareAttribute("texcoord", VariableType::Float2);
        _u_anim = declareUniform("anim", VariableType::Float1, Uniform::Usage::Fragment);
    }
    string getFragmentSource() override {
        return
// x = ((x-12)/24.0) * 4/3
        SL_FLOAT2 " v1(1.73/24.0 -0.5,12.91/24.0-0.5);\n"
        SL_FLOAT2 " v2(8.1/24.0-0.5,19.28/24.0-0.5);\n"
        SL_FLOAT2 " v3(22.79/24.0-0.5,4.59/24.0-0.5);\n"
        SL_FLOAT1 " half_width = 2.25;\n"
        
        SL_FLOAT2 " size(24.0, 24.0);\n"
        
        "v1 *= size;\n"
        "v2 *= size;\n"
        "v3 *= size;\n"

        SL_FLOAT2 " p=" SL_VERTEX_OUTPUT(texcoord) ";\n"

        // NB: dir and lngth can be uniforms, i.e. tick data is v1,dir1,lngth1, v2,dir2,lngth2. No need for v3.
        
 
        // Line #1
        SL_FLOAT2 " dir = v1 - v2;\n"
        SL_FLOAT1 " lngth = length(dir);\n"
                  " dir /= lngth;\n"
        SL_FLOAT2 " proj = max(0.0, min(lngth, dot((v1 - p), dir))) * dir;"
        SL_FLOAT1 " d1= length( (v1 - p) - proj ) - half_width;\n"

        // Line #2
                  " dir = v2 - v3;\n"
                  " lngth = length(dir);\n"
                  " dir /= lngth;\n"
                  " proj = max(0.0, min(lngth, dot((v2 - p), dir))) * dir;"
        SL_FLOAT1 " d2= length( (v2 - p) - proj ) - half_width;\n"

        // Union of the two lines
        SL_FLOAT1 " dist = min(d1,d2);\n"
        
        // Clamp horizontally
        SL_FLOAT1 " max_x = (" SL_UNIFORM(anim) "-0.5) * size.x;\n"
        "dist *= step(p.x, max_x);"

        // Tint
        SL_OUTPIXVAL " = " SL_VERTEX_OUTPUT(color) ";\n"

        // Alpha
        SL_OUTPIXVAL ".a = clamp(-dist, 0.0, 1.0);\n";
    }
    
    int16_t _u_anim;
};

static ShaderFactory<TickShader> s_factory;

class RenderOp_Tick : public RectRenderOp {
public:
    
    RenderOp_Tick() : RectRenderOp() {
        _anim = 1.0f;
    }
    
    void validateShader(RenderTask* r) override {
        _shader = s_factory.get(r->_renderer, {});
        _blendMode = BLENDMODE_NORMAL;

    }
    void prepareToRender(RenderTask* r, class Surface* surface) override {
        RectRenderOp::prepareToRender(r, surface);
        TickShader* shader = _shader.as<TickShader>();
        r->setUniform(shader->_u_anim, _anim);
    }
    
    void asQuads(QUAD *quad) override {
        rectToSurfaceQuad(_rect, quad);
        quad->tl.s = quad->bl.s = -_rect.size.width/2;
        quad->tl.t = quad->tr.t = -_rect.size.height/2;
        quad->tr.s = quad->br.s = _rect.size.width/2;
        quad->bl.t = quad->br.t = _rect.size.height/2;
    }

    void setAnim(float v) {
        _anim = v;
        invalidateBatch();
    }
    
    float _anim;
};

Checkbox::Checkbox() {
    _pressable = true;
    _renderOpBox = new RectRenderOp();
    _renderOpTick = new RenderOp_Tick();
    addRenderOp(_renderOpBox);
    _updateRenderOpsNeeded = true;

}

bool Checkbox::applySingleStyle(const string &name, const style &value) {
    if (name == "box-color") {
        _boxColor = value.colorVal();
        if (isChecked()) {
            _renderOpBox->setColor(_boxColor);
        }
        return true;
    }
    if (name == "box-corner-radius") {
        _boxCornerRadius = value.floatVal();
        _renderOpBox->setCornerRadius(_boxCornerRadius);
        return true;
    }
    if (name == "tick-color") {
        _renderOpTick->setColor(value.colorVal());
        return true;
    }
    return View::applySingleStyle(name, value);
}

void Checkbox::updateRenderOps() {
    if (isChecked()) {
        if (!_renderOpTick->_view) {
            addRenderOp(_renderOpTick);
        }
        _renderOpBox->setColor(_boxColor);
        _renderOpBox->setStrokeColor(0);
        _renderOpBox->setStrokeWidth(0);
    } else {
        if (_renderOpTick->_view) {
            removeRenderOp(_renderOpTick);
        }
        _renderOpBox->setFillColor(0);
        _renderOpBox->setStrokeColor(0xFF888888);
        _renderOpBox->setStrokeWidth(4);
    }
}

void Checkbox::layout(RECT constraint) {
    View::layout(constraint);
    RECT boxRect = getOwnRectPadded();
    _renderOpBox->setRect(boxRect);
    _renderOpTick->setRect(boxRect);
}

bool Checkbox::handleInputEvent(INPUTEVENT* event) {
    if (event->type == INPUT_EVENT_TAP) {
        setChecked(!isChecked());
        return true;
    }
    return View::handleInputEvent(event);
}

void Checkbox::onStateChanged(VIEWSTATE changes) {
    View::onStateChanged(changes);
    if (changes.mask & STATE_CHECKED) {
        _updateRenderOpsNeeded = true;
        if (onIsCheckedChanged) {
            onIsCheckedChanged(this, isChecked());
        }
        Animation::start(this, 200, [=](float v) {
            _renderOpTick.as<RenderOp_Tick>()->setAnim(v);
        }, Animation::regularEaseIn);
    }
}
