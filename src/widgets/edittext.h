//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class EditText : public Label, IKeyboardInputHandler {
public:

    EditText();
    
    // Overrides
    virtual void setPadding(EDGEINSETS padding);
    virtual IKeyboardInputHandler* getKeyboardInputHandler();
    virtual bool onTouchEvent(int eventType, int eventSource, POINT pt);
    virtual bool setFocused(bool focused);
    virtual void updateRenderOps();
    virtual void layout();
    virtual void detachFromWindow();
    virtual void setText(string text);
    
    // IKeyboardInputHandler
    virtual void insertText(string text, int replaceStart, int replaceEnd);
    virtual void deleteBackward();
    virtual int getTextLength();
    virtual int getSelectionStart();
    virtual int getInsertionPoint();
    virtual string textInRange(int start, int end);
    virtual void setSelectedRange(int start, int end);
    
protected:
    int _selectionStart;
    int _insertionPoint;
    bool _cursorOn;
    bool _cursorValid;
    bool _showClearButtonWhenNotEmpty;
    ObjPtr<Timer> _blinkCursorTimer;
    ObjPtr<RenderOp> _cursorRenderOp;
    ObjPtr<TextureRenderOp> _clearButtonOp;
    
    void blinkCursor();
    void updateCursor();
    void updateClearButton();
};

