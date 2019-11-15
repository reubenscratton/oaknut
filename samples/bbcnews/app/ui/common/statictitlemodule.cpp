//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "statictitlemodule.h"
#include "containermodule.h"
#include "cellsmodule.h"
#include "linearlayout.h"
/*#import "BNStyles.h"
#import "BNAppDelegate.h"
#import "BBCNURLHandler.h"
*/

DECLARE_DYNCREATE(BNStaticTitleModule, const variant&);

BNStaticTitleModule::BNStaticTitleModule(const variant& json) : BNModule(json) {
    _text = json.stringVal("title");
    string titleColor = json.stringVal("titleColor");
    if (titleColor.length()) {
        _titleColor = app->getStyleColor(titleColor);
    } else {
        _titleColor = 0xFF000000;
    }
    _contentId = json.stringVal("contentLink");
    string styleName = json.stringVal("textAttrs");
    if (!styleName.length()) {
        styleName = "title";
    }
    _style = app->getStyle(styleName);
    _numLines = json.intVal("numLines");
}

BNStaticTitleModule::BNStaticTitleModule(BNStaticTitleModule* source) : BNModule(source) {
	_text = source->_text;
	_titleColor = source->_titleColor;
	_contentId = source->_contentId;
	_style = source->_style;
	_numLines = source->_numLines;
}

BNModule* BNStaticTitleModule::clone() {
    return new BNStaticTitleModule(this);
}
/*
void BNStaticTitleModule::layoutWithContainingRect(const RECT& rect)  {

    // Find our index
    int myIndex = 0;
    for (BNModule* m : _container->_modules) {
        if (m==this) break;
        myIndex++;
    }
    
	// If module following this one is empty, then we occupy no space
	if (myIndex != _container->_modules.size()-1) {
        BNModule* nextModule = _container->_modules[myIndex+1];
		if (nextModule->isCellsModule()) {
			BNCellsModule* cellsModule = (BNCellsModule*)nextModule;
			if (cellsModule->_cells.size() == 0) {
                _frame = RECT::zero();
				return;
			}
		} else if (nextModule->isStaticTitleModule()) {
			nextModule->layoutWithContainingRect(rect); // slightly inefficient
			if (nextModule->_frame.size.height == 0.f) {
                _frame = RECT::zero();
				return;
			}
		} else if (nextModule->isLinearLayout()) {
			// Suggested (RS) fix for 'Your Questions Answered'
			BNLinearLayout *linearLayout = (BNLinearLayout*)nextModule;
			BNModule *module = linearLayout->_modules[0];
			if (module->isCellsModule()) {
				BNCellsModule* cellsModule = (BNCellsModule*)module;
				if (cellsModule->_cells.size() == 0) {
                    _frame = RECT::zero();
					return;
				}
			}
			
	
		}
	}

	RECT rect2 = rect.copyWithInsets(_padding);
	if (!_label) {
        _label = new BNLabel();
        _label->_useFullWidth = true;
        _label->setMaxLines(_numLines);
        _label->setText(_text);
        _label->applyStyle(*_style);
        if (_titleColor) {
            _label->setTextColor(_titleColor);
        }
	}
	rect2 = rect2.copyWithInsets(_textPadding);
	_labelInfo->measureForWidth(rect2.size.width, rect2.origin);
	_labelInfo->_bounds = _labelInfo->_bounds.copyWithUninsets(_textPadding);
	//rect.size.height = self.labelInfo.bounds.size.height;
	//self.frame = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height + self.padding.bottom);
	_frame = _labelInfo->_bounds.copyWithUninsets(_padding);
}

void BNStaticTitleModule::updateSubviews(View* superview) {
	bool visible = superview->getOwnRect().intersects(_frame) && _frame.size.height>0;
	if (visible == _visible) {
		return;
	}
	_visible = visible;
	if (visible && _frame.size.height>0) {
        createLabelView(superview);
	} else {
        removeLabelView();
	}
}

void BNStaticTitleModule::removeAllViews() {
	_visible = false;
    removeLabelView();
}

bool BNStaticTitleModule::showBackgroundForLinks() {
	return true;
}
*/

void BNStaticTitleModule::addToView(View* superview) {
    BNLabel* label = new BNLabel();
    label->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    label->setText(_text);
    label->setTextColor(_titleColor);
    label->setPadding(_padding);
	if (_contentId.length()) {
		//_labelInfo->createButton(superview);
		
		//Button* button = _label.as<Button>();
		//if (showBackgroundForLinks()) {
        //    AttributedString s = label->getText();
        //    s.setAttribute(Attribute::forecolor(0xFFCCCCCC), 0, s.length());
		//	label->setText(s);
		//} else {
            label->setBackgroundColor(app->getStyleColor("defaultSelectedFill"));
		//}
		
        label->onClick = [=]() {
            onTitleTapped();
        };
		
		//TODO [self.labelInfo.label setAccessibilityTraits:UIAccessibilityTraitHeader | UIAccessibilityTraitButton];

	} else {
		//TODO [self.labelInfo.label setAccessibilityTraits:UIAccessibilityTraitHeader];
	}
    superview->addSubview(label);
}


void BNStaticTitleModule::onTitleTapped() {

    app->warn("TODO: need URL launcher");
	if (_contentId.hasPrefix("/")) {
		//[[[BBCNURLHandler alloc] initWithURL:[BNAppDelegate URLforResourceSpec:self.contentId]] openURL:YES];
	} else {
		//NSURL *url = [NSURL URLWithString:self.contentId];
		//[[[BBCNURLHandler alloc] initWithURL:url] openURL:YES];
	}

}


