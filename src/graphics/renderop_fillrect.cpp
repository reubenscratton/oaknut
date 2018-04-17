//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class GLProgramSolidFill : public GLProgram {
public:
    
    virtual void load() {
        GLProgram::loadShaders(
           STANDARD_VERTEX_SHADER,
           "varying lowp vec4 v_colour;\n"
           "void main() {\n"
           "    gl_FragColor = v_colour;\n"
           "}\n"
        );
    }
};


static GLProgramSolidFill glprogSolidFill;



ColorRectFillRenderOp::ColorRectFillRenderOp(View* view) : RenderOp(view) {
    _prog = &glprogSolidFill;
    _blendMode = BLENDMODE_NONE;
}
ColorRectFillRenderOp::ColorRectFillRenderOp(View* view, const RECT& rect, COLOUR colour) : RenderOp(view) {
    _rect = rect;
    _prog = &glprogSolidFill;
    _alpha = 1.0f;
    _colour = colour;
    _blendMode = ((colour>>24) < 255) ? BLENDMODE_NORMAL : BLENDMODE_NONE;
}



