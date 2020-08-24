//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ViewSwitcher);

ViewSwitcher::ViewSwitcher() : View() {
}


void ViewSwitcher::setCurrentSubview(int subviewIndex) {
    for (int i=0 ; i<_subviews.size() ; i++) {
        _subviews[i]->setVisibility((i==subviewIndex) ? Visible : Gone);
    }
}
