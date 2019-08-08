//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>




TextRenderOp::TextRenderOp(AtlasPage* atlasPage) : RenderOp() {
    _alpha = 1.0f;
    _atlasPage = atlasPage;
    _blendMode = BLENDMODE_NORMAL;
    _mergeType = MERGETYPE_TEXT;
}

void TextRenderOp::validateShader(Renderer* renderer) {
    Bitmap* bitmap = _atlasPage->_bitmap;
    if (bitmap) {
        if (!bitmap->_texture) {
            renderer->createTextureForBitmap(bitmap);
        }
        Shader::Features features;        
        features.textures[0] = bitmap->_texture->_type;
        features.tint = 1;
        features.alpha = _alpha<1.0f;
        _shader = renderer->getStandardShader(features);
    }
}

void TextRenderOp::addGlyph(Glyph* glyph, const RECT& rect, COLOR forecolor) {
    RECT rectTex = glyph->_atlasNode->rect;
    Bitmap* bitmap =  glyph->_atlasNode->page->_bitmap;
    rectTex.origin.x /= bitmap->_width;
    rectTex.origin.y /= bitmap->_height;
    rectTex.size.width /= bitmap->_width;
    rectTex.size.height /= bitmap->_height;
    _rects.push_back(rect);
    _rectsTex.push_back(rectTex);
    if (_rects.size()==1) {
        _rect = rect;
    } else {
        _rect.unionWith(rect);
    }
    _forecolors.push_back(forecolor);
}

void TextRenderOp::reset() {
    _rects.clear();
    _rectsTex.clear();
}

bool TextRenderOp::canMergeWith(const RenderOp* op) {
    if (!RenderOp::canMergeWith(op)) return false;
    const TextRenderOp* textOp = (const TextRenderOp*)op;
    return _atlasPage == textOp->_atlasPage;
}

int TextRenderOp::numQuads() {
    return (int)_rects.size();
}

void TextRenderOp::prepareToRender(Renderer* renderer, Surface* surface) {
    RenderOp::prepareToRender(renderer, surface);
    renderer->bindBitmap(_atlasPage->_bitmap);
}

void TextRenderOp::asQuads(QUAD *quad) {
    for (int i=0 ; i<_rects.size() ; i++) {
        rectToSurfaceQuad(_rects.at(i), quad);
        RECT& rectTex = _rectsTex.at(i);
        quad->tl.s = quad->bl.s = rectTex.left();
        quad->tl.t = quad->tr.t = rectTex.top();
        quad->tr.s = quad->br.s = rectTex.right();
        quad->bl.t = quad->br.t = rectTex.bottom();
        quad->tl.color =
        quad->tr.color =
        quad->bl.color = 
        quad->br.color = _forecolors[i];
        quad++;
    }
}

