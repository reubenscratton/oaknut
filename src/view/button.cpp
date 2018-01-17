//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

DECLARE_DYNCREATE(Button);
DECLARE_DYNCREATE(ToolbarButton);

Button::Button() {
    _gravity = {GRAVITY_CENTER,GRAVITY_CENTER};
}

void Button::applyStyleValues(const StyleValueList& values) {
    Label::applyStyleValues(values);
    _textRenderer.setGravity(_gravity);
    
    if (_borderColour) {
        setBackground(new RoundRectRenderOp(this, _backgroundColour, dp(2), _borderColour,  dp(6)));
        setBackground(new RoundRectRenderOp(this, _pressedFillColour, dp(2), _borderColour,  dp(6)), {STATE_PRESSED,STATE_PRESSED});
    } else {
        if (_backgroundColour) {
            setBackground(new ColorRectFillRenderOp(this, getBounds(), _backgroundColour));
        }
        if (_pressedFillColour) {
            setBackground(new ColorRectFillRenderOp(this, getBounds(), _pressedFillColour), {STATE_PRESSED,STATE_PRESSED});
        }

    }
    

}

bool Button::applyStyleValue(const string& name, StyleValue* value) {
    if (name == "background") {
        _backgroundColour = value->i;
        return true;
    } else if (name == "border-colour") {
        _borderColour = value->i;
        return true;
    }
    else if (name == "pressed-colour") {
        _pressedFillColour = value->i;
        return true;
    }
    return Label::applyStyleValue(name, value);
}

bool Button::onTouchEvent(int eventType, int finger, POINT pt) {
    if (eventType == TOUCH_EVENT_DOWN) {
        setPressed(true);
    }
    if (eventType == TOUCH_EVENT_CANCEL || eventType==TOUCH_EVENT_UP) {
        setPressed(false);
    }
    if (eventType == TOUCH_EVENT_TAP) {
        if (_onClickDelegate) {
            _onClickDelegate(this);
        }
    }
    
    return true;
}

void ToolbarButton::setImageUrl(const string& url) {
    lazyCreateImageView();
	_imageView->setImageUrl(url);
}

void ToolbarButton::setImageBitmap(Bitmap* bitmap) {
    lazyCreateImageView();
    _imageView->setBitmap(bitmap);
}

void ToolbarButton::lazyCreateImageView() {
    if (!_imageView) {
        _imageView = new ImageView();
        _imageView->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_FillParent);
        //_imageView->setMeasureSpecs(MEASURESPEC_WrapContent, MEASURESPEC_WrapContent);
        _imageView->setAlignSpecs(ALIGNSPEC_Top, ALIGNSPEC_Left);
        _imageView->setPadding(EDGEINSETS(dp(8),dp(8),dp(8),dp(8)));
        _imageView->setTintColour(_tintColour);
        addSubview(_imageView);
    }
}

bool ToolbarButton::onTouchEvent(int eventType, int finger, POINT pt) {
	if (eventType == TOUCH_EVENT_DOWN) {
		_alpha = 0.5f;
		setNeedsFullRedraw();
	}
	if (eventType == TOUCH_EVENT_CANCEL || eventType==TOUCH_EVENT_UP) {
		if (!_alphaAnim) {
			animateAlpha(1.0, 350);
		}
	}
	if (eventType == TOUCH_EVENT_TAP) {
		if (_onClickDelegate) {
			_onClickDelegate(this);
		}
	}

	return true;
}
