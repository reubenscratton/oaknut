//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(CameraView);

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

