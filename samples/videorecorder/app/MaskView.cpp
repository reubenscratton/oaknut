//
//  IdaaS
//
//  Copyright © 2018 PQ. All rights reserved.
//

#include "MaskView.h"

DECLARE_DYNCREATE(MaskView);

class MaskRenderProg : public GLProgram {
public:
    Uniform<VECTOR2> _holeRadii;
    Uniform<COLOR> _backgroundColour;
    Uniform<COLOR> _holeStrokeColour;
    Uniform<COLOR> _holeFillColour;
    Uniform<float> _holeStrokeWidth;
    Uniform<float> _holeCornerRadius;

    void lazyLoadUniforms() override {
        GLProgram::lazyLoadUniforms();
        _holeRadii.use();
        _backgroundColour.use();
        _holeStrokeColour.use();
        _holeFillColour.use();
        _holeStrokeWidth.use();
    }

    void findVariables() override {
        GLProgram::findVariables();
        _holeRadii.position = check_gl(glGetUniformLocation, _program, "holeRadii");
        _backgroundColour.position = check_gl(glGetUniformLocation, _program, "backgroundColour");
        _holeStrokeColour.position = check_gl(glGetUniformLocation, _program, "holeStrokeColour");
        _holeFillColour.position = check_gl(glGetUniformLocation, _program, "holeFillColour");
        _holeStrokeWidth.position = check_gl(glGetUniformLocation, _program, "holeStrokeWidth");
    }
    virtual void load() override {
        GLProgram::loadShaders(
                               TEXTURE_VERTEX_SHADER,
                               (
                               "varying vec2 v_texcoord;\n"
                               "uniform mediump vec2 holeRadii;\n"
                               "uniform mediump float holeStrokeWidth;\n"
                               "uniform lowp vec4 backgroundColour;\n"
                               "uniform lowp vec4 holeStrokeColour;\n"
                               "uniform lowp vec4 holeFillColour;\n"
                               + getMainProg()).data());
    }
                               
    virtual string getMainProg() {
       return "void main() {\n"
              "    gl_FragColor = backgroundColour;\n"
              "}\n";
    }

};

class MaskRenderProgOval : public MaskRenderProg {
public:
    
    // -inf to -strokeWidth/2 = fillColour
    // -strokeWidth/2 to strokeWidth/2 = strokeColour
    // strokeWidth/2 to +inf  = backgroundColour
    
    string getMainProg() override {
        return "void main() {\n"
               "    vec2 w=vec2(holeStrokeWidth/2.0,holeStrokeWidth/2.0);\n"
               "    vec2 ri=holeRadii - w;\n"
               "    vec2 ro=holeRadii + w;\n"
               "    vec2 p=v_texcoord.xy;"
               // Ellipse distance function adapted from 3D version at http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
               "    float d_o = (length(p/ro) - 1.0) * min(ro.x,ro.y);\n"
               "    if (d_o >= 0.0) {"
               "        gl_FragColor = mix(holeStrokeColour, backgroundColour, min(max(d_o,0.0),1.0));\n"
               "    } else {\n"
               "        float d_i = (length(p/ri) - 1.0) * min(ri.x,ri.y);\n"
               "        gl_FragColor = mix(vec4(0.0), holeStrokeColour, min(max(d_i,0.0),1.0));\n"
               "    }\n"
               "}\n";
    }
};


class MaskRenderProgRect : public MaskRenderProg {
public:

    void findVariables() override {
        MaskRenderProg::findVariables();
        _holeCornerRadius.position = check_gl(glGetUniformLocation, _program, "holeCornerRadius");
    }
    void lazyLoadUniforms() override {
        MaskRenderProg::lazyLoadUniforms();
        _holeCornerRadius.use();
    }

    // Round-rect shader adapted from https://www.shadertoy.com/view/ltS3zW
    string getMainProg() override {
        return "uniform mediump float holeCornerRadius;\n"
            "void main() {\n"
            "    vec2 p=v_texcoord.xy;\n"
            "    float halfBorder = holeStrokeWidth/2.0;\n"
            "    float r = holeCornerRadius - halfBorder;\n"
            "    vec2 d = abs(p) - holeRadii + vec2(r);\n"
            "    float fDist = min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;\n"
            "    vec4 v4ToColor = backgroundColour;\n"
            "    if (fDist < 0.0) {\n"
            "       v4ToColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
            "       fDist = -fDist;\n"
            "    }\n"
            "    float fBlendAmount = smoothstep(-1.0, 1.0, fDist - halfBorder);\n"
            "    gl_FragColor = mix(holeStrokeColour, v4ToColor, fBlendAmount);\n"
            "}\n";
    }
};

static MaskRenderProg     s_progNone;
static MaskRenderProgOval s_progOval;
static MaskRenderProgRect s_progRect;

