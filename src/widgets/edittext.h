//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class EditText : public Label, IKeyboardInputHandler, ITextInputReceiver {
public:

    // API
    EditText();
    virtual void setInsertionPoint(int32_t newInsertionPoint);
    
    
    // Overrides
    bool applyStyleValue(const string& name, const StyleValue* value) override;
    void setPadding(EDGEINSETS padding) override;
    IKeyboardInputHandler* getKeyboardInputHandler() override;
    ITextInputReceiver* getTextInputReceiver() override;
    bool onInputEvent(INPUTEVENT* event) override;
    bool setFocused(bool focused) override;
    void updateRenderOps() override;
    void layout() override;
    void detachFromWindow() override;
    void setText(const AttributedString& text) override;
    void updateContentSize(float parentWidth, float parentHeight) override;
    
    // IKeyboardInputHandler
    void keyInputEvent(KeyboardInputEventType keyboardInputEventType, KeyboardInputSpecialKeyCode specialKeyCode, int osKeyCode, char32_t charCode) override;

    // ITextInputReceiver
    bool insertText(string text, int replaceStart, int replaceEnd) override;
    void deleteBackward() override;
    int getTextLength() override;
    int getSelectionStart() override;
    int getInsertionPoint() override;
    string textInRange(int start, int end) override;
    void setSelectedRange(int start, int end) override;
    SoftKeyboardType getSoftKeyboardType() override;
    
    std::function<void(const AttributedString& before, AttributedString& after)> onTextChange;

protected:
    int32_t _selectionStart;    // Text index, not character index
    int32_t _insertionPoint;    // Text index, not character index
    bool _cursorOn;
    bool _cursorValid;
    bool _showClearButtonWhenNotEmpty;
    ObjPtr<Timer> _blinkCursorTimer;
    ObjPtr<RectRenderOp> _cursorRenderOp;
    ObjPtr<TextureRenderOp> _clearButtonOp;
    SoftKeyboardType _softKeyboardType = General;
    
    void updateCursor();
    void updateClearButton();
    void moveCursor(int dx, int dy);
};

