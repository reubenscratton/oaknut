//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "element.h"

class BNElementNonText : public BNElement {
public:
    
    POINT _frameOffset;         // for typesetter use
    RECT _frame;		        // includes margins
    RECT _contentFrame; // excludes margins, should be equal to view.frame
    bool _disableForceFullWidth;

    View* getView();

    virtual void measureForContainingRect(const RECT& rect);
    virtual POINT getContentFrameOrigin()=0;  // excludes margins
    virtual void setContentFrameOrigin(POINT pt)=0;  // excludes margins
    virtual void updateSubviews(View* superview);
    // void onTextKitDealloc();

};
