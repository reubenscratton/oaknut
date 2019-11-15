//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "../../indexes/cells/cellcontent.h"
#include "../../common/label.h"
//#import "BNLabelInfo.h"
//#import "BNRelationship.h"
//#import "BNStyles.h"
//#import "BNNavigationController.h"
//#import "BNAManager.h"


class BNCellRelatedTopic : public BNCellContent {
public:
    
    sp<BNCollection> _collection;
    BNLabel* _topicName;

    BNCellRelatedTopic(BNCellsModule* module) : BNCellContent(module) {
        _topicName = new BNLabel();
        _topicName->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        addSubview(_topicName);
        View* divider = new View();
        divider->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Abs(1));
        divider->setBackgroundColor(0xff00ff00); // self.inverseColorScheme ? [UIColor thinDividerInvColor] : [UIColor thinDividerColor];
        addSubview(divider);
    }
    
    void setRelationship(BNRelationship* rel) override {
        _collection = (BNCollection*)rel->_childObject;
        //self.textAreaInsets = textInsetsRelatedTopic;
        BNItem* parentItem = (BNItem*)rel->_parentObject;
        _inverseColorScheme = parentItem->isMediaItem();
        _topicName->setText(_collection->_name);
    }

/*
- (void)measureForContainingRect:(CGRect)rect {
	rect = UIEdgeInsetsInsetRect(rect, self.textAreaInsets);
	[self.topicName measureForWidth:rect.size.width offset:CGPointMake(self.textAreaInsets.left, self.textAreaInsets.top)];
	rect.size.height = self.topicName.bounds.size.height;
	rect = UIEdgeInsetsUninsetRect(rect, self.textAreaInsets);
	self.frameSize = rect.size;
}*/



    void onTapped() override {
        app->log("todo! open topic");
        //BNNavigationController* navController = [BNNavigationController get];
        //[navController openItem:self.collection withinCollection:nil andTitle:nil];
    }
};

DECLARE_DYNCREATE(BNCellRelatedTopic, BNCellsModule*);
