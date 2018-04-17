//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

SimpleBitmapProvider::SimpleBitmapProvider(Bitmap *bitmap) {
    _bitmap = bitmap;
}
Bitmap* SimpleBitmapProvider::getBitmap() const {
    return _bitmap;
}
void SimpleBitmapProvider::addCallback(Callback *callback) {
}
void SimpleBitmapProvider::removeCallback(Callback *callback) {
}


AsyncBitmapProvider::AsyncBitmapProvider(const char* assetPath) {
    // todo: maybe defer loading until first getBitmap()?
    ByteBuffer *data = app.loadAsset(assetPath);
    Bitmap::createFromData(data->data, (int) data->cb, [&](Bitmap *bitmap) {
        _bitmap = bitmap;
        for (auto it : _callbacks) {
            it->onBitmapChanged();
        }
    });
}
Bitmap* AsyncBitmapProvider::getBitmap() const {
    return _bitmap;
}

void AsyncBitmapProvider::addCallback(Callback *callback) {
    _callbacks.push_back(callback);
    if (_bitmap) {
        callback->onBitmapChanged();
    }
}
void AsyncBitmapProvider::removeCallback(Callback *callback) {
    _callbacks.remove(callback);
}


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
    _bitmapProvider = new SimpleBitmapProvider(bitmap);
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
    ByteBuffer* data = app.loadAsset(assetPath);
    Bitmap::createFromData(data->data, (int)data->cb, [&](Bitmap* bitmap) {
        _bitmapProvider = new SimpleBitmapProvider(bitmap);
        _rect = RECT_Make(0,0,bitmap->_width,bitmap->_height);
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
        && _bitmapProvider==((const TextureRenderOp*)op)->_bitmapProvider;
}

void TextureRenderOp::render(Window* window, Surface* surface) {
    if (_bitmapProvider) {
        Bitmap *bitmap = _bitmapProvider->getBitmap();
        if (bitmap) {
            RenderOp::render(window, surface);
            window->bindTexture(bitmap);
        }
    }
}

void TextureRenderOp::setColour(COLOUR colour) {
    if (colour != _colour) {
        _colour = colour;
        if (_colour) {
            _prog = &glprogTextureTint;
        } else {
            _prog = &glprogTexture;
        }
        rebatchIfNecessary();
    }
}


void TextureRenderOp::setBitmap(Bitmap *bitmap) {
    Bitmap* currentBitmap = _bitmapProvider ? _bitmapProvider->getBitmap() : NULL;
    if (bitmap != currentBitmap) {
        _bitmapProvider = new SimpleBitmapProvider(bitmap);
        onBitmapChanged();
    }
}

void TextureRenderOp::setBitmapProvider(BitmapProvider *bitmapProvider) {
    if (bitmapProvider != _bitmapProvider) {
        if (_bitmapProvider) {
            _bitmapProvider->removeCallback(this);
        }
        _bitmapProvider = bitmapProvider;
        if (bitmapProvider) {
            _bitmapProvider->addCallback(this);
        }
        rebatchIfNecessary();
    }
}

// BitmapProvider::Callback
void TextureRenderOp::onBitmapChanged() {
    Bitmap* bitmap = _bitmapProvider->getBitmap();
    setBlendMode(bitmap->hasAlpha() ? BLENDMODE_NORMAL : BLENDMODE_NONE);
    rebatchIfNecessary();
    if (_view) {
        _view->setNeedsFullRedraw(); // lazy
    }
}
