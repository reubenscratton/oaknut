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

    MainViewController() {
        View* view = app.layoutInflate("layout/main.res");
        _editText = (EditText*)view->findViewById("editText");
        _editText->setGravity({GRAVITY_CENTER, GRAVITY_TOP});
        AttributedString str("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz. A long piece of text that should allow me to test multiline behaviours without having to type stuff in over and over and over. With a bit of luck this second sentence should push the whole EditText to overflow its bounds and thereby let me exercise the scroll behaviour");
        str.setAttribute(Attribute::forecolour(0xFFFF0000), 0, 1);
        str.setAttribute(Attribute::forecolour(0xFF7f0000), 1, 2);
        str.setAttribute(Attribute::forecolour(0xFF00FF00), 2, 3);
        str.setAttribute(Attribute::forecolour(0xFF007F00), 3, 4);
        str.setAttribute(Attribute::forecolour(0xFF0000FF), 4, 5);
        str.setAttribute(Attribute::forecolour(0xFF00007F), 5, 6);
        _editText->setAttributedText(str);
        setView(view);

    }


};

void App::main() {
    //loadStyleAsset("styles.res");

    MainViewController* mainVC = new MainViewController();
    _window->setRootViewController(mainVC);
}


