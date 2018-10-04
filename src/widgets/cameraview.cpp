//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if OAKNUT_WANT_CAMERA

#include <oaknut.h>

DECLARE_DYNCREATE(CameraView);

class GLProgramTextureDesaturated : public GLProgram {
public:
    virtual void load();
};

// TODO: Desaturation should be an optional shader attribute once we have shader cache done
void GLProgramTextureDesaturated::load()  {
    loadShaders(TEXTURE_VERTEX_SHADER,
        "varying vec2 v_texcoord;\n"
        "uniform sampler2D texture;\n"
        "void main() {\n"
        "    vec4 color = texture2D(texture, v_texcoord);\n"
        "    float lum = color.r * 0.299 + color.g * 0.587 + color.b * 0.144;\n"
        "    gl_FragColor = mix(color, vec4(lum,lum,lum, 1.0), 0.0);\n"
        "}\n"
        );
}

static GLProgramTextureDesaturated s_prog;

void CameraView::attachToWindow(Window* window) {
    View::attachToWindow(window);
    show();
}

void CameraView::show() {
    if (!_window) {
        return;
    }
    if (_renderOp) {
        return;
    }
    if (!_window->hasPermission(PermissionCamera)) {
        return;
    }
    _renderOp = new TextureRenderOp();
    addRenderOp(_renderOp);
    _renderOp->_prog = &s_prog;
}

void CameraView::handleNewCameraFrame(Bitmap* bitmap) {
    if (!_renderOp) {
        return;
    }
    if (_backgroundOp) {
        setBackground(NULL);
    }
    _renderOp->setBitmap(bitmap);
    float scaleWidth = _rect.size.width / (float)bitmap->_width;
    float scaleHeight = _rect.size.height / (float)bitmap->_height;
    RECT bounds = getOwnRect();
    RECT rect = bounds;
    if (scaleHeight >= scaleWidth) {
        rect.scale(scaleHeight/scaleWidth, 1.0);
    } else {
        rect.scale(1.0, scaleWidth/scaleHeight);
    }
    
    // Center the texture rect on the bounds rect
    rect.origin.x = bounds.midX() - rect.size.width/2;
    rect.origin.y = bounds.midY() - rect.size.height/2;
    
    _renderOp->setRect(rect);
}



void CameraView::detachFromWindow() {
    View::detachFromWindow();
    if (_renderOp) {
        removeRenderOp(_renderOp);
        _renderOp = NULL;
    }
}

RECT CameraView::getDisplayedFrameRect() const {
    return _renderOp ? _renderOp->_rect : RECT_Zero;
}


#endif

