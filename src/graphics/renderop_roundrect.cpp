//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


class GLProgramRoundRect : public GLProgram {
public:
    UniformColour _strokeColour;
    UniformFloat4 _u;
    
    virtual void findVariables();
    virtual void lazyLoadUniforms();
};

class GLProgramRoundRectOne : public GLProgramRoundRect {
public:
    GLint _posRadius;
    
    virtual void findVariables();
    virtual void load();
    virtual void setRadius(float radius);
};

class GLProgramRoundRectTwo : public GLProgramRoundRect {
public:
    GLint _posRadii;
    
    virtual void findVariables();
    virtual void load();
    virtual void setRadii(float radii[2]);
};

GLProgramRoundRectOne glprogRoundRect1;
GLProgramRoundRectTwo glprogRoundRect2;



RoundRectRenderOp::RoundRectRenderOp(View* view, COLOUR fillColour,  float strokeWidth, COLOUR strokeColour, float radius) : RenderOp(view) {
    this->_prog = &glprogRoundRect1;
    _radii[0] = radius;
    this->_colour = fillColour;
    this->_strokeColour = strokeColour;
    this->_strokeWidth = strokeWidth;
    this->_blendMode = BLENDMODE_NORMAL;
    //setRect(rect);
}

RoundRectRenderOp::RoundRectRenderOp(View* view, COLOUR fillColour, float strokeWidth, COLOUR strokeColour, float radii[2]) : RenderOp(view) {
    this->_prog = &glprogRoundRect2;
    this->_radii[0] = radii[0];
    this->_radii[1] = radii[1];
    this->_colour = fillColour;
    this->_strokeColour = strokeColour;
    this->_strokeWidth = strokeWidth;
    this->_blendMode = BLENDMODE_NORMAL;
    //setRect(rect);
}

void RoundRectRenderOp::setRect(const RECT& rect) {
    RenderOp::setRect(rect);
    
    // Put the quad size into the texture coords so the frag shader
    // can trivially know distance to quad center
    _rectTex.origin.x = -rect.size.width/2;
    _rectTex.size.width = rect.size.width;
    _rectTex.origin.y = -rect.size.height/2;
    _rectTex.size.height = rect.size.height;
}

bool RoundRectRenderOp::canMergeWith(const RenderOp* op) {
    return RenderOp::canMergeWith(op)
        && _rect.size.width == ((const RoundRectRenderOp*)op)->_rect.size.width
        && _rect.size.height == ((const RoundRectRenderOp*)op)->_rect.size.height
        && _colour==((const RoundRectRenderOp*)op)->_colour
        && _strokeColour==((const RoundRectRenderOp*)op)->_strokeColour
        && _strokeWidth==((const RoundRectRenderOp*)op)->_strokeWidth
        && _radii[0] ==((const RoundRectRenderOp*)op)->_radii[0]
        && _radii[1] ==((const RoundRectRenderOp*)op)->_radii[1];
}

void RoundRectRenderOp::setStrokeColour(COLOUR strokeColour) {
    if (_strokeColour != strokeColour) {
        _strokeColour = strokeColour;
        rebatchIfNecessary();
    }
}

void RoundRectRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    quad->tl.s = quad->bl.s = _rectTex.left();
    quad->tl.t = quad->tr.t = _rectTex.top();
    quad->tr.s = quad->br.s = _rectTex.right();
    quad->bl.t = quad->br.t = _rectTex.bottom();
}

void RoundRectRenderOp::render(Window* window, Surface* surface) {
    RenderOp::render(window, surface);
    GLProgramRoundRect* prog = (GLProgramRoundRect*)_prog;
    //prog->setColour(_colour);
    if (prog == &glprogRoundRect1) { // ugh
        glprogRoundRect1.setRadius(_radii[0]);
    } else {
        glprogRoundRect2.setRadii(_radii);
    }
    prog->_strokeColour.set(_strokeColour);
    prog->_u.set(Vector4(_rect.size.width/2,_rect.size.height/2,0,_strokeWidth));
}




