//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(TabBar);

TabBar::TabBar() {
    applyStyle("TabBar");
}

#ifdef DEBUG
string TabBar::debugViewType() {
    return "TabBar";
}
#endif
