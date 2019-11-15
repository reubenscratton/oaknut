//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "cellcontent.h"
#include "../../../model/_module.h"
#include "../../common/imageview.h"
#include "../../common/topiclabel.h"
#include "../../../data/network/urlrequest.h"
#include "../../common/durationlabel.h"


class BNCellItem : public BNCellContent {
public:
    enum BNCellStyle {
        TopStory,
        Feature,
        Digest,
        PhotoGallery
    };
    
    BNCellItem(BNCellsModule* module, BNCellStyle cellStyle);

    

    // Runtime type determinants
    bool isCellItem() override { return true; }
    virtual bool isCellItemRelatedStory() { return false; }

    void setItem(BNItem* item) override;
    void setRelationship(BNRelationship* relationship) override;

    void onTapped() override;
    void setPrefetchLevel(int level) override;
    void setDummyInfo(const string& text) override;

    // Views
    BNImageView* _imageView;
    View* _textFrame;
    BNLabel* _headline;
    BNTopicLabel* _topic;
    BNLabel* _dummyLabel;
    BNDurationLabel* _duration;

    BNLabel* _summary;
    BNItem* _item;
    bool _showMediaGlyphInHeadline;
    bool _hideTimestamp;
    float _imageAspect;
    sp<BNURLRequest> _reqPrefetch;

protected:
    BNCollection* getOwningCollection();
};

