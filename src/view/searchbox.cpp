
//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

DECLARE_DYNCREATE(SearchBox);

SearchBox::SearchBox() {
    _roundRectOp = new RoundRectRenderOp(this, 0xFFFFFFFF, 0, 0, app.dp(5));
    addRenderOp(_roundRectOp);
    _searchIconOp = new TextureRenderOp(this, "images/search.png", 0xff555555);
    addRenderOp(_searchIconOp);
    setBackgroundColour(0xffcccccc);
    setGravity({GRAVITY_LEFT,GRAVITY_CENTER});
    setPadding(EDGEINSETS(app.dp(16), app.dp(2), app.dp(16), app.dp(2)));
    _showClearButtonWhenNotEmpty = true;
    setTextColour(0xff000000);
}

void SearchBox::setSearchTextChangedDelegate(SEARCHTEXTCHANGED delegate) {
    _searchTextChangedDelegate = delegate;
}

float SearchBox::spaceForSearchIcon() {
    return app.dp(18);
}
void SearchBox::setPadding(EDGEINSETS padding) {
    padding.left += spaceForSearchIcon();
    EditText::setPadding(padding);
}

void SearchBox::layout() {
    EditText::layout();
    RECT rect = getBounds();
    RECT_inset(rect, app.dp(12), app.dp(6));
    _roundRectOp->setRect(rect);
    
    SIZE searchIconSize = SIZE_Make(app.dp(16),app.dp(16));
    _searchIconOp->setRect( RECT_Make(rect.origin.x+(rect.size.width-searchIconSize.width)/2,rect.origin.y+(rect.size.height-searchIconSize.height)/2, searchIconSize.width, searchIconSize.height));
}

bool SearchBox::becomeFirstResponder() {
    bool r = EditText::becomeFirstResponder();
    if (r) {
        DelegateAnimation* anim = new DelegateAnimation();
        anim->_interpolater = linear;
        anim->_delegate = [=](float val) {
            RECT iconRect;
            iconRect.size = SIZE_Make(app.dp(16), app.dp(16));
            RECT paddedBounds = getBoundsWithPadding();
            float spaceForSearchIcon = this->spaceForSearchIcon();
            paddedBounds.origin.x -= spaceForSearchIcon;
            paddedBounds.size.width += spaceForSearchIcon;
            iconRect.origin.y = paddedBounds.origin.y + (paddedBounds.size.height - iconRect.size.height) / 2;
            float x1 = paddedBounds.origin.x + (paddedBounds.size.width - iconRect.size.width) / 2;
            float x2 = paddedBounds.origin.x;
            iconRect.origin.x = x1 + (x2-x1)*val;
            _searchIconOp->setRect(iconRect);
        };
        anim->start(_window, 250);
    }
    return r;
}


void SearchBox::setText(string text) {
    EditText::setText(text);
    if (_searchTextChangedDelegate) {
        _searchTextChangedDelegate(this, text);
    }
}


