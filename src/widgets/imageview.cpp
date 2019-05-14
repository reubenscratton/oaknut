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
    applyStyle("ImageView");
    _renderOp = new TextureRenderOp();
    addRenderOp(_renderOp);
}

bool ImageView::applySingleStyle(const string& name, const style& value) {
    if (name=="image") {
        if (handleStatemapDeclaration(name, value)) {
            return true;
        }
        // TODO: leverage drawable code
        setImageAsset(value.stringVal());
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

void ImageView::setImageUrl(const string& url) {
    cancelLoad();
	_url = url;
    _assetPath = "";
	_renderOp->setBitmap((Bitmap*)NULL);
    loadImage();
}

void ImageView::setImageAsset(const string& assetPath) {
    cancelLoad();
    _assetPath = assetPath;
    loadImage();
}

void ImageView::cancelLoad() {
    if (_imageLoadTask) {
        _imageLoadTask->cancel();
        _imageLoadTask = NULL;
    }
    // Cancel any extant HTTP request.
    if (_request) {
        _request->cancel();
        _request = NULL;
    }
    _loaded = false;
}


void ImageView::setBitmap(Bitmap *bitmap) {
    _renderOp->setBitmap(bitmap);
    _rectTex = RECT(0,0,1,1);
    invalidateContentSize();
    _loaded = true;
}

void ImageView::setImageNode(AtlasNode* node) {
    _atlasNode = node;
    _renderOp->setBitmap(node->page->_bitmap);
    _loaded = true;
    _rectTex = node->rect;
    _rectTex.origin.x /= node->page->_bitmap->_width;
    _rectTex.origin.y /= node->page->_bitmap->_height;
    _rectTex.size.width /= node->page->_bitmap->_width;
    _rectTex.size.height /= node->page->_bitmap->_height;
    // todo: if sizespec==wrap then invalidate layout
    setNeedsFullRedraw();
}

void ImageView::attachToWindow(Window* window) {
	View::attachToWindow(window);
    loadImage();
}

void ImageView::detachFromWindow() {
	View::detachFromWindow();
    cancelLoad();
}

void ImageView::loadImage() {
    if (_loaded || !_window) {
        return;
    }
    if (!(_url.length() || _assetPath.length())) {
        return;
    }

	_startLoadTime = app.currentMillis();
    if (_assetPath.length() > 0) {
        auto hashVal = _assetPath.hash();
        ByteBuffer* data = app.loadAsset(_assetPath.data());
        assert(data);
        _imageLoadTask = Bitmap::createFromData(data->data, (int) data->cb, [=](Bitmap *bitmap) {
            if (hashVal == _assetPath.hash()) {
                setBitmap(bitmap);
            }
            _imageLoadTask = NULL;
        });
    } else if (_url.length() > 0) {
        _request = URLRequest::get(_url, URL_FLAG_BITMAP);
        _request->handleBitmap([=](URLRequest* req, Bitmap* bitmap) {
            setBitmap(bitmap);
        });
    }
}

ImageView::ContentMode ImageView::getContentMode() const {
    return _contentMode;
}
void ImageView::setContentMode(ContentMode contentMode) {
    if (contentMode != _contentMode) {
        _contentMode = contentMode;
        invalidateContentSize();
    }
}

void ImageView::updateContentSize(SIZE constrainingSize) {
    if (!_renderOp->_bitmap) {
        _contentSize = {0,0};
    } else {
        float imageWidth = (float)_renderOp->_bitmap->_width;
        float imageHeight = (float)_renderOp->_bitmap->_height;
        if (_contentMode == ActualSize) {
            _contentSize.width = imageWidth;
            _contentSize.height = imageHeight;
        } else {
            float scaleWidth = constrainingSize.width / imageWidth;
            float scaleHeight = constrainingSize.height / imageHeight;
            float scale;
            if (scaleHeight >= scaleWidth) {
                scale = (_contentMode == AspectFit) ? scaleWidth : scaleHeight;
            } else {
                scale = (_contentMode == AspectFit) ? scaleHeight : scaleWidth;
            }
            _contentSize.width = imageWidth * scale;
            _contentSize.height = imageHeight * scale;
        }
    }
    
    // Updating content size must trigger a renderop update
    _updateRenderOpsNeeded = true;
}


void ImageView::updateRenderOps() {
    RECT rect = {_padding.left,_padding.top,_contentSize.width,_contentSize.height};
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
    assert(_contentSizeValid);
    return _renderOp->_rect;
}

void ImageView::layout(RECT constraint) {
    View::layout(constraint);
    _updateRenderOpsNeeded = true;
}



void ImageView::onEffectiveTintColorChanged() {
    _renderOp->setColor(_effectiveTintColor);
}



