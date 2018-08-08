//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class GLProgramTextGlyph : public GLProgram {
public:
    virtual void load() {
        loadShaders(
            TEXTURE_VERTEX_SHADER,
            "varying vec2 v_texcoord;\n"
            "uniform sampler2D texture;\n"
            "uniform mediump float alpha;\n"
            "varying lowp vec4 v_color;\n"
            "void main() {\n"
            "   gl_FragColor.rgb = v_color.rgb;\n"
            "   gl_FragColor.a = texture2D(texture, v_texcoord).a * alpha;\n"
            "}\n"
        );
    }
};


static GLProgramTextGlyph glprogTextGlyph;

TextRenderOp::TextRenderOp(View* view, const TEXTRENDERPARAMS* textRenderParams) : RenderOp(view) {
    _alpha = 1.0f;
    _textRenderParams = *textRenderParams;
    this->_blendMode = BLENDMODE_NORMAL;
}

void TextRenderOp::validateShader() {
    _prog = &glprogTextGlyph;
}

void TextRenderOp::addGlyph(Glyph* glyph, const RECT& rect) {
    RECT rectTex = glyph->atlasNode->rect;
    Bitmap* bitmap =  glyph->atlasNode->page->_bitmap;
    rectTex.origin.x /= bitmap->_width;
    rectTex.origin.y /= bitmap->_height;
    rectTex.size.width /= bitmap->_width;
    rectTex.size.height /= bitmap->_height;
    _rects.push_back(rect);
    _rectsTex.push_back(rectTex);
    if (_rects.size()==1) {
        _rect = rect;
    } else {
        _rect = RECT_union(_rect, rect);
    }
}

void TextRenderOp::reset() {
    _rects.clear();
    _rectsTex.clear();
}

bool TextRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) return false;
    const TextRenderOp* textOp = (const TextRenderOp*)op;
    return _textRenderParams == textOp->_textRenderParams;
}

int TextRenderOp::numQuads() {
    return (int)_rects.size();
}

void TextRenderOp::render(Window* window, Surface* surface) {
    RenderOp::render(window, surface);
    window->bindTexture(_textRenderParams.atlasPage->_bitmap);
}

void TextRenderOp::asQuads(QUAD *quad) {
    _color = _textRenderParams.forecolor; // todo: why is this here??
    for (int i=0 ; i<_rects.size() ; i++) {
        rectToSurfaceQuad(_rects.at(i), quad);
        RECT& rectTex = _rectsTex.at(i);
        quad->tl.s = quad->bl.s = rectTex.left();
        quad->tl.t = quad->tr.t = rectTex.top();
        quad->tr.s = quad->br.s = rectTex.right();
        quad->bl.t = quad->br.t = rectTex.bottom();
        quad++;
    }
}



/*
 
 POINT origin = bounds.origin;
 float excess = bounds.size.width - _size.width;
 if (gravityHorz == GRAVITY_RIGHT) {
 origin.x += excess;
 } else if (gravityHorz == GRAVITY_CENTER) {
 origin.x += excess / 2;
 }
 
 excess = bounds.size.height - _size.height;
 if (gravityVert == GRAVITY_BOTTOM) {
 origin.y += excess;
 } else if (gravityVert == GRAVITY_CENTER) {
 origin.y += excess / 2;
 }
 
 
 void TextRenderOp::setGlyphs(int numGlyphs, Glyph** glyphs, RECT* rects,  POINT origin) {
 _origin = origin;
 _rects.reserve(numGlyphs);
 _rectsTex.reserve(numGlyphs);
 for (int i=0 ; i<numGlyphs ; i++) {
 RECT rect = rects[i];
 rect.origin.x += origin.x;
 rect.origin.y += origin.y;
 RECT rectTex = glyphs[i]->atlasNode->rect;
 Texture* tex =  glyphs[i]->atlasNode->page->_texture;
 rectTex.origin.x /= tex->_bitmap->_width;
 rectTex.origin.y /= tex->_bitmap->_height;
 rectTex.size.width /= tex->_bitmap->_width;
 rectTex.size.height /= tex->_bitmap->_height;
 //_texture = tex;
 _rects.push_back(rect);
 _rectsTex.push_back(rectTex);
 if (i==0) {
 _rect = rect;
 } else {
 _rect = RECT_union(_rect, rect);
 }
 }
 }
 // Render rect must be pixel-aligned since we use GL_NEAREST
 POINT origin;
 origin.x = floorf(_origin.x);
 origin.y = floorf(_origin.y);

void TextRenderOp::setOrigin(const POINT origin) {
    POINT delta = POINT_Make(origin.x-_origin.x, origin.y-_origin.y);
    if (delta.x!=0.f || delta.y!=0.f) {
        _rect.origin.x += delta.x;
        _rect.origin.y += delta.y;
        _origin = origin;
        for (int i=0 ; i<_rects.size() ; i++) {
            RECT& rect = _rects[i];
            rect.origin.x += delta.x;
            rect.origin.y += delta.y;
        }
        updateBoundingRect();
        invalidateBatchGeometry();
    }
}
*/

