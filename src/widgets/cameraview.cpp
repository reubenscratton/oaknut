//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(CameraView);
/*
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
*/
void CameraView::attachToWindow(Window* window) {
    View::attachToWindow(window);
    show();
}

void CameraView::show() {
    if (!_window) {
        return;
    }
    /*if (_renderOp) {
        return;
    }*/
    if (!_window->hasPermission(PermissionCamera)) {
        return;
    }
}

void CameraView::handleNewCameraFrame(Bitmap* bitmap) {
    if (_backgroundOp) {
        setBackground(NULL);
    }
    setBitmap(bitmap);
}


RECT CameraView::getDisplayedFrameRect() const {
    return _renderOp ? _renderOp->_rect : RECT::zero();
}

