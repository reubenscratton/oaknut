//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "../../indexes/cells/cellcontent.h"
#include "../../common/label.h"
#include "../../common/imageview.h"
//#import "BNItem.h"


class BNCellMedia : public BNCellContent {
public:
    
    BNCellMedia(BNCellsModule* module);
    void setMediaObject(BNBaseModel* mediaObject, BNItem* containingItem);

    void setRelationship(BNRelationship* rel) override;

protected:
    sp<BNImage> _image;
    float _imageAspect;
    sp<BNItem> _containingItem;
    sp<BNAV> _media;
    // BNVideoView* videoView;
    BNLabel* _caption;
    bool _parallax;
    string _secondaryType;
    BNImageView* _imageView;

};

