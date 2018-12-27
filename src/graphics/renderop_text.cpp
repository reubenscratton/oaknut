//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>




TextRenderOp::TextRenderOp(const TEXTRENDERPARAMS* textRenderParams) : RenderOp() {
    _alpha = 1.0f;
    _textRenderParams = *textRenderParams;
    this->_blendMode = BLENDMODE_NORMAL;
}

void TextRenderOp::validateShader(Renderer* renderer) {
    Bitmap* bitmap = _textRenderParams.atlasPage->_bitmap;
    if (bitmap) {
        if (!bitmap->_texture) {
            renderer->createTexture(bitmap);
            assert(bitmap->_texture);
        }
        ShaderFeatures features;
        features.sampler0 = bitmap->_texture->getSampler();
        features.tint = 1;
        features.alpha = _alpha<1.0f;
        _shader = renderer->getShader(features);
        _shaderValid = true;
    }
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
        _rect.unionWith(rect);
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

void TextRenderOp::prepareToRender(Renderer* renderer, Surface* surface) {
    RenderOp::prepareToRender(renderer, surface);
    renderer->bindBitmap(_textRenderParams.atlasPage->_bitmap);
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

