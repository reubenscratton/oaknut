//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


AtlasNode::AtlasNode(AtlasPage* page) {
    this->page = page;
    left = right = NULL;
    filled = false;
}

void AtlasPage::importAsset(const string& assetPath, std::function<void(AtlasNode*)> callback) {
    ObjPtr<ByteBuffer> data = app.loadAsset(assetPath.data());
    assert(data);
    Bitmap::createFromData(data->data, (int)data->cb, [=](Bitmap* bitmap) {
        assert(bitmap);
        if (bitmap->_format != _bitmap->_format) {
            bitmap = bitmap->convertToFormat(_bitmap->_format);
        }
        AtlasNode* node = reserve(bitmap->_width, bitmap->_height);
        assert(node);
        POINT origin = node->rect.origin;
        PIXELDATA pixelDataDest, pixelDataSrc;
        _bitmap->lock(&pixelDataDest, true);
        bitmap->lock(&pixelDataSrc, false);
        uint8_t* destData = (uint8_t*)_bitmap->pixelAddress(&pixelDataDest, origin.x, origin.y);
        uint8_t* srcData = (uint8_t*)pixelDataSrc.data;
        for (int y=0 ; y<bitmap->_height ; y++) {
            memcpy(destData, srcData, pixelDataSrc.stride);
            destData += pixelDataDest.stride;
            srcData += pixelDataSrc.stride;
        }
        bitmap->unlock(&pixelDataSrc, false);
        _bitmap->unlock(&pixelDataDest, true);
        callback(node);
    });
}


//
// Adapted from: https://github.com/mackstann/binpack/blob/master/livedemo.html
//
bool rectFitsIn(RECT* outer, RECT* inner) {
    return outer->size.width >= inner->size.width && outer->size.height >= inner->size.height;
}
bool rectSameSizeAs(RECT* outer, RECT* inner) {
    return outer->size.width == inner->size.width && outer->size.height == inner->size.height;
}
AtlasNode* AtlasNode::insertRect(RECT* r) {
    if(left != NULL) {
        AtlasNode* node = left->insertRect(r);
        if (node == NULL) {
            node = right->insertRect(r);
        }
        return node;
    }
    if(filled) {
        return NULL;
    }
    
    if(!rectFitsIn(&rect, r)) {
        return NULL;
    }
    
    if(rectSameSizeAs(&rect, r)) {
        filled = true;
        return this;
    }
    
    left = new AtlasNode(page);
    right = new AtlasNode(page);
    
    int width_diff = rect.size.width - r->size.width;
    int height_diff = rect.size.height - r->size.height;
    
    //var me = this.rect;
    
    if(width_diff > height_diff) {
        // split literally into left and right, putting the rect on the left.
        left->rect = RECT_Make(RECT_left(rect), RECT_top(rect), r->size.width, rect.size.height);
        right->rect = RECT_Make(RECT_left(rect) + r->size.width, RECT_top(rect), rect.size.width - r->size.width, rect.size.height);
    }
    else {
        // split into top and bottom, putting rect on top.
        left->rect = RECT_Make(RECT_left(rect), RECT_top(rect), rect.size.width, r->size.height);
        right->rect = RECT_Make(RECT_left(rect), RECT_top(rect) + r->size.height, rect.size.width, rect.size.height - r->size.height);
    }
    
    return left->insertRect(r);
}


AtlasPage::AtlasPage(int width, int height, int bitmapFormat) {
    _bitmap = new Bitmap(width, height, bitmapFormat);
    _bitmap->_texSampleMethod = GL_NEAREST;
    start_node = new AtlasNode(this);
    start_node->rect = RECT_Make(0, 0, width, height);
}


AtlasNode* AtlasPage::reserve(int w, int h) {
    RECT r = RECT_Make(0, 0, w, h);
    AtlasNode* node = start_node->insertRect(&r);
    if (node == NULL) {
        return NULL;
    }
    return node;
}



Atlas::Atlas(int width, int height, int bitmapFormat) {
    this->width = width;
    this->height = height;
    this->bitmapFormat = bitmapFormat;
}

AtlasPage* Atlas::lastPage() {
	if (pages.size() == 0) {
		return NULL;
	}
	return pages.at(pages.size()-1);
}

AtlasNode* Atlas::reserve(int w, int h, int padding) {
	if (padding > 0) {
		w = padding+ w +padding;
		h = padding+ h +padding;
	}
    AtlasNode* node = NULL;
    AtlasPage* currentPage = lastPage();
    if (currentPage != NULL) {
        node = currentPage->reserve(w, h); // can fail if not enough space on page
    }
    if (node == NULL) {
        currentPage = new AtlasPage(width, height, bitmapFormat);
        pages.push_back(currentPage);
        node = currentPage->reserve(w,h);
    }
    if (node != NULL) {
		if (padding > 0) {
			RECT_inset(node->rect, padding, padding);
		}
        node->page = currentPage;
        nodes.push_back(node);
    }
    return node;
}

