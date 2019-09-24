//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "cellcontent.h"
#include "../../model/_module.h"
#include "../imageview.h"
#include "../topiclabel.h"
#include "../../data/network/urlrequest.h"


class BNCellItem : public BNCellContent {
public:
    BNCellItem(BNCellsModule* module);

    // Runtime type determinants
    bool isCellItem() override { return true; }

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

    BNLabel* _summary;
    BNItem* _item;
    bool _showMediaGlyphInHeadline;
    bool _hideTimestamp;
    bool _hideTopics;
    float _imageAspect;
    sp<BNURLRequest> _reqPrefetch;

protected:
    BNCollection* getOwningCollection();
};

