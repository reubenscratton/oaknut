//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(CanvasView);


CanvasView::CanvasView() {
    applyStyle("CanvasView");
    _canvas = Canvas::create();
    _textureRenderOp = new TextureRenderOp();
    _textureRenderOp->setTexRect(RECT(0,1,1,-1));
    addRenderOp(_textureRenderOp);
}

void CanvasView::layout(RECT constraint) {
    View::layout(constraint);
    RECT bounds = getOwnRect();
    _textureRenderOp->setRect(bounds);
    
    _canvas->resize(_rect.size.width, _rect.size.height);
    Bitmap* bitmap = _canvas->getBitmap();
    bitmap->_texSampleMethod = GL_NEAREST;
    _textureRenderOp->setBitmap(bitmap);
}

void CanvasView::redraw() {
    _canvas->clear(0);
}

