//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "scrollablecontainer.h"

class BNVerticalStack : public BNContainerModule {
public:
    BNVerticalStack(const variant& json);

    // Cloneable
    BNVerticalStack(BNVerticalStack* source);
    BNModule* clone() override;

    View* createView() override;
    
    // Overrides
    //RECT layoutModules(vector<BNModule*> modules, RECT containingRect) override;
    //RECT boundsAfter(RECT frame) override;
    //void updateScrollsToTop() override;

};
