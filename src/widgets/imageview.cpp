//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ImageView);



ImageView::ImageView() {
    _contentMode = ActualSize;
    _scrollHorz._disabled = true;
    _scrollVert._disabled = true;
    applyStyle("ImageView");
    _renderOp = new TextureRenderOp();
    addRenderOp(_renderOp);
}

bool ImageView::applySingleStyle(const string& name, const style& value) {
    if (name=="image") {
        // TODO: leverage drawable code
        setImage(value.stringVal());
        return true;
    }
    if (name=="contentMode") {
        string mode = value.stringVal();
        if (mode == "actualSize") setContentMode(ActualSize);
        else if (mode == "aspectFit") setContentMode(AspectFit);
        else if (mode == "aspectFill") setContentMode(AspectFill);
        else assert(0);
        return true;
    }
    return View::applySingleStyle(name, value);
}

void ImageView::setImage(const string& assetOrUrl) {
    if (0 == assetOrUrl.compare(_url)) {
        return;
    }
    cancelLoad();
	_url = assetOrUrl;
	_renderOp->setBitmap((Bitmap*)NULL);
    loadImage();
}

void ImageView::cancelLoad() {
    if (_task) {
        _task->cancel();
        _task = nullptr;
    }
    _loaded = false;
}


void ImageView::setTexture(Texture* texture) {
    _renderOp->setTexture(texture);
    invalidateIntrinsicSize();
    _loaded = (texture!=nullptr);
}

void ImageView::setBitmap(Bitmap *bitmap) {
    _renderOp->setBitmap(bitmap);
    invalidateIntrinsicSize();
    _loaded = (bitmap!=nullptr);
}

void ImageView::setImageNode(AtlasNode* node) {
    _atlasNode = node;
    _renderOp->setBitmap(node->page->_bitmap);
    _loaded = true;
    // todo: if sizespec==wrap then invalidate layout
    setNeedsFullRedraw();
}

void ImageView::attachToSurface() {
    View::attachToSurface();
    loadImage();
}

void ImageView::detachFromSurface() {
	View::detachFromSurface();
    cancelLoad();
    _renderOp->setBitmap((Bitmap*)NULL);
}

void ImageView::loadImage() {
    if (_loaded || !_surface || _task || !_url.length()) {
        return;
    }
    auto timeReqStart = app->currentMillis();
    auto hashVal = _url.hash();

    _task = app->loadBitmap(_url, [=](Bitmap* bitmap) {
        if (hashVal == _url.hash()) {
            auto elapsed = app->currentMillis() - timeReqStart;
            if (elapsed >= _window->_fadeInThreshold) {
                Animation::start(this, _window->_fadeInDuration, [=](float val) {
                    _renderOp->setAlpha(val);
                });
            }
            setBitmap(bitmap);
        }
        _task = nullptr;
    });
    
}

ImageView::ContentMode ImageView::getContentMode() const {
    return _contentMode;
}
void ImageView::setContentMode(ContentMode contentMode) {
    if (contentMode != _contentMode) {
        _contentMode = contentMode;
        invalidateIntrinsicSize();
    }
}

void ImageView::updateIntrinsicSize(SIZE constrainingSize) {
    if (!_renderOp->_bitmap && !_renderOp->_texture) {
        _intrinsicSize = {0,0};
        return;
    }
    SIZEI imageSize;
    if (_renderOp->_bitmap) {
        imageSize.width = _renderOp->_bitmap->_width;
        imageSize.height = _renderOp->_bitmap->_height;
    } else {
        imageSize = _renderOp->_texture->_size;
    }
    if (_contentMode == ActualSize) {
        _intrinsicSize.width = imageSize.width;
        _intrinsicSize.height = imageSize.height;
    } else {
        float scaleWidth = constrainingSize.width / imageSize.width;
        float scaleHeight = constrainingSize.height / imageSize.height;
        float scale;
        if (scaleHeight >= scaleWidth) {
            scale = (_contentMode == AspectFit) ? scaleWidth : scaleHeight;
        } else {
            scale = (_contentMode == AspectFit) ? scaleHeight : scaleWidth;
        }
        _intrinsicSize.width = ceilf(imageSize.width * scale);
        _intrinsicSize.height = ceilf(imageSize.height * scale);
    }
    
    // Updating content size must trigger a renderop update
    _updateRenderOpsNeeded = true;
}


void ImageView::updateRenderOps() {
    RECT rect = {_padding.left,_padding.top,_intrinsicSize.width,_intrinsicSize.height};
    RECT bounds = getOwnRect();
    _padding.applyToRect(bounds);
    float dx = bounds.size.width - rect.size.width;
    float dy = bounds.size.height - rect.size.height;
    if (_gravity.horz == GRAVITY_CENTER) {
        rect.origin.x += dx/2;
    } else if (_gravity.horz == GRAVITY_RIGHT) {
        rect.origin.x += dx;
    }
    if (_gravity.vert == GRAVITY_CENTER) {
        rect.origin.y += dy/2;
    } else if (_gravity.vert == GRAVITY_BOTTOM) {
        rect.origin.y += dy;
    }
    _renderOp->setRect(rect);
}

RECT ImageView::getImageRect() const {
    assert(_intrinsicSizeValid);
    return _renderOp->_rect;
}

void ImageView::layout(RECT constraint) {
    View::layout(constraint);
    _updateRenderOpsNeeded = true;
}



void ImageView::onEffectiveTintColorChanged() {
    _renderOp->setColor(_effectiveTintColor);
}



