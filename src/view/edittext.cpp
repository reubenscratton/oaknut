//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

DECLARE_DYNCREATE(EditText);

IKeyboardInputHandler* EditText::getKeyboardInputHandler() {
    return this;
}

EditText::EditText() : Label() {
    _gravity.vert = GRAVITY_CENTER;
}

bool EditText::onTouchEvent(int eventType, int eventSource, POINT pt) {
    if (eventType == INPUT_EVENT_TAP) {
        if (_clearButtonOp && _clearButtonOp->_rect.contains(pt)) {
            setText("");
            return true;
        }
        becomeFirstResponder();
    }
    return true;//View::onTouchEvent(eventType, eventSource, pt);
}

void EditText::detachFromWindow() {
    if (isFirstResponder()) {
        resignFirstResponder();
    }
    Label::detachFromWindow();
    if (_blinkCursorTimer) {
        _blinkCursorTimer->stop();
    }
}

bool EditText::becomeFirstResponder() {
    bool r = Label::becomeFirstResponder();
    if (r) {
        _blinkCursorTimer = Timer::start(std::bind(&EditText::blinkCursor, this), 500, true);
        blinkCursor();
    }
    return r;
}

void EditText::blinkCursor() {
    if (!_cursorRenderOp) {
        updateCursor();
    }
    if (_cursorRenderOp->_batch) {
        removeRenderOp(_cursorRenderOp);
    } else {
        addRenderOp(_cursorRenderOp);
    }
    invalidateRect(_cursorRenderOp->_rect); // todo: shouldn't this be unnecessary? i.e. implicit to add/removeOp
}

void EditText::updateCursor() {
    if (!isFirstResponder()) {
        if (_cursorRenderOp) {
            removeRenderOp(_cursorRenderOp);
            _cursorRenderOp = NULL;
        }
        return;
    }
    if (!_cursorRenderOp) {
        _cursorRenderOp = new ColorRectFillRenderOp(this);
        _cursorRenderOp->setColour(0xff333333);
    }
    POINT cursorOrigin;
    float ascent, descent;
    _textRenderer.getGlyphOrigin(_insertionPoint, &cursorOrigin, &ascent, &descent);
    _cursorRenderOp->setRect(RECT_Make(cursorOrigin.x,cursorOrigin.y-ascent,4,ascent-descent));
    _cursorValid = true;
}

void EditText::setPadding(EDGEINSETS padding) {
    if (_showClearButtonWhenNotEmpty) {
        padding.right += dp(16);
    }
    Label::setPadding(padding);
}

void EditText::setText(string text) {
    Label::setText(text);
    _selectionStart = min(_selectionStart, (int)text.length());
    _insertionPoint = min(_insertionPoint, (int)text.length());
    updateClearButton();
    if (isFirstResponder()) {
        oakKeyboardNotifyTextChanged();
    }
}

void EditText::updateClearButton() {
    if (getTextLength() == 0) {
        if (_clearButtonOp) {
            removeRenderOp(_clearButtonOp);
            _clearButtonOp = NULL;
        }
    } else {
        if (_showClearButtonWhenNotEmpty) {
            if (!_clearButtonOp) {
                _clearButtonOp = new TextureRenderOp(this, "images/edittext_clear.png", 0xff999999);
                RECT rect = getBoundsWithPadding();
                _clearButtonOp->_rect.origin.x = rect.origin.x+rect.size.width-_clearButtonOp->_rect.size.width;
                _clearButtonOp->_rect.origin.y = rect.origin.y+(rect.size.height-_clearButtonOp->_rect.size.height) / 2;
                addRenderOp(_clearButtonOp);
            }
        }
    }
}

void EditText::updateRenderOps() {
    Label::updateRenderOps();
    updateCursor();
}

void EditText::layout() {
    Label::layout();
    if (_clearButtonOp) {
    }
}

void EditText::insertText(string insertionText, int replaceStart, int replaceEnd) {
    string text = getText();
    if (replaceEnd-replaceStart > 0) {
        text.erase(text.begin()+replaceStart, text.begin()+replaceEnd);
    }
    text.insert(replaceStart, insertionText);
    setText(text);
}

void EditText::deleteBackward() {
    string text = getText();
    if (_selectionStart != _insertionPoint) {
        int s = min(_selectionStart, _insertionPoint);
        int e = max(_selectionStart, _insertionPoint);
        text.erase(text.begin() + s, text.begin() + e);
        setText(text);
        _insertionPoint = _selectionStart = min(s, (int)text.length());
    }
    else if (_insertionPoint > 0) {
        text.erase(text.begin() + _insertionPoint - 1);
        _insertionPoint--;
        setText(text);
    }
}

int EditText::getTextLength() {
    return (int)_textRenderer.getText().length();
}

int EditText::getSelectionStart() {
    return _selectionStart;
}
int EditText::getInsertionPoint() {
    return _insertionPoint;
}
void EditText::setSelectedRange(int start, int end) {
    _selectionStart = start;
    _insertionPoint = end;
    _cursorValid = false;
}

string EditText::textInRange(int start, int end) {
    if (start>end) {
        std::swap(start, end);
    }
    const string& text = _textRenderer.getText();
    if (start>=text.length()) start = (int)text.length();
    if (start<0) start=0;
    if (end>=text.length()) end = (int)text.length();
    if (end<0) end=0;
    //int len = end-start;
    // TODO: make utf8 safe
    return string(text.begin()+start, text.begin() + end);
}
