//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#include <oaknut.h>
#include "ui/layoutsgroup.h"


class BBCNewsApp : public App {
public:
    
    void main() override;

    BNLayout* layoutWithContent(const string& modelId,
                                const string& site,
                                const string& format,
                                const string& orientation);

    void setCompactMode(bool compactMode);
    void setCarouselsMode(bool carouselsMode);


    void loadLayouts();
    
    BNLayoutsGroup* _currentLayouts;
    bool _compactMode;
    bool _carouselsMode;
    bool _hasEverUsedCarouselsMode;
};





