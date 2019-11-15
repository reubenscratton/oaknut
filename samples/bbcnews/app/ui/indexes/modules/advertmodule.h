//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "module.h"

class BNAdvertModule : public BNModule {
public:
    
    BNAdvertModule(const variant& json);

    // Clonable
    BNAdvertModule(BNAdvertModule* source);
    BNModule* clone() override;
    
protected:
    bool _contentAllowsAdvertising;
    //BNAdvertView* _advertView;
    bool _parallax;

};
