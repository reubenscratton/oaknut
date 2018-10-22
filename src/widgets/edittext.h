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
    virtual void setMaxLength(int32_t maxLength);
    virtual void setActionType(ActionType actionType);
    
    // Overrides
    bool applyStyleValue(const string& name, const StyleValue* value) override;
    void setPadding(EDGEINSETS padding) override;
    IKeyboardInputHandler* getKeyboardInputHandler() override;
    ITextInputReceiver* getTextInputReceiver() override;
    bool handleInputEvent(INPUTEVENT* event) override;
    void onStateChanged(STATESET changedStates) override;
    void updateRenderOps() override;
    void layout() override;
    void detachFromWindow() override;
    void setText(const AttributedString& text) override;
    void updateContentSize(SIZE constrainingSize) override;
    
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
    ActionType getActionType() override;
    void handleActionPressed() override;


    std::function<void(const AttributedString& before, AttributedString& after)> onTextChange;
    std::function<void(int32_t insertionPointBefore, int32_t& insertionPointAfter)> onInsertionPointChanged;
    std::function<void()> onKeyboardAction;

    
protected:
    int32_t _selectionStart;    // Text index, not character index
    int32_t _insertionPoint;    // Text index, not character index
    int32_t _maxLength;
    bool _cursorOn;
    bool _cursorValid;
    bool _showClearButtonWhenNotEmpty;
    sp<Timer> _blinkCursorTimer;
    sp<RectRenderOp> _cursorRenderOp;
    sp<TextureRenderOp> _clearButtonOp;
    SoftKeyboardType _softKeyboardType = KeyboardGeneral;
    ActionType _actionType = ActionNone;
    string _next; // id of next field
    
    void updateCursor();
    void updateClearButton();
    void moveCursor(int dx, int dy);
    void setInsertionPointReal(int32_t& newInsertionPoint);
    void notifySelectionChanged();
};

