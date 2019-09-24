//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "label.h"


class BNDurationLabel : public BNLabel {
public:
    
    bool _isLive;
    ImageView* _imageView;

    BNDurationLabel(class BNItem* item, const string& styleName);

    //void measureForWidth(float width, POINT offset) override;
    //void createLabel(View* superview) override;
    //void removeLabel() override;

};
