//
//  IdaaS
//
//  Copyright © 2018 PQ. All rights reserved.
//

#include "MaskView.h"

DECLARE_DYNCREATE(MaskView);


class MaskShader : public Shader {
public:
    
    MaskShader(Renderer* renderer) : Shader(renderer) {
        _u_backgroundColour = declareUniform("backgroundColour", Uniform::Float4, Uniform::Fragment);
        _u_holeStrokeColour = declareUniform("holeStrokeColour", Uniform::Float4, Uniform::Fragment);
        _u_holeFillColour = declareUniform("holeFillColour", Uniform::Float4, Uniform::Fragment);
        _u_holeStrokeWidth = declareUniform("holeStrokeWidth", Uniform::Float1, Uniform::Fragment);
    }
    int16_t _u_backgroundColour;
    int16_t _u_holeStrokeColour;
    int16_t _u_holeFillColour;
    int16_t _u_holeStrokeWidth;
    
#if RENDERER_GL
    string getVertexSource() override {
        return
        "attribute highp vec2 vPosition;\n"
        "uniform highp mat4 mvp;\n"
        "attribute vec2 texcoord;\n"
        "varying vec2 v_texcoord;\n"
        "void main() {\n"
        "  gl_Position = mvp * vec4(vPosition,0,1);\n"
        "  v_texcoord = texcoord;\n"
        "}\n";
    }
    string getFragmentSource() override {
        string s =
        "varying vec2 v_texcoord;\n"
        "uniform mediump vec2 holeRadii;\n"
        "uniform mediump float holeStrokeWidth;\n"
        "uniform lowp vec4 backgroundColour;\n"
        "uniform lowp vec4 holeStrokeColour;\n"
        "uniform lowp vec4 holeFillColour;\n"
        "void main() {\n";
        
        s += getMainProg();
        s += "}";
        return s;
    }
#elif RENDERER_METAL
    string getVertexSource() override {
        string s =
        "using namespace metal;\n"
        "struct VertexInput {\n"
        "   float2 position [[attribute(0)]];\n"
        "   float2 texcoord [[attribute(1)]];\n"
        "   uint color [[attribute(2)]];\n"
        "   float unused1;\n"
        "   float unused2;\n"
        "   float unused3;\n"
        "};\n"
        "struct VertexOutput {\n"
        "   float4 position [[position]];\n"
        "   float2 texcoord;\n"
        "};\n"
        "struct VertexUniforms {\n"
        "   float4x4 mvp;\n"
        "};\n"
        "vertex VertexOutput vertex_shader(uint vid [[vertex_id]],\n"
        "                                  constant VertexInput* v_in [[buffer(0)]],\n"
        "                                  constant VertexUniforms* uniforms [[buffer(1)]]) {\n"
        "   VertexOutput output;\n"
        "   output.position = uniforms->mvp * float4(v_in[vid].position,0,1);\n"
        "   output.texcoord = v_in[vid].texcoord;\n"
        "   return output;\n"
        "}\n";
        s+= "struct FragUniforms {\n";
        s+= getUniformFields(Uniform::Usage::Fragment);;
        s+= "};\n";
        
        s+= "fragment half4 frag_shader(VertexOutput in [[stage_in]]\n";
        s += ",constant FragUniforms* uniforms [[buffer(0)]]\n";
        s+= ") {\n";
        s+= " half4 c;\n";
        s+= getMainProg();
        s+= " return " SL_OUTPIXVAL ";\n";
        s += "}\n";
        return s;

    }
    string getFragmentSource() override {
        return "";
    }
#else
#error todo
#endif

    
    virtual string getMainProg() {
        return "c = " SL_UNIFORM(backgroundColour) ";\n";
    }

};


class MaskShaderOval : public MaskShader {
public:
    
    MaskShaderOval(Renderer* renderer) : MaskShader(renderer) {
        _u_holeRadii = declareUniform("holeRadii", Uniform::Float2, Uniform::Fragment);
    }
    
