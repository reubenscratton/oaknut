//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "beebview.h"

DECLARE_DYNCREATE(BeebView);

BeebView::BeebView() {
}

void BeebView::setBeeb(Beeb* beeb) {
    _beeb = beeb;
    _beeb->displayCallbacks = this;
    _opaque = true;
    
    // Create pixel store
#ifdef USE_32BPP
# ifdef PLATFORM_APPLE
    int format = PIXELFORMAT_BGRA32;
    app.log("beebview format is PIXELFORMAT_BGRA32");
# else
    int format = PIXELFORMAT_RGBA32;
    app.log("beebview format is PIXELFORMAT_RGBA32");
# endif
#else
    int format = PIXELFORMAT_RGB565;
    app.log("beebview format is PIXELFORMAT_RGB565");
#endif
    _bitmap = Bitmap::create(SURFACE_WIDTH, SURFACE_HEIGHT, format);
    _bitmap->lock(&_bitmapData, true);
    beeb->setVideoBitmapParams((uint8_t*)_bitmapData.data, _bitmapData.stride);
    _renderOp = new TextureRenderOp(RECT::zero(), _bitmap, &_visibleArea, 0);
    addRenderOp(_renderOp);
}

void BeebView::attachToWindow(Window* window) {
    View::attachToWindow(window);
    drawFrame();
    _timer = Timer::start([=]() {
        if (!_paused) {
            _beeb->tick();
        }
    }, 20, true);
}

void BeebView::detachFromWindow() {
    View::detachFromWindow();
    _timer->stop();
}



void BeebView::layout(RECT constraint) {
    View::layout(constraint);
    _beeb->video.setViewSize(getWidth(), getHeight());
    _renderOp->setRect(getOwnRect());
}


void BeebView::setVisibleArea(int left, int top, int width, int height) {
    _visibleArea.origin.x = left / (float)_bitmap->_width;
    _visibleArea.origin.y = top / (float)_bitmap->_height;
    _visibleArea.size.width = width / (float)_bitmap->_width;
    _visibleArea.size.height = height / (float)_bitmap->_height;
    _renderOp->setTexRect(_visibleArea);
}

void BeebView::drawFrame() {
    _bitmap->unlock(&_bitmapData, true);
    _bitmap->lock(&_bitmapData, true);
    _beeb->setVideoBitmapParams((uint8_t*)_bitmapData.data, _bitmapData.stride);
    setNeedsFullRedraw();
}

