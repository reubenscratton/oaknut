//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

DECLARE_DYNCREATE(ImageView);

ImageView::ImageView() {
}

void ImageView::setImageUrl(const string& url) {
	
	// Cancel any extant HTTP request. Note that the request will only really get cancelled
	// if this is the only UIImageView that wants the current _imageURL.
	if (_url.length()) {
		URLRequest::unrequest(_url, this);
	}
	
	// Update URL and reset image
	_url = url;
    /*if (_tex) {
        _tex->_bitmap = NULL;
    }*/
	_bitmap = NULL;
	_errorDisplay = false;
	
	// If we're visible then try to display the new image
	if (url.length() && _window) {
		loadImage();
	}
}

void ImageView::setImageBitmap(Bitmap *bitmap) {
    _bitmap = bitmap;
    _rectTex = RECT_Make(0,0,1,1);
    _updateRenderOpsNeeded = true;
    invalidateContentSize();
}

void ImageView::setImageNode(AtlasNode* node) {
    _atlasNode = node;
    _bitmap = node->page->_bitmap;
    _rectTex = node->rect;
    _rectTex.origin.x /= _bitmap->_width;
    _rectTex.origin.y /= _bitmap->_height;
    _rectTex.size.width /= _bitmap->_width;
    _rectTex.size.height /= _bitmap->_height;
    _updateRenderOpsNeeded = true;
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
	_startLoadTime = oakCurrentMillis();
	URLRequest::request(_url, this, URL_FLAG_BITMAP);
}



void ImageView::updateRenderOps() {
    if (_renderOp) {
        removeRenderOp(_renderOp);
        _renderOp = NULL;
    }
    if (_window && (_bitmap || _atlasNode)) {
        RECT bounds = getBounds();
        _padding.applyToRect(bounds);
        _renderOp = new TextureRenderOp(this, bounds, _bitmap, &_rectTex, _effectiveTintColour);
    }
    if (_renderOp) {
        _renderOp->setBlendMode(_bitmap->hasAlpha() ? BLENDMODE_NORMAL : BLENDMODE_NONE);
        
        if (_useFadeEffect) {
            long loadTime = oakCurrentMillis() - _startLoadTime;
            if (loadTime >= 500) {
                _alpha = 0.1f;
                animateAlpha(1.0, 500);
            }
        }
        addRenderOp(_renderOp);

    }
}

void ImageView::onEffectiveTintColourChanged() {
    if (_renderOp) {
        _renderOp->setColour(_effectiveTintColour);
    }
}

void ImageView::onUrlRequestLoad(URLData* data) {
    if (data) {
        if (data->_type != URLDataTypeBitmap) {
            oakLog("Warning: Unexpected urldata type");
            return;
        }
        setImageBitmap(data->_value.bitmap);
    }
}


