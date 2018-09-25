//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(PinCodeView);

PinCodeView::PinCodeView() {
    _orientation = Horizontal;
    _subviewsInheritState = true;
}

string PinCodeView::getText() {
    string str;
    for (auto subview : _subviews) {
        EditText* field = (EditText*)(View*)subview;
        str.append(field->getText());
    }
    return str;
}

void PinCodeView::clear() {
    View* firstField = _subviews[0];
    for (auto subview : _subviews) {
        EditText* field = (EditText*)(View*)subview;
        field->setText("");
        field->setEnabled(field == firstField);
    }
    firstField->requestFocus();
}

class PinCodeViewField : public EditText {
public:
    PinCodeViewField() {
        applyStyle("pincodeField");
        setMaxLength(1);
        setPreferredActionType(ActionNext);
    }
    
    void deleteBackward() override {
        if (_textRenderer.getText().length() == 0) {
            if (_prev) {
                _prev->requestFocus();
                _prev->deleteBackward();
                setEnabled(false);
            }
            return;
        }
        EditText::deleteBackward();
    }
    
    PinCodeViewField* _prev;
    PinCodeViewField* _next;
};

bool PinCodeView::applyStyleValue(const string &name, const StyleValue *value) {
    if (name == "numDigits") {
        int numDigits = value->intVal();
        PinCodeViewField* prev = NULL;
        for (int i=0 ; i<numDigits ; i++) {
            PinCodeViewField* field = new PinCodeViewField();
            if (i>0) {
                field->setEnabled(false);
            }
            field->_prev = prev;
            if (prev) {
                prev->_next = field;
            }
            prev = field;
            field->onTextChange = [=](const AttributedString& before, AttributedString& after) {
                int index = indexOfSubview(field);
                if (before.length()==0 && after.length()==1) {
                    if (index < _subviews.size() - 1) {
                        EditText* nextField = (EditText*)(View*)_subviews[index+1];
                        nextField->setEnabled(true);
                        nextField->requestFocus();
                    } else {
                        if (onFilled) {
                            onFilled(true);
                        }
                    }
                }
                if (before.length()==1 && after.length()==0) {
                    if (onFilled) {
                        onFilled(false);
                    }
                }
            };
            field->onKeyboardAction = [=]() {
                if (onKeyboardAction) {
                    onKeyboardAction();
                }
            };
            addSubview(field);
        }
        return true;
    }
    return LinearLayout::applyStyleValue(name, value);
}


