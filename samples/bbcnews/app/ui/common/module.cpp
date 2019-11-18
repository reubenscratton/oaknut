//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "module.h"
#include "containermodule.h"

// For whatever reason I copied UIEdgeInsetsFromString in BBC News 2014, which Oaknut does not.
EDGEINSETS BNModule::edgeInsetsVal(const variant& v, const string& field) {
    string s = v.stringVal(field);
    if (!s.length()) {
        return {0,0,0,0};
    }
    s.trim();
    s.hadPrefix("{");
    s.hadSuffix("}");
    auto a = s.split(",");
    EDGEINSETS e;
    e.top = a[0].asFloat();
    e.left = a[1].asFloat();
    e.bottom = a[2].asFloat();
    e.right = a[3].asFloat();
    return e;
}

BNModule::BNModule(const variant& json) {
    _json = json;
    style s;
    s.fromVariant(json);
    _modes = json.stringArrayVal("modes");
    auto colorstr = s.stringVal("background");
    if (colorstr.length()) {
        if (colorstr.charAt(0) == '#') {
            _backgroundColor = s.colorVal("background");
        } else {
            _backgroundColor = app->getStyleColor("color."_S + colorstr);
        }
    }
    _textPadding = edgeInsetsVal(json, "textPadding");
    float num = json.floatVal("textPaddingHorizontal");
    if (num > 0) {
        _textPadding.left = _textPadding.right = num;
    }
    _padding = edgeInsetsVal(json, "padding");
    num = json.floatVal("paddingHorizontal");
    if (num > 0) {
        _padding.left = _padding.right = num;
    }
    num = json.floatVal("paddingTop");
    if (num > 0) {
        _padding.top = num;
    }
    num = json.floatVal("paddingBottom");
    if (num > 0) {
        _padding.bottom = num;
    }
}


BNModule::BNModule(BNModule* source) {
    _json = source->_json;
    _modes = source->_modes;
    _backgroundColor = source->_backgroundColor;
    _padding = source->_padding;
    _textPadding = source->_textPadding;
}
BNModule* BNModule::clone() {
    return new BNModule(this);
}


COLOR BNModule::getBackgroundColor() const {
	return _backgroundColor ? _backgroundColor :
        (_container ? _container->getBackgroundColor() : COLOR(0));
}

void BNModule::updateLayoutWithContentObject(BNContent* contentObject) {
	_contentObject = contentObject;
}

void BNModule::addToView(View* superview) {
}
/*
void BNModule::layoutWithContainingRect(const RECT& bounds) {
}

void BNModule::extendToHeight(float height) {
    _frame.size.height = height;
}

void BNModule::removeAllViews() {
}*/
/*
- (id)copyWithZone:(NSZone *)zone {
	BNModule* copy = [[[self class] allocWithZone:zone] init];
	copy.json = self.json;
	copy.modes = self.modes;
	copy.backgroundColor = self.backgroundColor;
	copy.padding = self.padding;
	copy.textPadding = self.textPadding;
	return copy;
}*/

float BNModule::getState() {
	return 0.f;
}

void BNModule::applyState(float state) {
}

void BNModule::addItemsToArray(vector<BNBaseModel*>& array) {
}

void BNModule::onIsInitialContent() {
}

void BNModule::invalidateSize() {
	BNModule* module = this;
	while (module && !module->_moduleHolder) {
		module = module->_container;
	}
    module->_moduleHolder->invalidateModuleSize(this);
}

void BNModule::onSuperviewDidAppear(bool viewControllerIsMovingToParent) {
}
void BNModule::onSuperviewDidDisappear(bool viewControllerIsMovingFromParent) {
}
void BNModule::cleanup() {
}