    // -inf to -strokeWidth/2 = fillColour
    // -strokeWidth/2 to strokeWidth/2 = strokeColour
    // strokeWidth/2 to +inf  = backgroundColour
    
    string getMainProg() override {
        return
        SL_FLOAT2 " w=" SL_FLOAT2 "(" SL_UNIFORM(holeStrokeWidth)"/2.0," SL_UNIFORM(holeStrokeWidth) "/2.0);\n"
        SL_FLOAT2 " ri=" SL_UNIFORM(holeRadii)" - w;\n"
        SL_FLOAT2 " ro=" SL_UNIFORM(holeRadii)" + w;\n"
        SL_FLOAT2 " p=" SL_ATTRIB(texcoord) ";\n"
        // Ellipse distance function adapted from 3D version at http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
        SL_FLOAT1 " d_o = (length(p/ro) - 1.0) * min(ro.x,ro.y);\n"
        SL_FLOAT4 " tmp;\n"
        "if (d_o >= 0.0) {"
            "tmp = mix(" SL_UNIFORM(holeStrokeColour) ", " SL_UNIFORM(backgroundColour) ", min(max(d_o,0.0),1.0));\n"
        "} else {\n"
            SL_FLOAT1 " d_i = (length(p/ri) - 1.0) * min(ri.x,ri.y);\n"
            "tmp = mix(" SL_FLOAT4 "(0.0), " SL_UNIFORM(holeStrokeColour) ", min(max(d_i,0.0),1.0));\n"
        "}\n"
        SL_OUTPIXVAL " = " SL_FLOAT4_TO_OUTPIX(tmp) ";\n";
    }
    
    int16_t _u_holeRadii;
};




class MaskShaderRect : public MaskShader {
public:

    MaskShaderRect(Renderer* renderer) : MaskShader(renderer) {
        _u_holeCornerRadius = declareUniform("holeCornerRadius", Uniform::Float1, Uniform::Fragment);
    }
    
    int16_t _u_holeCornerRadius;

    
    // Round-rect shader adapted from https://www.shadertoy.com/view/ltS3zW
    string getMainProg() override {
        return
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
        "    gl_FragColor = mix(holeStrokeColour, v4ToColor, fBlendAmount);\n";
    }
};


class MaskRenderOp : public RenderOp {
public:

    MaskView* _maskView;

    MaskRenderOp(MaskView* maskView) : RenderOp(), _maskView(maskView) {
        
    }
    void validateShader(Renderer* renderer) override {
        if (_maskView->_holeShape == MaskView::HoleShape::None) {
            _shader = new MaskShader(renderer);
        } else if (_maskView->_holeShape == MaskView::HoleShape::Oval) {
            _shader = new MaskShaderOval(renderer);
        } else if (_maskView->_holeShape == MaskView::HoleShape::Rect) {
            _shader = new MaskShaderRect(renderer);
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

    void prepareToRender(Renderer* renderer, class Surface* surface) override {
        RenderOp::prepareToRender(renderer, surface);
        MaskShader* shader = _shader.as<MaskShader>();
        renderer->setUniform(shader->_u_backgroundColour, _maskView->_backgroundColour);
        renderer->setUniform(shader->_u_holeStrokeColour, _maskView->_holeStrokeColour);
        renderer->setUniform(shader->_u_holeStrokeWidth, _maskView->_holeStrokeWidth);
        if (_maskView->_holeShape == MaskView::HoleShape::Oval) {
            renderer->setUniform(((MaskShaderOval*)shader)->_u_holeRadii, VECTOR2(_maskView->_holeRect.size.width/2, _maskView->_holeRect.size.height/2));
        }
        if (_maskView->_holeShape == MaskView::HoleShape::Rect) {
            renderer->setUniform(((MaskShaderRect*)shader)->_u_holeCornerRadius, _maskView->_holeCornerRadius);
        }
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


