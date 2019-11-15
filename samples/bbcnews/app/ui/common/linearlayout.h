//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "containermodule.h"


class BNLinearLayout : public BNContainerModule {
public:

    BNLinearLayout(const variant& json);
    
    // Cloning
    BNLinearLayout(BNLinearLayout* source);
    BNModule* clone() override;

    //void updateSubviews(View* superview) override;
    //void removeAllViews() override;
    View* createView() override;

    bool isLinearLayout() override { return true; }

};
