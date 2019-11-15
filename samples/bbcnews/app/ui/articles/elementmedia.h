//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "elementnontext.h"
#include "../../model/_module.h"
//#import "BNItem.h"
//#import "BNCellMedia.h"
//#import "BNCellsModuleArticleBody.h"


class BNElementMedia : public BNElementNonText {
public:
    
    BNElementMedia(BNRelationship* relationship);
    //BNCellMedia* createMediaCell(BNCellsModuleArticleBody* module);
    void measureForContainingRect(const RECT& rect) override;
    POINT getContentFrameOrigin() override;
    void setContentFrameOrigin(POINT pt) override;

    string _mediaId;
    sp<BNBaseModel> _media;
    
    //BNCellMedia* _mediaCell;
    
    bool isElementMedia() override { return true; }

};


class BNElementImage : public BNElementMedia {
public:
    class BNImage* getImage();
};

class BNElementAudio : public BNElementMedia {
public:
    class BNAudio* getAudio();
};

class BNElementVideo : public BNElementMedia {
public:
    class BNVideo* getVideo();
};

