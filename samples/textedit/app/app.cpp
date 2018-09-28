//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class MainViewController : public ViewController {
public:

    EditText* _editText;

    View* _alignLeft;
    View* _alignCenter;
    View* _alignRight;
    View* _bold;

    MainViewController() {
        inflate("layout/main.res");
        bind(_editText, "editText");
        bind(_alignLeft, "alignLeft");
        bind(_alignCenter, "alignCenter");
        bind(_alignRight, "alignRight");
        bind(_bold, "bold");

        _editText->setGravity({GRAVITY_CENTER, GRAVITY_TOP});
        AttributedString str("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz. A long piece of text that should allow me to test multiline behaviours without having to type stuff in over and over and over. With a bit of luck this second sentence should push the whole EditText to overflow its bounds and thereby let me exercise the scroll behaviour");
        //AttributedString str("A\nB\nC\nD\nE\nF\nG\nH");
        str.setAttribute(Attribute::forecolor(0xFFFF0000), 0, 1);
        str.setAttribute(Attribute::forecolor(0xFF7f0000), 1, 2);
        str.setAttribute(Attribute::forecolor(0xFF00FF00), 2, 3);
        str.setAttribute(Attribute::forecolor(0xFF007F00), 3, 4);
        str.setAttribute(Attribute::forecolor(0xFF0000FF), 4, 5);
        str.setAttribute(Attribute::forecolor(0xFF00007F), 5, 6);
        str.setAttribute(Attribute::bold(), 0, 6);
        _editText->setText(str);
        
        _editText->onInsertionPointChanged = [&](int32_t before, int32_t& after) {
            //asm("int3;");
        };
        
        //string str("AA BB CC DD EE FF GG HH II JJ KK LL MM NN OO PP QQ RR SS TT UU VV WW XX YY ZZ");
        //_editText->setText(str);

        
        _alignLeft->onInputEvent = [&](View* view,INPUTEVENT* ev) -> bool {
            if (ev->type == INPUT_EVENT_TAP) {
                setTextAlignment(GRAVITY_LEFT);
            }
            return true;
        };
        _alignCenter->onInputEvent = [&](View*,INPUTEVENT* ev) -> bool {
            if (ev->type == INPUT_EVENT_TAP) {
                setTextAlignment(GRAVITY_CENTER);
            }
            return true;
        };
        _alignRight->onInputEvent = [&](View*,INPUTEVENT* ev) -> bool {
            if (ev->type == INPUT_EVENT_TAP) {
                setTextAlignment(GRAVITY_RIGHT);
            }
            return true;
        };
        
        _editText->onInsertionPointChanged = [&](int32_t insertionPointBefore, int32_t& insertionPointAfter) {
            auto fontWeightAttrib = _editText->getAttribute(insertionPointAfter, Attribute::FontWeight);
            bool isBold = (fontWeightAttrib && fontWeightAttrib->_f >= FONT_WEIGHT_BOLD);
            _bold->setSelected(isBold);
        };
        _bold->onClick = []() -> bool {
            return true;
        };
    }

    void setTextAlignment(int gravity) {
        _alignLeft->setState(STATE_SELECTED, (gravity==GRAVITY_LEFT) ? STATE_SELECTED : 0);
        _alignCenter->setState(STATE_SELECTED, (gravity==GRAVITY_CENTER) ? STATE_SELECTED : 0);
        _alignRight->setState(STATE_SELECTED, (gravity==GRAVITY_RIGHT) ? STATE_SELECTED : 0);
        _editText->setGravity({static_cast<uint8_t>(gravity), 0});
    }
};



void App::main() {
    _window->setRootViewController(new MainViewController());
}


