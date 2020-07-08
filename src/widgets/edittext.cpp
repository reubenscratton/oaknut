//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(EditText);

IKeyboardInputHandler* EditText::getKeyboardInputHandler() {
    return this;
}
ITextInputReceiver* EditText::getTextInputReceiver() {
    return this;
}

EditText::EditText() : Label() {
    applyStyle("EditText");
}

bool EditText::handleInputEvent(INPUTEVENT* event) {
    if (event->type == INPUT_EVENT_DOWN && event->deviceType!=INPUTEVENT::ScrollWheel) {
        POINT pt = event->ptLocal;
        setInsertionPoint(_textLayout.charIndexFromPoint(pt));
    }
    if (event->type == INPUT_EVENT_TAP) {
        if (_clearButtonOp && _clearButtonOp->_rect.contains(event->ptLocal)) {
            setText("");
            return true;
        }
        requestFocus();
    }
    Label::handleInputEvent(event);
    return true;
}

void EditText::detachFromWindow() {
    Label::detachFromWindow();
    updateCursor();
}

void EditText::onStateChanged(VIEWSTATE changedStates) {
    Label::onStateChanged(changedStates);
    if (changedStates.mask & STATE_FOCUSED) {
        updateCursor();
    }
}

bool EditText::applySingleStyle(const string& name, const style& value) {
    if (name == "softKeyboardType") {
        string softKeyboardType = value.stringVal().lowercase();
        if (softKeyboardType == "phone") {
            _softKeyboardType = KeyboardPhone;
        } else if (softKeyboardType == "email") {
            _softKeyboardType = KeyboardEmail;
        } else if (softKeyboardType == "general") {
            _softKeyboardType = KeyboardGeneral;
        } else {
            log_warn("invalid softKeyboardType '%s'", softKeyboardType.c_str());
        }
        return true;
    }
    if (name == "actionType") {
        string actionType = value.stringVal().lowercase();
        if (actionType == "none") {
            _actionType = ActionNone;
        } else if (actionType == "next") {
            _actionType = ActionNext;
        } else if (actionType == "search") {
            _actionType = ActionSearch;
        } else if (actionType == "done") {
            _actionType = ActionDone;
        } else if (actionType == "go") {
            _actionType = ActionGo;
        } else {
            log_warn("invalid actionType '%s'", actionType.c_str());
        }
        return true;
    }
    if (name == "next") {
        _next = value.stringVal();
        _actionType = ActionNext;
        return true;
    }
    if (name == "maxLength") {
        setMaxLength(value.intVal());
        return true;
    }
    if (name == "iconTint") {
        _iconTint = value.colorVal();
        return true;
    }
    return Label::applySingleStyle(name, value);
}

void EditText::updateCursor() {
    if (_blinkCursorTimer) {
        _blinkCursorTimer->stop();
        _blinkCursorTimer = NULL;
    }
    if (!_window || !isFocused()) {
        if (_cursorRenderOp) {
            if (_cursorRenderOp->_view) {
                removeRenderOp(_cursorRenderOp);
            }
            _cursorRenderOp = NULL;
        }
        return;
    }
    if (!_cursorRenderOp) {
        _cursorRenderOp = new RectRenderOp();
        _cursorRenderOp->setColor(0xff333333); // todo: style
        _cursorOn = true;
    }
    // If the content size is invalid then we can't update the cursor immediately. This will get
    // called again when updateContentSize() completes.
    if (!_intrinsicSizeValid) {
        return;
    }
    POINT cursorOrigin;
    float ascent, descent;
    _textLayout.charOriginFromIndex(_insertionPoint, &cursorOrigin, &ascent, &descent);
    _cursorRenderOp->setRect(RECT(cursorOrigin.x,cursorOrigin.y-ascent,4 /*todo: style*/,ascent-descent));
    _cursorValid = true;
    if (_cursorOn && !_cursorRenderOp->_batch) {
        addRenderOp(_cursorRenderOp);
    } else if (!_cursorOn && _cursorRenderOp->_batch) {
        removeRenderOp(_cursorRenderOp);
    }
    
    invalidateRect(_cursorRenderOp->_rect); // todo: shouldn't this be unnecessary? i.e. implicit to add/removeOp
    _blinkCursorTimer = Timer::start([=]() {
        _cursorOn = !_cursorOn;
        _blinkCursorTimer = NULL;
        updateCursor();
    }, 500, false); // todo: style
}

void EditText::setPadding(EDGEINSETS padding) {
    if (_showClearButtonWhenNotEmpty) {
        padding.right += app->dp(16);
    }
    Label::setPadding(padding);
}

void EditText::updateIntrinsicSize(SIZE constrainingSize) {
    Label::updateIntrinsicSize(constrainingSize);
    _cursorOn = true;
    updateCursor();
}

void EditText::setText(const attributed_string& text) {
    auto tt = text;
    if (onTextChange) {
        onTextChange(_textLayout.getText(), tt);
    }
    Label::setText(tt);
    _selectionStart = MIN(_selectionStart, (int)tt.length());
    _insertionPoint = MIN(_insertionPoint, (int)tt.length());
    updateClearButton();
    if (_window && isFocused()) {
        _window->keyboardNotifyTextChanged();
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
                _clearButtonOp = new TextureRenderOp(nullptr, _iconTint);
                app->loadBitmapAsset("images/edittext_clear.png", [=](Bitmap* bitmap) {
                    _clearButtonOp->setBitmap(bitmap);
                });

                RECT rect = getOwnRectPadded();
                _clearButtonOp->_rect.origin.x = rect.origin.x+rect.size.width-app->dp(22);
                _clearButtonOp->_rect.origin.y = rect.origin.y+(rect.size.height-app->dp(22)) / 2;
                addRenderOp(_clearButtonOp);
            }
        }
    }
}

