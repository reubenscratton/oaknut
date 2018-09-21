//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


enum SoftKeyboardType {
    General,
    Phone,
    Number,
    Email,
};

/**
 * @class ITextInputReceiver
 * @brief Interface that must be implemented by Views which receive text input
 */
class ITextInputReceiver {
public:
    virtual bool insertText(string text, int replaceStart, int replaceEnd) = 0;
    virtual void deleteBackward() = 0;
    virtual int getTextLength() = 0;
    virtual int getSelectionStart() = 0;
    virtual int getInsertionPoint() = 0;    // NB: insertion point is also the end of selection
    virtual void setSelectedRange(int start, int end) = 0;
    virtual string textInRange(int start, int end) = 0;
    virtual SoftKeyboardType getSoftKeyboardType() = 0;
};

/**
 * @class IKeyboardInputHandler
 * @brief Interface to be implemented by Views which want physical keyboard keyDown & keyUp events
 */
enum KeyboardInputEventType {
    KeyDown,
    KeyUp
};
enum KeyboardInputSpecialKeyCode {
    SpecialKeyNone,
    SpecialKeyShift,
    SpecialKeyControl,
    SpecialKeyCommand,
    SpecialKeyFunction,
    SpecialKeyEscape,
    SpecialKeyAlt,
    SpecialKeyDelete,
    SpecialKeyCapsLock,
    SpecialKeyCursorLeft,
    SpecialKeyCursorRight,
    SpecialKeyCursorUp,
    SpecialKeyCursorDown,
};
class IKeyboardInputHandler {
public:
    virtual void keyInputEvent(KeyboardInputEventType keyboardInputEventType, KeyboardInputSpecialKeyCode specialKeyCode, int osKeyCode, char32_t charCode) = 0;
};

