//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

/*
 
 What if we generated shaders based on unique feature combinations rather than hard-coding them?
 
 Features:
 - Texture unit
 - Alpha (last bit of rendering)
 - Fill color
 - Stroke
 - Rounded corners
 
 When a RenderOp changes its feature set
 
 */

class GLProgramRect : public GLProgram {
public:
    
    void load() override {
        GLProgram::loadShaders(
           STANDARD_VERTEX_SHADER,
           "varying lowp vec4 v_color;\n"
           "void main() {\n"
           "    gl_FragColor = v_color;\n"
           "}\n"
        );
    }
    
    virtual void configureForRenderOp(RectRenderOp* op) {
        // no-op
    }
    
    
};

class GLProgramRectAlpha : public GLProgramRect {
public:
    
    void load() override {
        GLProgram::loadShaders(
           STANDARD_VERTEX_SHADER,
           "varying lowp vec4 v_color;\n"
           "uniform mediump float alpha;\n"
           "void main() {\n"
           "    gl_FragColor = v_color;\n"
           "    gl_FragColor.a *= alpha;\n"
           "}\n"
           );
    }
    
    void configureForRenderOp(RectRenderOp* op) override {
        _alpha.set(op->_alpha);
    }
};

class GLProgramRoundRect : public GLProgramRect {
public:
    UniformColor _strokeColor;
    UniformFloat4 _u;
    
    void findVariables() override {
        GLProgram::findVariables();
        _strokeColor.position = check_gl(glGetUniformLocation, _program, "strokeColor");
        _u.position = check_gl(glGetUniformLocation, _program, "u");
    }
    void lazyLoadUniforms() override {
        GLProgram::lazyLoadUniforms();
        _strokeColor.use();
        _u.use();
    }
    void configureForRenderOp(RectRenderOp* op) override {
        _alpha.set(op->_alpha);
        _strokeColor.set(op->_strokeColor);
        _u.set(Vector4(op->_rect.size.width/2,op->_rect.size.height/2,0,op->_strokeWidth));
    }
};

class GLProgramRoundRectOne : public GLProgramRoundRect {
public:
    GLint _posRadius;
    
    void findVariables() override {
        GLProgramRoundRect::findVariables();
        _posRadius = check_gl(glGetUniformLocation, _program, "radius");
    }
    
    //void setRadius(float radius) {
    //    check_gl(glUniform1f, _posRadius, radius);
    //}

    void load() override {
        loadShaders(
            TEXTURE_VERTEX_SHADER
            ,
            "varying lowp vec4 v_color;\n"
            "uniform mediump float alpha;\n"
            "varying vec2 v_texcoord;\n" // not actually texture coords, this is x-dist and y-dist from quad centre
            "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
            "uniform mediump float radius;\n"
            "uniform lowp vec4 strokeColor;\n"
            
            "void main() {\n"
            "   vec2 b = u.xy - vec2(radius); \n"
            "    float dist = length(max(abs(v_texcoord)-b, 0.0)) - radius  - 0.5;\n"
            "    vec4 col = strokeColor;\n"
            "    col.a = mix(0.0, strokeColor.a, clamp(-dist, 0.0, 1.0));\n"   // outer edge blend
            "    col = mix(col, v_color, clamp(-(dist + u.w), 0.0, 1.0));\n"
            "    col.a *= alpha;\n"
            "    gl_FragColor = col;\n"
            "}\n"
            );
    }
    
    void configureForRenderOp(RectRenderOp* op) override {
        GLProgramRoundRect::configureForRenderOp(op);
        check_gl(glUniform1f, _posRadius, op->_radii[0]);
    }

};

class GLProgramRoundRectSymmetric : public GLProgramRoundRect {
public:
    GLint _posRadii;
    
    void findVariables() override {
        GLProgramRoundRect::findVariables();
        _posRadii = check_gl(glGetUniformLocation, _program, "radii");
    }
    
