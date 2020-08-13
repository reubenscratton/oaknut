//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(Button);
DECLARE_DYNCREATE(ToolbarButton);


Button::Button() {
    _pressable = true;
    applyStyle("Button");
}

bool Button::applySingleStyle(const string &name, const style &value) {
    if (name == "icon-tint") {
        _iconTint = value.boolVal();
        if (_imageOp) {
            _imageOp->setColor(_defaultColor);
        }
        return true;
    }
    if (name == "image-spacing") {
        _imageSpacing = value.floatVal();
        return true;
    }
    return Label::applySingleStyle(name, value);
}

void Button::setTextColor(COLOR color) {
    Label::setTextColor(color);
    if (_imageOp && _iconTint) {
        _imageOp->setColor(color);
    }
}

void Button::setImage(const string& assetOrUrl) {
    if (_imageTask) {
        _imageTask->cancel();
    }
    _imageTask = app->loadBitmap(assetOrUrl, [=](Bitmap* bitmap) {
        setImage(bitmap);
        _imageTask = nullptr;
    });
}
void Button::setImage(Bitmap* bitmap) {
    if (!_imageOp) {
        _imageOp = new TextureRenderOp(bitmap, _textLayout.color());
        addRenderOp(_imageOp);
    } else {
        _imageOp->setBitmap(bitmap);
    }
    _imageOp->setRect({0,0, static_cast<float>(bitmap->_width), static_cast<float>(bitmap->_height)});
    invalidateIntrinsicSize();
}

void Button::updateIntrinsicSize(SIZE constrainingSize) {
    Label::updateIntrinsicSize(constrainingSize);
    
    if (_imageOp) {
        _intrinsicSize.width += _imageOp->_rect.size.width + _imageSpacing;
        _intrinsicSize.height = MAX(_intrinsicSize.height, _imageOp->_rect.size.height);
    }
}
void Button::layout(RECT constraint) {
    if (_imageOp) {
        float imageWidthPlusGap = _imageOp->_rect.size.width + _imageSpacing;
        constraint.origin.x += imageWidthPlusGap;
        constraint.size.width -= imageWidthPlusGap;
    }
    Label::layout(constraint);
    if (_imageOp) {
        // Center image vertically
        RECT imageRect = _imageOp->_rect;
        imageRect.origin.y = (_rect.size.height - imageRect.size.height) / 2;
        _imageOp->setRect(imageRect);
    }
}

RECT Button::getTextRectForLayout() {
    RECT rect = Label::getTextRectForLayout();
    if (_imageOp) {
        
        // Position image horizontally
        RECT imageRect = _imageOp->_rect;
        imageRect.origin.x = rect.origin.x;
        imageRect.origin.y = (_rect.size.height - imageRect.size.height) / 2;
        _imageOp->setRect(imageRect);
        
        // Inset the text rect to make space for image
        float imageWidthPlusGap = _imageOp->_rect.size.width + _imageSpacing;
        rect.origin.x += imageWidthPlusGap;
        rect.size.width -= imageWidthPlusGap;
    }
    return rect;
}

ToolbarButton::ToolbarButton() {
    applyStyle("ToolbarButton");
}

bool ToolbarButton::handleInputEvent(INPUTEVENT* event) {
    
    // TODO: this is an iOS-only effect
    if (event->type == INPUT_EVENT_DOWN) {
        setAlpha(0.5f);
    }
    if (event->type == INPUT_EVENT_TAP_CANCEL || event->type==INPUT_EVENT_UP) {
        if (_alpha == 0.5f) {
            animateAlpha(1.0, 350);
        }
    }
    
    return ImageView::handleInputEvent(event);
}


#ifdef DEBUG
string Button::debugViewType() {
    char ach[256];
    snprintf(ach, 256, "Button:%s", _textLayout.getText().c_str());
    return string(ach, -1);
}
#endif

