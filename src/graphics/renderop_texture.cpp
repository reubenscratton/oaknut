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

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif

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

class GLProgramTexture_OES_EGL : public GLProgram {
    void load() override {
        loadShaders(TEXTURE_VERTEX_SHADER,
            "varying vec2 v_texcoord;\n"
            "uniform samplerExternalOES sampler;\n"
            "void main() {\n"
            "    gl_FragColor = texture2D(sampler, v_texcoord);\n"
            "}\n",
                    "#extension GL_OES_EGL_image_external : require\n"
        );
    }
};

class GLProgramTextureTint : public GLProgram {
public:
    
    virtual void load() {
        loadShaders(
            TEXTURE_VERTEX_SHADER,
            "varying vec2 v_texcoord;\n"
            "varying lowp vec4 v_color;\n"
            "uniform sampler2D texture;\n"
            "void main() {\n"
            "    gl_FragColor = vec4(v_color.rgb, texture2D(texture, v_texcoord).a);\n"
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
            "varying lowp vec4 v_color;\n"
            "uniform sampler2D texture;\n"
            "uniform mediump float alpha;\n"
            "void main() {\n"
            "    gl_FragColor = vec4(v_color.rgb, texture2D(texture, v_texcoord).a);\n"
            "    gl_FragColor.a *= alpha;\n"
            "}\n"
        );
    }
};


GLProgramTexture glprogTexture;
static GLProgramTexture_OES_EGL glprogTexture_OES_EGL;
static GLProgramTextureAlpha glprogTextureAlpha;
static GLProgramTextureTint glprogTextureTint;
static GLProgramTextureTintAlpha glprogTextureTintAlpha;


TextureRenderOp::TextureRenderOp() : RenderOp() {
    _alpha = 1.0f;
    _rectTex = RECT(0,0,1,1);
}
TextureRenderOp::TextureRenderOp(const RECT& rect, Bitmap* bitmap, const RECT* rectTex, COLOR tintColor) : RenderOp() {
    _bitmap = bitmap;
    _alpha = 1.0f;
    _color = tintColor;
    _rect = rect;
    if (rectTex) {
        _rectTex = *rectTex;
    } else {
        _rectTex = RECT(0,1,1,-1);
    }
}

/**
 * Constructor for tinted .png icons
 */
TextureRenderOp::TextureRenderOp(const char* assetPath, int tintColor) : TextureRenderOp() {
    _alpha = 1.0f;
    _color = tintColor;
    ByteBuffer* data = app.loadAsset(assetPath);
    Bitmap::createFromData(data->data, (int)data->cb, [&](Bitmap* bitmap) {
        _rectTex = RECT(0,0,1,1);
        setBitmap(bitmap);
    });
    setBlendMode(BLENDMODE_NORMAL);
}

void TextureRenderOp::validateShader() {
    if (_bitmap && _bitmap->_texTarget == GL_TEXTURE_EXTERNAL_OES) {
        _prog = &glprogTexture_OES_EGL;
        assert(_alpha == 1.0f); // don't yet support variants
        assert(_color == 0U); // don't yet support variants
    } else {
        if (_alpha<1.0f) {
            if (_color) {
                _prog = &glprogTextureTintAlpha;
            } else {
                _prog = &glprogTextureAlpha;
            }
        } else {
            if (_color) {
                _prog = &glprogTextureTint;
            } else {
                _prog = &glprogTexture;
            }
        }
    }
    _shaderValid = true;
}
void TextureRenderOp::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        invalidate();
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
        && _bitmap==((const TextureRenderOp*)op)->_bitmap;
}

void TextureRenderOp::render(Window* window, Surface* surface) {
    if (_bitmap) {
        if (_prog == &glprogTexture_OES_EGL) {
            app.log("glprogTexture_OES_EGL render!");
        }
        RenderOp::render(window, surface);
        window->bindTexture(_bitmap);
    }
}

void TextureRenderOp::setColor(COLOR color) {
    if (color != _color) {
        _color = color;
        invalidate();
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
