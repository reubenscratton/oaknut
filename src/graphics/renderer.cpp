//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Texture::Texture(Bitmap* bitmap) : _bitmap(bitmap) {
    assert(!bitmap->_texture);
    bitmap->_texture = this;
}
Texture::~Texture() {
    unload();
}

Shader::Shader(ShaderFeatures features) : _features(features) {
    
}


Renderer::Renderer(Window* window) : _window(window), _quadBuffer(sizeof(QUAD), 256) {
}

void Renderer::reset() {
    _doneInit = false;
    //delete _quadBuffer; todo: fix this leak
    for (auto tex : _textures) {
        tex->unload();
    }
    _textures.clear();
    for (auto shader : _shaders) {
        shader.second->unload();
    }
    _shaders.clear();
}


void Renderer::releaseTexture(Texture* tex) {
    tex->unload();
    _textures.erase(tex->_it);
    delete tex;
}

void Renderer::bindBitmap(Bitmap* bitmap) {
    assert(bitmap);
    if (!bitmap->_texture) {
        bitmap->_texture = createTexture(bitmap);
        bitmap->_texture->_it = _textures.insert(_textures.end(), bitmap->_texture);
    }
    if (_currentTexture != bitmap->_texture) {
        _currentTexture = bitmap->_texture;
        _currentTexture->bind();
    }
}



ItemPool::Alloc* Renderer::allocQuads(int num, ItemPool::Alloc* existingAlloc) {
    return _quadBuffer.alloc(num, existingAlloc);
}


void Renderer::invalidateQuads(ItemPool::Alloc* alloc) {
    uint8_t* lo = alloc->addr();
    uint8_t* hi = lo + alloc->cb();
    if (!_quadBufferDirtyLo) {
        _quadBufferDirtyLo = lo;
        _quadBufferDirtyHi = hi;
    } else {
        _quadBufferDirtyLo = MIN(_quadBufferDirtyLo, lo);
        _quadBufferDirtyHi = MAX(_quadBufferDirtyHi, hi);
    }
}