    void load() override {
        loadShaders(
            TEXTURE_VERTEX_SHADER
            ,
            "varying lowp vec4 v_color;\n"
            "uniform mediump float alpha;\n"
            "varying vec2 v_texcoord;\n" // not actually texture coords, this is x-dist and y-dist from quad centre
            "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
            "uniform mediump vec2 radii;\n"
            "uniform lowp vec4 strokeColor;\n"
            
            "float boxDist(vec2 p, vec2 size, vec2 r) {\n"
            // branchless selection of radius=r.x if on left side of quad or radius=r.y on right side
            "   float s=step(p.x,0.0);"
            "   float radius = s*r.x + (1.0-s)*r.y;\n"
            "   size -= vec2(radius);\n"
            "   vec2 d = abs(p) - size;\n"
            "   return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;\n"
            "}\n"
            
            "void main() {\n"
            "    float dist = boxDist(v_texcoord, u.xy, radii) - 0.5;\n"
            "    vec4 col = strokeColor;\n"
            "    col.a = mix(0.0, strokeColor.a, clamp(-dist, 0.0, 1.0));\n"   // outer edge blend
            "    col = mix(col, v_color, clamp(-(dist + u.w), 0.0, 1.0));\n"
            "    col.a *= alpha;\n"
            "    gl_FragColor = col;\n"
            "}\n"
            );
    }
    
    void configureForRenderOp(RectRenderOp* op) override {
        GLProgramRoundRect::configureForRenderOp(op);
        float r[2];
        r[0] = op->_radii[0];
        r[1] = op->_radii[3];
        check_gl(glUniform2fv, _posRadii, 1, r);
    }

};

static GLProgramRect glprogSolidFill;
static GLProgramRectAlpha glprogSolidFillAlpha;
static GLProgramRoundRectOne glprogRoundRect1;
static GLProgramRoundRectSymmetric glprogRoundRectSymmetric;


RectRenderOp::RectRenderOp(View* view) : RenderOp(view) {
    _prog = &glprogSolidFill;
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
Vector4 RectRenderOp::getCornerRadii() const {
    return _radii;
}
void RectRenderOp::setCornerRadius(float radius) {
    setCornerRadii({radius, radius, radius, radius});
}
void RectRenderOp::setCornerRadii(const Vector4& radii) {
    if (_radii != radii) {
        _radii = radii;
        invalidate();
    }
}

void RectRenderOp::validateShader() {
    _shaderValid = true;
    if (_alpha<1.0f) {
        _prog = &glprogSolidFillAlpha;
        _blendMode = BLENDMODE_NORMAL;
    } else {
        _prog = &glprogSolidFill;
        _blendMode = ((_color>>24) < 255) ? BLENDMODE_NORMAL : BLENDMODE_NONE;
    }
    if (_strokeWidth>0 && _strokeColor!=0) {
        
    }
    bool singleRadius = (_radii[0]==_radii[1] && _radii[2]==_radii[3] && _radii[0]==_radii[3]);
    if (singleRadius) {
        if (_radii[0] != 0.0f) {
            _prog = &glprogRoundRect1;
        }
    } else {
        if ((_radii[0]==_radii[2]) && (_radii[1]==_radii[3])) {
            _prog = &glprogRoundRectSymmetric;
        }
        else {
            assert(0); // unsupported combination
        }
    }
    _blendMode = BLENDMODE_NORMAL;
}




bool RectRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) {
        return false;
    }
    if (_prog == &glprogRoundRect1 || _prog == &glprogRoundRectSymmetric) {
        return _rect.size.width == ((const RectRenderOp*)op)->_rect.size.width
        && _rect.size.height == ((const RectRenderOp*)op)->_rect.size.height
        && _strokeColor==((const RectRenderOp*)op)->_strokeColor
        && _strokeWidth==((const RectRenderOp*)op)->_strokeWidth
        && _radii[0] ==((const RectRenderOp*)op)->_radii[0]
        && _radii[1] ==((const RectRenderOp*)op)->_radii[1];
    }
    return true;
}



void RectRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    if (_prog == &glprogRoundRect1 || _prog == &glprogRoundRectSymmetric) {
        // Put the quad size into the texture coords so the frag shader
        // can trivially know distance to quad center
        quad->tl.s = quad->bl.s = -_rect.size.width/2;
        quad->tl.t = quad->tr.t = -_rect.size.height/2;
        quad->tr.s = quad->br.s = _rect.size.width/2;
        quad->bl.t = quad->br.t = _rect.size.height/2;
    }
}

void RectRenderOp::render(Window* window, Surface* surface) {
    RenderOp::render(window, surface);
    GLProgramRoundRect* prog = (GLProgramRoundRect*)_prog;
    prog->configureForRenderOp(this);
}





