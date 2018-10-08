//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

NavigationItem::NavigationItem() {
    _leftButtonsFrame = new LinearLayout();
    _leftButtonsFrame->_orientation = LinearLayout::Horizontal;
    _leftButtonsFrame->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    _leftButtonsFrame->setAlignSpecs(ALIGNSPEC::Left(), ALIGNSPEC::Center());
    _rightButtonsFrame = new LinearLayout();
    _rightButtonsFrame->_orientation = LinearLayout::Horizontal;
    _rightButtonsFrame->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    _rightButtonsFrame->setAlignSpecs(ALIGNSPEC::Right(), ALIGNSPEC::Center());
}

void NavigationItem::setTitle(const string& title) {
	if (_titleView) {
		_titleView->removeFromParent();
        _titleView = NULL;
	}
    _title = title;
}

void NavigationItem::setTitleView(View* titleView) {
	if (_titleView) {
		_titleView->removeFromParent();
	}
	titleView->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    titleView->setAlignSpecs(ALIGNSPEC::Center(), ALIGNSPEC(NULL, 0.5f, -0.5f, 0));
	_titleView = titleView;
    _title = "";
}

ImageView* NavigationItem::createIconButton(const string& src, std::function<void()> onClick) {
	ToolbarButton* button = new ToolbarButton();
    ByteBuffer* data = app.loadAsset(src.data());
    Bitmap::createFromData(data->data, (int)data->cb, [=](Bitmap* bitmap) {
        button->setBitmap(bitmap);
    });
	button->onClick = onClick;
	return button;
}

void NavigationItem::addLeftButton(ImageView* button) {
    _leftButtonsFrame->addSubview(button);
}

void NavigationItem::addRightButton(ImageView* button) {
    _rightButtonsFrame->addSubview(button);
}


/**
NavVC item:
When animating a push/pop, the left & right buttons fade in/out and the 
incoming title also slides in/out as well as fades
*/
void NavigationItem::applyTransition(NavigationBar* navBar, float val, bool incoming, bool isPop) {
	float alpha = incoming?val:(1-val);
    _leftButtonsFrame->setAlpha(alpha);
	if (_titleView) {
		_titleView->setAlpha(alpha);
		float tx = incoming ? (1-val) : -val;
		float titleDistance = _titleView->getWidth()/2 + navBar->getWidth()/2;
		if (isPop) {
            _titleView->setTranslate({-tx * titleDistance,0});
        } else {
            _titleView->setTranslate({tx * titleDistance,0});
        }
	}
    _rightButtonsFrame->setAlpha(alpha);
}

