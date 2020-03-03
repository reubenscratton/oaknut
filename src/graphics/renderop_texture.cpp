//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

void BitmapProvider::dispatch(Bitmap *bitmap) {
    _bitmap = bitmap;
    for (auto& it : _ops) {
        it->setBitmap(bitmap);
    }
    _ops.clear();
}
void BitmapProvider::dispatch(AtlasNode* node) {
    _node = node;
    for (auto& it : _ops) {
        it->setBitmap(node);
    }
    _ops.clear();
}


TextureRenderOp::TextureRenderOp() : RenderOp() {
    _alpha = 1.0f;
    _rectTex = RECT(0,0,1,1);
}
TextureRenderOp::TextureRenderOp(const RECT& rect, Bitmap* bitmap, const RECT* rectTex, COLOR tintColor) : TextureRenderOp() {
    _bitmap = bitmap;
    _color = tintColor;
    _rect = rect;
    if (rectTex) {
        _rectTex = *rectTex;
    }
}

/**
 * Constructor for tinted .png icons
 */
TextureRenderOp::TextureRenderOp(Bitmap* bitmap, int tintColor) : TextureRenderOp() {
    setBitmap(bitmap);
    _color = tintColor;
    setBlendMode(BLENDMODE_NORMAL);
}

void TextureRenderOp::validateShader(RenderTask* r) {
    Texture* tex = _texture;
    if (!tex) {
        if (_bitmap) {
            if (!_bitmap->_texture) {
                r->_renderer->createTextureForBitmap(_bitmap);
            }
            tex = _bitmap->_texture;
        }
    }
    if (tex) {
        Shader::Features features;
        features.textures[0] = tex->_type;
        features.alpha = (_alpha<1.0f);
        features.tint = (_color!=0);
        _shader = r->_renderer->getStandardShader(features);
    }
}
void TextureRenderOp::setTexRect(const RECT& texRect) {
    _rectTex = texRect;
    invalidateBatchGeometry();
}

void TextureRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    Texture* tex = _texture ? _texture : _bitmap->_texture;
    if (tex->_denormalizedCoords) {
        quad->tl.s = quad->bl.s = _rectTex.left() * _bitmap->_width;
        quad->tl.t = quad->tr.t = _rectTex.top()  * _bitmap->_height;
        quad->tr.s = quad->br.s = _rectTex.right() * _bitmap->_width;
        quad->bl.t = quad->br.t = _rectTex.bottom() * _bitmap->_height;
    } else {
        quad->tl.s = quad->bl.s = _rectTex.left();
        quad->tl.t = quad->tr.t = _rectTex.top();
        quad->tr.s = quad->br.s = _rectTex.right();
        quad->bl.t = quad->br.t = _rectTex.bottom();
    }
}

bool TextureRenderOp::canMergeWith(const RenderOp* op) {
    return RenderOp::canMergeWith(op)
        && ((_bitmap && _bitmap==((const TextureRenderOp*)op)->_bitmap)
        || (_texture && _texture==((const TextureRenderOp*)op)->_texture));
}

void TextureRenderOp::prepareToRender(RenderTask* r, Surface* surface) {
    RenderOp::prepareToRender(r, surface);
    if (_alpha < 1) {
        r->setUniform(_shader->_u_alpha, _alpha);
    }
    if (_texture) {
        r->setCurrentTexture(_texture);
    } else {
        r->bindBitmap(_bitmap);
    }
}

void TextureRenderOp::setTexture(Texture* texture) {
    if (_bitmapProvider) {
        _bitmapProvider = NULL;
    }
    if (_bitmap) {
        _bitmap = nullptr;
    }
    _texture = texture;
    invalidate();
    if (_view) {
        _view->setNeedsFullRedraw(); // lazy
    }
}

void TextureRenderOp::setBitmap(Bitmap *bitmap) {
    if (_bitmapProvider) {
        //_bitmapProvider->_ops.erase(std::find( _bitmapProvider->_ops.begin(),  _bitmapProvider->_ops.end(), this));
        _bitmapProvider = NULL;
    }
    if (bitmap != _bitmap) {
        _bitmap = bitmap;
        if (bitmap) {
            if (bitmap->hasAlpha()) {
                if (bitmap->hasPremultipliedAlpha()) {
                    setBlendMode(BLENDMODE_PREMULTIPLIED);
                 } else {
                    setBlendMode(BLENDMODE_NORMAL);
                 }
            } else {
                setBlendMode(BLENDMODE_NONE);
            }
        }
        invalidate();
        if (_view) {
            _view->setNeedsFullRedraw(); // lazy
        }
    }
}

void TextureRenderOp::setBitmap(AtlasNode *node) {
    if (!node) {
        setBitmap((Bitmap*)NULL);
        return;
    }
    setBitmap(node->page->_bitmap);
    _rectTex = node->rect;
    _rectTex.origin.x /= node->page->_bitmap->_width;
    _rectTex.origin.y /= node->page->_bitmap->_height;
    _rectTex.size.width /= node->page->_bitmap->_width;
    _rectTex.size.height /= node->page->_bitmap->_height;
}

void TextureRenderOp::setBitmap(BitmapProvider *bitmapProvider) {
    if (_bitmapProvider) {
        //_bitmapProvider->_ops.erase(std::find( _bitmapProvider->_ops.begin(),  _bitmapProvider->_ops.end(), this));
    }
    if (bitmapProvider->_node) {
        setBitmap(bitmapProvider->_node);
    }
    else if (bitmapProvider->_bitmap) {
        setBitmap(bitmapProvider->_bitmap);
    }
    else {
        _bitmapProvider = bitmapProvider;
        bitmapProvider->_ops.push_back(this);
    }
}
