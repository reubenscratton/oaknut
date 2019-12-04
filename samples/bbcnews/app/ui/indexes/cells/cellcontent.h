//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "../../common/cell.h"

class BNCellContent : public BNCell {
public:
    BNCellContent(BNCellsModule* module);
    

    EDGEINSETS _textAreaInsets;
    bool _inverseColorScheme;

    virtual void onTapped();
    bool handleInputEvent(INPUTEVENT* event) override;
    void detachFromSurface() override;

    RectRenderOp* _highlightOp;
};
