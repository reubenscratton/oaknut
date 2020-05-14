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
    if (0 == url.compare(_url)) {
        return;
    }
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


void ImageView::setTexture(Texture* texture) {
    _renderOp->setTexture(texture);
    _rectTex = RECT(0,0,1,1);
    invalidateIntrinsicSize();
    _loaded = (texture!=nullptr);
}

void ImageView::setBitmap(Bitmap *bitmap) {
    _renderOp->setBitmap(bitmap);
    _rectTex = RECT(0,0,1,1);
    invalidateIntrinsicSize();
    _loaded = (bitmap!=nullptr);
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
    if (_loaded || !_surface) {
        return;
    }
    if (!(_url.length() || _assetPath.length())) {
        return;
    }

	_startLoadTime = app->currentMillis();
    if (_assetPath.length() > 0) {
        auto hashVal = _assetPath.hash();
        _imageLoadTask = Task::enqueue({
            {Task::IO, [=] (variant&) -> variant {
                return app->loadAssetSync(_assetPath);
            }},
            {Task::Background, [=] (variant& loadResult) -> variant  {
                if (loadResult.isError()) {
                    return loadResult;
                }
                bytearray& data = loadResult.bytearrayRef();
                assert(data.size());
                Bitmap* bitmap = Bitmap::createFromData(data);
                return variant(bitmap);
            }},
            {Task::MainThread, [=] (variant& loadResult) -> variant  {
                if (loadResult.isError()) {
                    // TODO: show error image or something
                } else {
                    Bitmap* bitmap = loadResult.ptr<Bitmap>();
                    if (hashVal == _assetPath.hash()) {
                        setBitmap(bitmap);
                    }
                }
                _imageLoadTask = NULL;
                return variant();
            }}
        });
    } else if (_url.length() > 0) {
        assert(!_request);
        _request = URLRequest::get(_url, this, URL_FLAG_BITMAP);
        _request->handle([=](const URLResponse* res, bool isFromCache) {
            setBitmap(res->decoded.bitmap);
        });
    }
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



