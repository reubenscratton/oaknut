//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#ifdef PLATFORM_LINUX

#include	<oaknut.h>


int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    Style::loadStyleAssetSync("styles.res");
    app->_window = Window::create();
    app->_window->show();
    app->main();
    gtk_main();
    return(0);
}


#endif