void EditText::updateRenderOps() {
    Label::updateRenderOps();
    updateCursor();
}

void EditText::layout(RECT constraint) {
    Label::layout(constraint);
    if (_clearButtonOp) {
    }
}

/*
 ITextInputReceiver
 */
bool EditText::insertText(string insertionText, int replaceStart, int replaceEnd) {
    if (insertionText=="\n" && _textLayout.maxLines() >0) {
        if (_textLayout.lineCount() >= _textLayout.maxLines()) {
            handleActionPressed();
            return false;
        }
    }
    string text = getText();
    if (replaceEnd-replaceStart > 0) {
        text.eraseAt(replaceStart, replaceEnd);
    }
    text.insertAt(replaceStart, insertionText);
    if (_maxLength>0 && text.length() > _maxLength) {
        return false;
    }
    setText(text);
    return true;
}

void EditText::deleteBackward() {
    string text = getText();
    if (_selectionStart != _insertionPoint) {
        int s = MIN(_selectionStart, _insertionPoint);
        int e = MAX(_selectionStart, _insertionPoint);
        text.eraseAt(s, e);
        setText(text);
        int newInsertionPoint = MIN(s, (int)text.length());
        setInsertionPointReal(newInsertionPoint);
        _selectionStart = newInsertionPoint;
    }
    else if (_insertionPoint > 0) {
        text.eraseAt(_insertionPoint - 1);
        _insertionPoint--;
        _selectionStart--;
        setText(text);
    }
    notifySelectionChanged();
}

void EditText::notifySelectionChanged() {
    if (_window && _window->_textInputReceiver == this) {
        _window->keyboardNotifyTextSelectionChanged();
    }
}

int EditText::getTextLength() {
    return (int)_textLayout.getText().length();
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
    notifySelectionChanged();
}

string EditText::textInRange(int start, int end) {
    if (start>end) {
        std::swap(start, end);
    }
    const string& text = _textLayout.getText();
    if (start>=text.length()) start = (int)text.length();
    if (start<0) start=0;
    if (end>=text.length()) end = (int)text.length();
    if (end<0) end=0;
    return text.substr(start, end);
}

void EditText::setInsertionPoint(int32_t newInsertionPoint) {
    setInsertionPointReal(newInsertionPoint);
    _selectionStart = newInsertionPoint; // todo: not if shift held
    _cursorOn = true;
    updateCursor();
    
    // Ensure the cursor is fully visible (i.e. autoscroll it into view)
    int lineIndex = _textLayout.lineIndexFromCharIndex(_insertionPoint, 0);
    auto lineRect = _textLayout.lineRect(lineIndex);
    float dy = lineRect.top() - _contentOffset.y;
    if (dy < 0) {
        scrollBy({0,dy});
    } else {
        dy = lineRect.bottom() - (_rect.size.height+_contentOffset.y);
        if (dy > 0) {
            scrollBy({0,dy});
        }
    }
    notifySelectionChanged();
}

void EditText::moveCursor(int dx, int dy) {
    setInsertionPoint(_textLayout.charIndexMove(_insertionPoint, dx, dy));
}



void EditText::setActionType(ActionType actionType) {
    _actionType = actionType;
}



/*
IKeyboardInputHandler
*/
void EditText::keyInputEvent(KeyboardInputEventType keyboardInputEventType, KeyboardInputSpecialKeyCode specialKeyCode, int osKeyCode, char32_t charCode)  {
    if (keyboardInputEventType == KeyboardInputEventType::KeyUp) {
        return;
    }
    
    switch (specialKeyCode) {
        case SpecialKeyNone:
            break;
        case SpecialKeyDelete:
            deleteBackward();
            return;
        case SpecialKeyCursorLeft:
            moveCursor(-1, 0);
            return;
        case SpecialKeyCursorRight:
            moveCursor(1, 0);
            return;
        case SpecialKeyCursorUp:
            moveCursor(0, -1);
            return;
        case SpecialKeyCursorDown:
            moveCursor(0, 1);
            return;
        default:
            return;
    }

    if (charCode=='\r') charCode = '\n';
    if (charCode=='\n' && _textLayout.maxLines() >0) {
        if (_textLayout.lineCount() >= _textLayout.maxLines()) {
            return;
        }
    }

    string str;
    str.append(charCode);
    insertText(str, _selectionStart, _insertionPoint);
    _selectionStart++;
    setInsertionPoint(_selectionStart);
}

void EditText::setInsertionPointReal(int32_t& newInsertionPoint) {
    if (onInsertionPointChanged) {
        onInsertionPointChanged(_insertionPoint, newInsertionPoint);
    }
    _insertionPoint = newInsertionPoint;
}

void EditText::setMaxLength(int32_t maxLength) {
    if (maxLength != _maxLength) {
        _maxLength = maxLength;
        auto& text = _textLayout.getText();
        if (text.length() > maxLength) {
            attributed_string truncText = text;
            truncText.eraseAt(maxLength);
            setText(truncText);
        }
    }
}

void EditText::handleActionPressed() {
    if (onKeyboardAction) {
        onKeyboardAction();
    }
    
    // Move
    if (_actionType == ActionNext && _next.length()>0) {
        View* nextView = getRootView()->findViewById(_next);
        if (nextView) {
            nextView->requestFocus();
        }
    }
}

SoftKeyboardType EditText::getSoftKeyboardType() {
    return _softKeyboardType;
}

ActionType EditText::getActionType() {
    return _actionType;
}
