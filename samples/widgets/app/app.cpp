//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

class InkShader : public Shader {
public:
    InkShader(Renderer* renderer) : Shader(renderer, Features(true, 1, false, Texture::Type::None)) {
        _u_origin = declareUniform("origin", VariableType::Float2, Uniform::Usage::Fragment);
        _u_radius = declareUniform("radius2", VariableType::Float1, Uniform::Usage::Fragment);
    }
    
    string getFragmentSource() override {
        return
        SL_FLOAT2 " p=" SL_VERTEX_OUTPUT(texcoord) ";\n"
        SL_FLOAT1 " d = (length(p) - " SL_UNIFORM(radius2) ");\n"
        SL_FLOAT1 " a;\n"
        "if (d >= 0.0) {\n"
        "    a =0.0;\n"
        "} else {\n"
        "    a =0.4;\n"
        "}\n"
        SL_OUTPIXVAL "=half4(1.0,1.0,1.0,a * " SL_UNIFORM(alpha) ");\n";
    }
    
    int16_t _u_origin;
    int16_t _u_radius;
};

class MDInkRenderOp : public RectRenderOp {
public:
    
    void validateShader(RenderTask* r) override {
        _shader = new InkShader(r->_renderer);
        _blendMode = BLENDMODE_NORMAL;
    }
    void setOrigin(POINT origin) {
        if (origin != _origin) {
            _origin = origin;
            invalidate();
        }
    }
    void setRadius(float radius) {
        if (radius != _radius) {
            _radius = radius;
            invalidate();
        }
    }

    void asQuads(QUAD *quad) override {
        rectToSurfaceQuad(_rect, quad);
        if (1 /*_signedDistanceField*/) {
            // Put the quad size into the texture coords so the frag shader
            // can trivially know distance to quad center
            quad->tl.s = quad->bl.s = -_rect.size.width/2;
            quad->tl.t = quad->tr.t = -_rect.size.height/2;
            quad->tr.s = quad->br.s = _rect.size.width/2;
            quad->bl.t = quad->br.t = _rect.size.height/2;
        }
    }
    
    void prepareToRender(RenderTask* r, class Surface* surface) override {
        RenderOp::prepareToRender(r, surface);
        InkShader* shader = _shader.as<InkShader>();
        r->setUniform(shader->_u_alpha, _alpha);
        r->setUniform(shader->_u_radius, _radius);
        r->setUniform(shader->_u_origin, _origin);
    }

    POINT _origin;
    float _radius;
};

class MDButton : public Button {
public:
    MDButton() : Button() {
        _inkOp = new MDInkRenderOp();
        _inkOp->setAlpha(1);
        _inkOp->setColor(0xFF00FF00);
        addRenderOp(_inkOp);
    }
    bool handleInputEvent(INPUTEVENT* event) override {
        if (event->type == INPUT_EVENT_DOWN) {
            _inkOp->setOrigin(event->ptLocal);
            _inkOp->setAlpha(1);
            _rippleAnim = Animation::start(this, 500, [=](float val) {
                _inkOp->setRadius(500*val);
                setNeedsFullRedraw();
            });
        }
        if (event->type == INPUT_EVENT_UP) {
            _fadeAnim = Animation::start(this, 500, [=](float val) {
                _inkOp->setAlpha(1-val);
                setNeedsFullRedraw();
            });

        }
        return Button::handleInputEvent(event);
    }
    //if (event->type == INPUT_EVENT_DOWN) {
    //    setPressed(true);
    //}

    void applyStatemapStyleValue(const string& name, const style& value) override {
        if (name == "pressed") {
            
        }
    }
    void updateBackgroundRect() override {
        View::updateBackgroundRect();
        _inkOp->setRect(getOwnRect());
    }
    
    MDInkRenderOp* _inkOp;
    sp<Animation> _rippleAnim;
    sp<Animation> _fadeAnim;
};

class MainViewController : public ViewController {
public:

    MainViewController() {
        View* view = new View();
        
        _button = new MDButton();
        _button->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        _button->setLayoutOrigin(ALIGNSPEC::Center(), ALIGNSPEC::Center());
        _button->setBackgroundColor(0xFFea2162);
        _button->setText("BUTTON");
        _button->setTextColor(0xFFFFFFFF);
        
        view->addSubview(_button);
        
        setView(view);
    }


    MDButton* _button;

};


class WidgetsApp : public App {
public:
    
    void main() override {
        MainViewController* mainVC = new MainViewController();
        _window->setRootViewController(mainVC);
    };

};

static WidgetsApp the_app;

