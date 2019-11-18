
//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(SearchBox);

SearchBox::SearchBox() {
    applyStyle("SearchBox");
    _searchIconOp = new TextureRenderOp("images/search.png", 0xff555555);
    addRenderOp(_searchIconOp);
    _showClearButtonWhenNotEmpty = true;    
}

void SearchBox::setSearchTextChangedDelegate(SEARCHTEXTCHANGED delegate) {
    _searchTextChangedDelegate = delegate;
}

float SearchBox::spaceForSearchIcon() {
    return app->dp(18);
}
void SearchBox::setPadding(EDGEINSETS padding) {
    padding.left += spaceForSearchIcon();
    EditText::setPadding(padding);
}

RECT SearchBox::getIconRect(float val) {
    RECT rect = getOwnRectPadded();
    
    SIZE searchIconSize = {app->dp(16),app->dp(16)};
    
    rect.origin.y = rect.midY() - searchIconSize.height / 2;
    rect.size.height = searchIconSize.height;

    float spaceForSearchIcon = this->spaceForSearchIcon();
    rect.origin.x -= spaceForSearchIcon;
    rect.size.width += spaceForSearchIcon;
    float x1 = rect.origin.x + (rect.size.width - searchIconSize.width) / 2;
    float x2 = rect.origin.x;
    rect.origin.x = x1 + (x2-x1)*val;
    rect.size.width = searchIconSize.width;
    return rect;
}

void SearchBox::layout(RECT constraint) {
    EditText::layout(constraint);
    _searchIconOp->setRect(getIconRect(isFocused()?1:0));
}

void SearchBox::onStateChanged(STATESET changes) {
    EditText::onStateChanged(changes);
    if ((changes.mask & STATE_FOCUSED) && (changes.state & STATE_FOCUSED)) {
        Animation::start(this, 250,  [=](float val) {
            _searchIconOp->setRect(getIconRect(val));
            setNeedsFullRedraw();
        });
    }
}


void SearchBox::setText(const attributed_string& text) {
    EditText::setText(text);
    if (_searchTextChangedDelegate) {
        _searchTextChangedDelegate(this, text);
    }
}


