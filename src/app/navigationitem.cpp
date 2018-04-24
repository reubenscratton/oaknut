//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

NavigationItem::NavigationItem() {
    _leftButtonsFrame = new LinearLayout();
    _leftButtonsFrame->_orientation = LinearLayout::Horizontal;
    _leftButtonsFrame->setMeasureSpecs(MEASURESPEC_WrapContent, MEASURESPEC_WrapContent);
    _leftButtonsFrame->setAlignSpecs(ALIGNSPEC_Left, ALIGNSPEC_Center);
    _rightButtonsFrame = new LinearLayout();
    _rightButtonsFrame->_orientation = LinearLayout::Horizontal;
    _rightButtonsFrame->setMeasureSpecs(MEASURESPEC_WrapContent, MEASURESPEC_WrapContent);
    _rightButtonsFrame->setAlignSpecs(ALIGNSPEC_Right, ALIGNSPEC_Center);
}

void NavigationItem::setTitle(const string& title) {
	if (_titleView) {
		_titleView->removeFromParent();
	}
	Label* titleLabel = new Label();
	_titleView = titleLabel;
	titleLabel->setMeasureSpecs(MEASURESPEC_WrapContent, MEASURESPEC_WrapContent);
	titleLabel->setAlignSpecs(ALIGNSPEC_Center, ALIGNSPEC_Make(NULL, 0.5f, -0.5f, 0));
	
    // Todo: use an "apply style" method here
	titleLabel->setFont(app.getFont("navbar.title"));
	titleLabel->setTextColour(app.getColour("navbar.title.forecolour"));
	titleLabel->setText(title);

}

void NavigationItem::setTitleView(View* titleView) {
	if (_titleView) {
		_titleView->removeFromParent();
	}
	titleView->setMeasureSpecs(MEASURESPEC_WrapContent, MEASURESPEC_WrapContent);
	titleView->setAlignSpecs(ALIGNSPEC_Center, ALIGNSPEC_Make(NULL, 0.5f, -0.5f, 0));
	_titleView = titleView;
}

ToolbarButton* NavigationItem::createIconButton(const string& src, OnClickDelegate onClickDelegate) {
	ToolbarButton* button = new ToolbarButton();
	button->setMeasureSpecs(MEASURESPEC_UseAspect(1), MEASURESPEC_FillParent);
    ByteBuffer* data = app.loadAsset(src.data());
    Bitmap::createFromData(data->data, (int)data->cb, [=](Bitmap* bitmap) {
        button->setImageBitmap(bitmap);
    });
	button->_onClickDelegate = onClickDelegate;
	return button;
}

void NavigationItem::addLeftButton(ToolbarButton* button) {
    _leftButtonsFrame->addSubview(button);
}

void NavigationItem::addRightButton(ToolbarButton* button) {
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
		float titleDistance = _titleView->_frame.size.width/2 + navBar->_frame.size.width/2;
		if (isPop) {
			_titleView->setAnimTranslate(POINT_Make(-tx * titleDistance,0));
        } else {
            _titleView->setAnimTranslate(POINT_Make(tx * titleDistance,0));
        }
	}
    _rightButtonsFrame->setAlpha(alpha);
}

