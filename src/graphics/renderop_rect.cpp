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
    Uniform<COLOR> _strokeColor;
    Uniform<Vector4> _u;
    
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
    void unload() override {
        GLProgramRect::unload();
        _strokeColor.dirty = true;
        _u.dirty = true;
    }
};

class GLProgramRoundRectOne : public GLProgramRoundRect {
public:
    Uniform<float> _radius;
    
    void findVariables() override {
        GLProgramRoundRect::findVariables();
        _radius.position = check_gl(glGetUniformLocation, _program, "radius");
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
        _radius.set(op->_radii[0]);
    }
    void lazyLoadUniforms() override {
        GLProgramRoundRect::lazyLoadUniforms();
        _radius.use();
    }

    void unload() override {
        GLProgramRoundRect::unload();
        _radius.dirty = true;
    }

};

class GLProgramRoundRectSymmetric : public GLProgramRoundRect {
public:
    Uniform<Vector2> _radii;

    void findVariables() override {
        GLProgramRoundRect::findVariables();
        _radii.position = check_gl(glGetUniformLocation, _program, "radii");
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
        _radii.set({op->_radii[0], op->_radii[3]});
    }
    void lazyLoadUniforms() override {
        GLProgramRoundRect::lazyLoadUniforms();
        _radii.use();
    }
    void unload() override {
        GLProgramRoundRect::unload();
        _radii.dirty = true;
    }

};

class GLProgramRoundRectComplex : public GLProgramRoundRect {
public:
    Uniform<Vector4> _radii;
    
    void findVariables() override {
        GLProgramRoundRect::findVariables();
        _radii.position = check_gl(glGetUniformLocation, _program, "radii");
    }
    
    void load() override {
        
        loadShaders(
                    TEXTURE_VERTEX_SHADER
                    ,
                    "varying lowp vec4 v_color;\n"
                    "uniform mediump float alpha;\n"
                    "varying vec2 v_texcoord;\n" // not actually texture coords, this is x-dist and y-dist from quad centre
                    "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
                    "uniform mediump vec4 radii;\n"
                    "uniform lowp vec4 strokeColor;\n"
                    

                    "void main() {\n"

                    // Branchless selection of radius
                    // NB: step(val,edge) ==  ((val < edge) ? 0.0 : 1.0)
                    // NB: radii.x=top left, radii.y=top right, radii.z=bot left, radii.w = bot right
                    "   float l=step(v_texcoord.x,0.0);"
                    "   float r=1.0-l;"
                    "   float t=step(v_texcoord.y,0.0);"
                    "   float b=1.0-t;"
                    "   float radius = t*l*radii.x + t*r*radii.y + b*l*radii.z + b*r*radii.w;\n"
                    
                    // Round rect distance function
                    "    vec2 foo = max(abs(v_texcoord) - (u.xy - vec2(radius)), vec2(0.0)); \n"
                    "    float dist = 1.0 - clamp(length(foo) - radius, 0.0, 1.0);"

                    // Blend stroke & fill (TODO: stroke)
                    "    vec4 col = v_color;\n"
                    "    col.a *= dist;\n"
                    "    col.a *= alpha;\n"
                    "    gl_FragColor = col;\n"
                    "}\n"
                    );
    }
    
    void configureForRenderOp(RectRenderOp* op) override {
        GLProgramRoundRect::configureForRenderOp(op);
        _radii.set(op->_radii);
    }
    void lazyLoadUniforms() override {
        GLProgramRoundRect::lazyLoadUniforms();
        _radii.use();
    }
    void unload() override {
        GLProgramRoundRect::unload();
        _radii.dirty = true;
    }
    
};

static GLProgramRect glprogSolidFill;
static GLProgramRectAlpha glprogSolidFillAlpha;
static GLProgramRoundRectOne glprogRoundRect1;
static GLProgramRoundRectSymmetric glprogRoundRectSymmetric;
static GLProgramRoundRectComplex glprogRoundRectComplex;


RectRenderOp::RectRenderOp() : RenderOp() {
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
            _prog = &glprogRoundRectComplex;
        }
    }
    _blendMode = BLENDMODE_NORMAL;
}




bool RectRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) {
        return false;
    }
    // TODO: this is utter bollocks, fix when implementing shader cache
    if (_prog == &glprogRoundRect1 || _prog == &glprogRoundRectSymmetric || _prog == &glprogRoundRectComplex) {
        return _rect.size.width == ((const RectRenderOp*)op)->_rect.size.width
        && _rect.size.height == ((const RectRenderOp*)op)->_rect.size.height
        && _strokeColor==((const RectRenderOp*)op)->_strokeColor
        && _strokeWidth==((const RectRenderOp*)op)->_strokeWidth
        && _radii[0] ==((const RectRenderOp*)op)->_radii[0]
        && _radii[1] ==((const RectRenderOp*)op)->_radii[1]
        && _radii[2] ==((const RectRenderOp*)op)->_radii[2]
        && _radii[3] ==((const RectRenderOp*)op)->_radii[3];
    }
    return true;
}



void RectRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    // TODO: this is utter bollocks, fix when implementing shader cache
    if (_prog == &glprogRoundRect1 || _prog == &glprogRoundRectSymmetric || _prog == &glprogRoundRectComplex) {
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





