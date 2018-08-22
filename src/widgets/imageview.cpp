//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ImageView);

ImageView::ImageView() {
    applyStyle("ImageView");
    _renderOp = new TextureRenderOp(this);
    addRenderOp(_renderOp);
}

bool ImageView::applyStyleValue(const string& name, const StyleValue* value) {
    if (name=="image") {
        // TODO: leverage drawable code
        ByteBuffer* data = app.loadAsset(value->stringVal().data());
        Bitmap::createFromData(data->data, (int)data->cb, [=](Bitmap* bitmap) {
            setBitmap(bitmap);
        });
        return true;
    }
    return View::applyStyleValue(name, value);
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
    // todo: if wrap_content then invalidate layout
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
        Bitmap::createFromData(data->data, (int) data->cb, [=](Bitmap *bitmap) {
            if (hashVal == _assetPath.hash()) {
                setBitmap(bitmap);
            }
        });
    } else if (_url.length() > 0) {
        _request = URLRequest::get(_url, URL_FLAG_BITMAP);
        _request->handleBitmap([=](int httpStatus, Bitmap* bitmap) {
            setBitmap(bitmap);
        });
    }
}

void ImageView::layout() {
    View::layout();
    RECT bounds = getOwnRect();
    _padding.applyToRect(bounds);
    _renderOp->setRect(bounds);
}



void ImageView::onEffectiveTintColorChanged() {
    _renderOp->setColor(_effectiveTintColor);
}



