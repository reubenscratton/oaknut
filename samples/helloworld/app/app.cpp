//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


void App::main() {
    Label* label = new Label();
    label->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Fill());
    label->setBackgroundColor(0xFFFFFFFF);
    label->setGravity({GRAVITY_CENTER, GRAVITY_CENTER});
    label->setText("Hello World!");
    ViewController* vc = new ViewController();
    vc->setView(label);
    _window->setRootViewController(vc);
}
