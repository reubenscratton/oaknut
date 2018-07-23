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
        setView(_editText);

    }


};

void App::main() {
    //loadStyleAsset("styles.res");

    MainViewController* mainVC = new MainViewController();
    _window->setRootViewController(mainVC);
}