class MaskRenderOp : public RenderOp {
public:

    MaskView* _maskView;

    MaskRenderOp(MaskView* maskView) : RenderOp(), _maskView(maskView) {
        
    }
    void validateShader() override {
        _shaderValid = true;
        if (_maskView->_holeShape == MaskView::HoleShape::None) {
            _prog = &s_progNone;
        } else if (_maskView->_holeShape == MaskView::HoleShape::Oval) {
            _prog = &s_progOval;
        } else if (_maskView->_holeShape == MaskView::HoleShape::Rect) {
            _prog = &s_progRect;
        }
        _blendMode = BLENDMODE_NORMAL;
    }
    
    void asQuads(QUAD *quad) override {
        rectToSurfaceQuad(_rect, quad);
        
        RECT rect = _rect;
        rect.origin.x -= _maskView->_holeRect.midX();
        rect.origin.y -= _maskView->_holeRect.midY();

        // Put the quad into the texture coords so the frag shader
        // can trivially know distance to hole center
        quad->tl.s = quad->bl.s = rect.left();
        quad->tl.t = quad->tr.t = rect.top();
        quad->tr.s = quad->br.s = rect.right();
        quad->bl.t = quad->br.t = rect.bottom();
    }

    void render(Window* window, Surface* surface) override {
        RenderOp::render(window, surface);
        MaskRenderProg* prog = (MaskRenderProg*)_prog;
        prog->_holeRadii.set({_maskView->_holeRect.size.width/2, _maskView->_holeRect.size.height/2});
        prog->_backgroundColour.set(_maskView->_backgroundColour);
        prog->_holeStrokeColour.set(_maskView->_holeStrokeColour);
        prog->_holeStrokeWidth.set(_maskView->_holeStrokeWidth);
        prog->_holeCornerRadius.set(_maskView->_holeCornerRadius);
    }

};


MaskView::MaskView() : _holeWidthMeasureSpec(MEASURESPEC::Abs(0)), _holeHeightMeasureSpec(MEASURESPEC::Abs(0)) {
    _holeShape = None;
    setBackground(new MaskRenderOp(this));
}
bool MaskView::applyStyleValue(const string& name, const StyleValue* value) {
    if (name == "background") {
        _backgroundColour = value->colorVal();
        _backgroundOp->invalidate();
        return true;
    }
    if (name == "hole-shape") {
        string shape = value->stringVal();
        if (shape == "none") {
            setHoleShape(None);
        } else if (shape == "oval") {
            setHoleShape(Oval);
        } else if (shape == "rect") {
            setHoleShape(Rect);
        } else {
            assert(0);
        }
        return true;
    }
    if (name == "hole-size") {
        processSizeStyleValue(value, &_holeWidthMeasureSpec, &_holeHeightMeasureSpec);
        return true;
    }
    if (name == "hole-align") {
        processAlignStyleValue(value, &_holeAlignSpecX, &_holeAlignSpecY);
        return true;
    }
    if (name == "hole-stroke-color") {
        _holeStrokeColour = value->colorVal();
        return true;
    }
    if (name == "hole-stroke-width") {
        _holeStrokeWidth = value->floatVal();
        return true;
    }
    if (name == "hole-corner-radius") {
        _holeCornerRadius = value->floatVal();
        return true;
    }
    return View::applyStyleValue(name, value);
}

void MaskView::setHoleShape(HoleShape shape) {
    _holeShape = shape;
    _backgroundOp->invalidate();
}


void MaskView::setHoleStrokeColour(COLOR colour) {
    if (_strokeColorAnim) {
        _strokeColorAnim->stop();
        _strokeColorAnim = NULL;
    }
    if (!_window) {
        _holeStrokeColour = colour;
    } else {
        COLOR animColourStart = _holeStrokeColour;
        _strokeColorAnim = Animation::start(this, 500, [=](float val) {
            _holeStrokeColour = COLOR::interpolate(animColourStart, colour, val);
            _backgroundOp->rebatchIfNecessary();
        });
    }
}

RECT MaskView::getHoleRect() const {
    return _holeRect;
}



void MaskView::layout(RECT constraint) {
    View::layout(constraint);
    RECT bounds = getOwnRect();
    _holeRect.size.width = _holeWidthMeasureSpec.calcConstraint(bounds.size.width, 0);
    _holeRect.size.height = _holeHeightMeasureSpec.calcConstraint(bounds.size.height, _holeRect.size.width);
    _holeRect.origin.x = _holeAlignSpecX.calc(_holeRect.size.width, 0, bounds.size.width);
    _holeRect.origin.y = _holeAlignSpecY.calc(_holeRect.size.height, 0, bounds.size.height);
}


