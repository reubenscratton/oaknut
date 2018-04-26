//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * \interface IKeyboardInputHandler
 * \brief Interface that must be implemented by Views which provide text entry via a system soft keyboard.
 */
class IKeyboardInputHandler {
public:
    virtual void insertText(string text, int replaceStart, int replaceEnd) = 0;
    virtual void deleteBackward() = 0;
    virtual int getTextLength() = 0;
    virtual int getSelectionStart() = 0;
    virtual int getInsertionPoint() = 0;    // NB: insertion point is also the end of selection
    virtual void setSelectedRange(int start, int end) = 0;
    virtual string textInRange(int start, int end) = 0;
};


