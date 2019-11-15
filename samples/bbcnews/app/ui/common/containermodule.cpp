//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "containermodule.h"

//#import "BNCellsModule.h"
//#import "BNCell.h"


BNContainerModule::BNContainerModule(const variant& json) : BNModule(json) {
    auto& jsonModules = json.arrayRef("modules");
    for (auto& jsonModule : jsonModules) {
        string moduleClassName = jsonModule.stringVal("class");
        if (!moduleClassName.length()) {
            moduleClassName = "BNCellsModule";
        }
        BNModule* module = (BNModule*)Object::createByName(moduleClassName, jsonModule);
        module->_container = this;
        _modules.push_back(module);
    }
}


BNContainerModule::BNContainerModule(BNContainerModule* source) : BNModule(source) {
    for (BNModule* module : source->_modules) {
        BNModule* module_copy = module->clone();
        module_copy->_container = this;
        _modules.push_back(module_copy);
    }
}
BNModule* BNContainerModule::clone() {
    return new BNContainerModule(this);
}

View* BNContainerModule::createView() {
    return new View();
}

void BNContainerModule::addToView(View* parent) {
    if (!_view) {
        _view = createView();
        parent->addSubview(_view);
    }
    for (BNModule* module : _modules) {
        module->addToView(_view);
    }
}

/*
RECT BNContainerModule::layoutModules(vector<BNModule*> modules, RECT containingRect) {
	if (_container) {
		return _container->layoutModules(modules, containingRect);
	}
	return RECT(0,0,0,0);
}
*/
void BNContainerModule::updateLayoutWithContentObject(BNContent* contentObject) {
    BNModule::updateLayoutWithContentObject(contentObject);
    for (auto submodule : _modules) {
		try{
			submodule->updateLayoutWithContentObject(contentObject);
		}
        catch(...) {
			app->warn("Exception in layout");
		}
	}
}

/*
void BNContainerModule::layoutWithContainingRect(const RECT& containingRect) {
    _frame = layoutModules(_modules, containingRect);
}

RECT BNContainerModule::boundsAfter(RECT frame) {
	if (_container) {
		return _container->boundsAfter(frame);
	}
	return frame;
}
*/
void BNContainerModule::setIsOnScreen(bool isOnScreen) {
    _isOnScreen = isOnScreen;
    for (auto module : _modules) {
		module->setIsOnScreen(isOnScreen);
	}
}

void BNContainerModule::onIsInitialContent() {
    for (auto module : _modules) {
        module->onIsInitialContent();
	}
}

void BNContainerModule::addItemsToArray(vector<BNBaseModel*>& array) {
    for (auto module : _modules) {
        module->addItemsToArray(array);
    }
}

void BNContainerModule::onSuperviewDidAppear(bool viewControllerIsMovingToParent) {
    for (auto module : _modules) {
        module->onSuperviewDidAppear(viewControllerIsMovingToParent);
    }
}
void BNContainerModule::onSuperviewDidDisappear(bool viewControllerIsMovingFromParent) {
    for (auto module : _modules) {
        module->onSuperviewDidDisappear(viewControllerIsMovingFromParent);
    }
}
void BNContainerModule::cleanup() {
    for (auto module : _modules) {
        module->cleanup();
    }
}


