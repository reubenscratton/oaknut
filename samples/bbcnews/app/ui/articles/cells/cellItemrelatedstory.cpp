//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "../../indexes/cells/cellitemtopstory.h"


class BNCellItemRelatedStory : public BNCellItemTopStory {
public:

    BNCellItemRelatedStory(BNCellsModule* module) : BNCellItemTopStory(module) {
        
    }

    bool isCellItemRelatedStory() override { return true; }


    void setRelationship(BNRelationship* rel) override {
        BNCellItemTopStory::setRelationship(rel);
        BNItem* parentItem = (BNItem*)rel->_parentObject;
        _inverseColorScheme = parentItem->isMediaItem();
        // self.headlineAttrs = self.inverseColorScheme ? attrsH3Inv : attrsH3;
    }
/*
    void measureForContainingRect:(CGRect)rect {
	[super measureForContainingRect:rect];
	
	// Add a vertical margin to top and bottom
	CGFloat vmargin = IS_IPAD ? 16 : 12;
	self.imageOrigin = CGPointMake(self.imageOrigin.x, self.imageOrigin.y+vmargin);
	self.headline.bounds = CGRectOffset(self.headline.bounds, 0, vmargin);
	self.topic.bounds = CGRectOffset(self.topic.bounds, 0, vmargin);
	self.frameSize = CGSizeMake(self.frameSize.width, self.frameSize.height+vmargin*2);
}


- (void)createView:(UIView*)superview {
	[super createView:superview];
	UIView* divider = [[UIView alloc] initWithFrame:CGRectMake(0,0,self.frameSize.width,1)];
	divider.autoresizingMask = UIViewAutoresizingFlexibleWidth;
	divider.backgroundColor = self.inverseColorScheme ? [UIColor thinDividerInvColor] : [UIColor thinDividerColor];
	[self.view addSubview:divider];
}

- (void)onTapped {
	[super onTapped];
	//echo needs view topic first
	[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromRelatedStoryLink];
	
	[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameRelatedStoryLink labels:nil];

}

*/
    
    
};

DECLARE_DYNCREATE(BNCellItemRelatedStory, BNCellsModule*);
