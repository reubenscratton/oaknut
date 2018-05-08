//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(Button);
DECLARE_DYNCREATE(ToolbarButton);

Button::Button() {
    setGravity({GRAVITY_CENTER,GRAVITY_CENTER});
}

void Button::applyStyleValues(const StyleValueList& values) {
    Label::applyStyleValues(values);
    _textRenderer.setGravity(_gravity);
    
    if (_borderColour) {
        setBackground(new RoundRectRenderOp(this, _backgroundColour, app.dp(2), _borderColour,  app.dp(6)));
        setBackground(new RoundRectRenderOp(this, _pressedFillColour, app.dp(2), _borderColour,  app.dp(6)), {STATE_PRESSED,STATE_PRESSED});
    } else {
        if (_backgroundColour) {
            setBackground(new ColorRectFillRenderOp(this, getOwnRect(), _backgroundColour));
        }
        if (_pressedFillColour) {
            setBackground(new ColorRectFillRenderOp(this, getOwnRect(), _pressedFillColour), {STATE_PRESSED,STATE_PRESSED});
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
    if (eventType == INPUT_EVENT_DOWN) {
        setPressed(true);
    }
    if (eventType == INPUT_EVENT_CANCEL || eventType==INPUT_EVENT_UP) {
        setPressed(false);
    }
    if (eventType == INPUT_EVENT_TAP) {
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
        _imageView->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::FillParent());
        //_imageView->setMeasureSpecs(MEASURESPEC::WrapContent(), MEASURESPEC::WrapContent());
        _imageView->setAlignSpecs(ALIGNSPEC::Top(), ALIGNSPEC::Left());
        _imageView->setPadding(EDGEINSETS(app.dp(8),app.dp(8),app.dp(8),app.dp(8)));
        _imageView->setTintColour(_tintColour);
        addSubview(_imageView);
    }
}

bool ToolbarButton::onTouchEvent(int eventType, int finger, POINT pt) {
	if (eventType == INPUT_EVENT_DOWN) {
		setAlpha(0.5f);
	}
	if (eventType == INPUT_EVENT_CANCEL || eventType==INPUT_EVENT_UP) {
		if (_alpha == 0.5f) {
			animateAlpha(1.0, 350);
		}
	}
	if (eventType == INPUT_EVENT_TAP) {
		if (_onClickDelegate) {
			_onClickDelegate(this);
		}
	}

	return true;
}
