//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

class GLProgramTexture : public GLProgram {
public:
    virtual void load();
};

void GLProgramTexture::load()  {
    loadShaders(TEXTURE_VERTEX_SHADER,
        "varying vec2 v_texcoord;\n"
        "uniform sampler2D texture;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(texture, v_texcoord);\n"
        "}\n"
    );
}

class GLProgramTextureTint : public GLProgram {
public:
    
    virtual void load() {
        loadShaders(
            TEXTURE_VERTEX_SHADER,
            "varying vec2 v_texcoord;\n"
            "varying lowp vec4 v_colour;\n"
            "uniform sampler2D texture;\n"
            "void main() {\n"
            "    gl_FragColor = vec4(v_colour.rgb, texture2D(texture, v_texcoord).a);\n"
            "}\n"
        );
    }
};

class GLProgramTextureAlpha : public GLProgramTexture {
public:
    
    virtual void load() {
        loadShaders(
            TEXTURE_VERTEX_SHADER,
            "varying vec2 v_texcoord;\n"
            "uniform sampler2D texture;\n"
            "uniform mediump float alpha;\n"
            "void main() {\n"
            "    gl_FragColor = texture2D(texture, v_texcoord);\n"
            "    gl_FragColor.a *= alpha;\n"
            "}\n"
        );
    }
};

class GLProgramTextureTintAlpha : public GLProgramTextureTint {
public:
    
    virtual void load() {
        loadShaders(
            TEXTURE_VERTEX_SHADER,
            "varying vec2 v_texcoord;\n"
            "varying lowp vec4 v_colour;\n"
            "uniform sampler2D texture;\n"
            "uniform mediump float alpha;\n"
            "void main() {\n"
            "    gl_FragColor = vec4(v_colour.rgb, texture2D(texture, v_texcoord).a);\n"
            "    gl_FragColor.a *= alpha;\n"
            "}\n"
        );
    }
};


GLProgramTexture glprogTexture;
static GLProgramTextureAlpha glprogTextureAlpha;
static GLProgramTextureTint glprogTextureTint;
static GLProgramTextureTintAlpha glprogTextureTintAlpha;


TextureRenderOp::TextureRenderOp(View* view) : RenderOp(view) {
    _alpha = 1.0f;
    _prog = &glprogTexture;
    _rectTex = RECT_Make(0,0,1,1);
}
TextureRenderOp::TextureRenderOp(View* view, const RECT& rect, Bitmap* bitmap, const RECT* rectTex, COLOUR tintColour) : RenderOp(view) {
    _bitmap = bitmap;
    _alpha = 1.0f;
    _colour = tintColour;
    if (_colour) {
        _prog = &glprogTextureTint;
    } else {
        _prog = &glprogTexture;
    }
    _rect = rect;
    if (rectTex) {
        _rectTex = *rectTex;
    } else {
        _rectTex = RECT_Make(0,1,1,-1);
    }
}

/**
 * Constructor for tinted .png icons
 */
TextureRenderOp::TextureRenderOp(View* view, const char* assetPath, int tintColour) : TextureRenderOp(view) {
    _prog = &glprogTextureTintAlpha;
    _alpha = 1.0f;
    _colour = tintColour;
    Data* data = oakLoadAsset(assetPath);
    oakBitmapCreateFromData(data->data, (int)data->cb, [&](Bitmap* bitmap) {
        _bitmap = bitmap;
        _rect = RECT_Make(0,0,_bitmap->_width, _bitmap->_height);
    });
    _rectTex = RECT_Make(0,0,1,1);
    setBlendMode(BLENDMODE_NORMAL);
}

void TextureRenderOp::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        _prog = (alpha<1.0f) ? &glprogTextureTintAlpha : &glprogTextureTint;
        rebatchIfNecessary();
    }
}
void TextureRenderOp::setTexRect(const RECT& texRect) {
    _rectTex = texRect;
    invalidateBatchGeometry();
}

void TextureRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    quad->tl.s = quad->bl.s = _rectTex.left();
    quad->tl.t = quad->tr.t = _rectTex.top();
    quad->tr.s = quad->br.s = _rectTex.right();
    quad->bl.t = quad->br.t = _rectTex.bottom();
}

bool TextureRenderOp::canMergeWith(const RenderOp* op) {
    return RenderOp::canMergeWith(op)
        && _bitmap._obj==((const TextureRenderOp*)op)->_bitmap._obj;
}

void TextureRenderOp::render(Canvas* canvas, Surface* surface) {
    RenderOp::render(canvas, surface);
    canvas->bindTexture(_bitmap);
}


