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

        _editText = new EditText();
        _editText->setBackgroundColour(0xFFFFFFFF);
        _editText->setPadding(EDGEINSETS(16,16,16,16));
        _editText->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::FillParent());
        _editText->setAlignSpecs(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        _editText->setGravity({GRAVITY_CENTER, GRAVITY_TOP});
        _editText->setText("A long piece of text that should allow me to test multiline behaviours without having to type stuff in over and over and over. With a bit of luck this second sentence should push the whole EditText to overflow its bounds and thereby let me exercise the scroll behaviour");
        setView(_editText);

    }


};

void App::main() {
    //loadStyleAsset("styles.res");

    MainViewController* mainVC = new MainViewController();
    _window->setRootViewController(mainVC);
}