void GLProgramRoundRect::findVariables() {
    GLProgram::findVariables();
    _strokeColour.position = check_gl(glGetUniformLocation, _program, "strokeColour");
    _u.position = check_gl(glGetUniformLocation, _program, "u");
}

void GLProgramRoundRect::lazyLoadUniforms() {
    GLProgram::lazyLoadUniforms();
    _strokeColour.use();
    _u.use();
}

void GLProgramRoundRectOne::findVariables() {
    GLProgramRoundRect::findVariables();
    _posRadius = check_gl(glGetUniformLocation, _program, "radius");
}


void GLProgramRoundRectOne::setRadius(float radius) {
    check_gl(glUniform1f, _posRadius, radius);
}


void GLProgramRoundRectOne::load() {
    loadShaders(
        TEXTURE_VERTEX_SHADER
        ,
        "varying lowp vec4 v_colour;\n"
        "uniform mediump float alpha;\n"
        "varying vec2 v_texcoord;\n" // not actually texture coords, this is x-dist and y-dist from quad centre
        "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
        "uniform mediump float radius;\n"
        "uniform lowp vec4 strokeColour;\n"
        
        "void main() {\n"
        "    vec2 b = u.xy - vec2(radius); \n"
        "    float dist = length(max(abs(v_texcoord)-b, 0.0)) - radius; \n"
        "    lowp vec4 col = v_colour;\n"
        "    //col.a *= (1.0-dist);\n"
        "    if (u.w > 0.0) {\n"
        "        float alpha1 = clamp(dist + u.w, 0.0, 1.0);\n"
        "        float alpha2 = clamp(dist, 0.0, 1.0);\n"
        "        col = mix(col, strokeColour, alpha1 - alpha2);\n"
        "    }\n"
        "    col.a *= alpha;\n"
        "    gl_FragColor = col;\n"
        "}\n"
    );
}

void GLProgramRoundRectTwo::findVariables() {
    GLProgramRoundRect::findVariables();
    _posRadii = check_gl(glGetUniformLocation, _program, "radii");
}

void GLProgramRoundRectTwo::setRadii(float radii[2]) {
    check_gl(glUniform2fv, _posRadii, 1, radii);
}

void GLProgramRoundRectTwo::load() {
    loadShaders(
        TEXTURE_VERTEX_SHADER
        ,
        "varying lowp vec4 v_colour;\n"
        "uniform mediump float alpha;\n"
        "varying vec2 v_texcoord;\n" // not actually texture coords, this is x-dist and y-dist from quad centre
        "uniform vec4 u;\n" // xy = quad half size, w = strokeWidth
        "uniform mediump vec2 radii;\n"
        "uniform lowp vec4 strokeColour;\n"

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
        "    vec4 col = strokeColour;\n"
        "    col.a = mix(0.0, strokeColour.a, clamp(-dist, 0.0, 1.0));\n"   // outer edge blend
        "    col = mix(col, v_colour, clamp(-(dist + u.w), 0.0, 1.0));\n"
        "    col.a *= alpha;\n"
        "    gl_FragColor = col;\n"
        "}\n"
    );
}

/*
 
 
 "float boxDist(vec2 p, vec2 size, float radius) {\n"
 "   size -= vec2(radius);\n" // nb: can be done on cpu
 "   vec2 d = abs(p) - size;\n"
 "   return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;\n"
 "}\n"
 
 "void main() {\n"
 "    float dist = boxDist(v_texcoord, u.xy, radius) - 0.5;\n"
 "    vec4 col = strokeColour;\n"
 "    col.a = mix(0.0, strokeColour.a, clamp(-dist, 0.0, 1.0));\n"   // outer edge blend
 "    col = mix(col, v_colour, clamp(-(dist + u.w), 0.0, 1.0));\n"
 "    col.a *= alpha;\n"
 "    gl_FragColor = col;\n"
 "}\n"
*/




