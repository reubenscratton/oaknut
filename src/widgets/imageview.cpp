//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ImageView);

ImageView::ImageView() {
    _renderOp = new TextureRenderOp(this);
    addRenderOp(_renderOp);
}

bool ImageView::applyStyleValue(const string& name, StyleValue* value) {
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
	
	// Cancel any extant HTTP request. Note that the request will only really get cancelled
	// if this is the only UIImageView that wants the current _imageURL.
	if (_url.length()) {
		URLRequest::unrequest(_url, this);
	}
	
	// Update URL and reset image
	_url = url;
	_renderOp->setBitmap((Bitmap*)NULL);
	_errorDisplay = false;
	
	// If we're visible then try to display the new image
	if (url.length() && _window) {
		loadImage();
	}
}

void ImageView::setImageAsset(const string& assetPath) {
    _errorDisplay = false;
    ByteBuffer* data = app.loadAsset(assetPath.data());
    assert(data);
    _url = assetPath;
    Bitmap::createFromData(data->data, (int) data->cb, [=](Bitmap *bitmap) {
        if (_url.compare(assetPath) == 0) {
            setBitmap(bitmap);
        }
    });
}


void ImageView::setBitmap(Bitmap *bitmap) {
    _renderOp->setBitmap(bitmap);
    _rectTex = RECT(0,0,1,1);
    invalidateContentSize();
}

void ImageView::setImageNode(AtlasNode* node) {
    _atlasNode = node;
    _renderOp->setBitmap(node->page->_bitmap);
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
    
	if (_url.length() && _window) {
		loadImage();
	}
}

void ImageView::detachFromWindow() {
	View::detachFromWindow();
	if (_url.length()) {
		URLRequest::unrequest(_url, this);
	}
}

void ImageView::loadImage() {
	_startLoadTime = app.currentMillis();
	URLRequest::request(_url, this, URL_FLAG_BITMAP);
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

void ImageView::onUrlRequestLoad(URLData* data) {
    if (data) {
        if (data->_type != URLDataTypeBitmap) {
            app.warn("Unexpected urldata type");
            return;
        }
        setBitmap(data->_value.bitmap);
    }
}


