//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

CanvasView::CanvasView() {
    _canvas = oakCanvasCreate();
    _textureRenderOp = new TextureRenderOp(this);
    _textureRenderOp->setBlendMode(BLENDMODE_PREMULTIPLIED);
    _textureRenderOp->setTexRect(RECT_Make(0,1,1,-1));
    addRenderOp(_textureRenderOp);
}

void CanvasView::layout() {
    View::layout();
    RECT bounds = getBounds();
    _textureRenderOp->setRect(bounds);
    
    oakCanvasResize(_canvas, _frame.size.width, _frame.size.height);
    Bitmap* bitmap = oakCanvasGetBitmap(_canvas);
    bitmap->_texSampleMethod = GL_NEAREST;
    _textureRenderOp->setBitmap(bitmap);
}

void CanvasView::redraw() {
    oakCanvasClear(_canvas, 0);
}

