//
//  BNCell.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#include "cell.h"
#include "cellsmodule.h"


BNCell::BNCell(BNCellsModule *module) {
    _module = module;
    _usesScrollviewOffset = module->_usesScrollviewOffset;
    COLOR color = _module->getBackgroundColor();
    if (color) {
        setBackgroundColor(color);
    }
    setLayoutSize(LAYOUTSPEC::Fill(), LAYOUTSPEC::Wrap());
}

void BNCell::setItem(BNItem* item) {
}
void BNCell::setRelationship(BNRelationship* rel) {
}
void BNCell::setPrefetchLevel(int prefetchLevel) {
    _prefetchLevel = prefetchLevel;
}


void BNCell::layout(RECT constraint) {
    View::layout(constraint);
}



void BNCell::setDummyInfo(const string& text) {
    _isDummy = true;
}

//void BNCell::adviseScrollviewOffset(float offset) {
//}

void BNCell::onIsInitialContent() {
}
void BNCell::onSuperviewDidAppear(bool viewControllerIsMovingToParent) {
}
void BNCell::onSuperviewDidDisappear(bool viewControllerIsMovingFromParent) {
}

