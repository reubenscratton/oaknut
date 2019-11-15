//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "../common/statictitlemodule.h"

class BNRelatedContentTitleModule : public BNStaticTitleModule {
public:
    
    BNRelatedContentTitleModule(const variant& json) : BNStaticTitleModule(json) {
    }
    
    void updateLayoutWithContentObject(BNContent* contentObject) override {
        BNStaticTitleModule::updateLayoutWithContentObject(contentObject);
        //self.titleColor = [contentObject isMediaItem] ? [UIColor contentForegroundInvColor] : [UIColor contentForegroundColor];
        //self.dividerColor = [contentObject isMediaItem] ? [UIColor thinDividerInvColor] : [UIColor thinDividerColor];

    }

    void addToView(View* superview) override {
        BNStaticTitleModule::addToView(superview);
        /*
         if (self.visible && !self.topDivider) {
             
             // Insert view underneath label to avoid the parallax'd image sometimes showing through on iPad.
             CGRect frame = self.frame;
             frame.size.width += frame.origin.x*2;
             frame.origin.x = 0;
             frame.origin.y -= 16;
             frame.size.height += 16;
             self.backgroundView = [[UIView alloc] initWithFrame:frame];
             self.backgroundView.backgroundColor = self.backgroundColor;
             [superview insertSubview:self.backgroundView belowSubview:self.labelInfo.label];
             
             self.labelInfo.label.backgroundColor = superview.backgroundColor;
             self.topDivider = [[UIView alloc] initWithFrame:CGRectMake(self.frame.origin.x,self.frame.origin.y,self.frame.size.width,4)];
             self.topDivider.backgroundColor = self.dividerColor;
             [superview addSubview:self.topDivider];
         } else if (!self.visible && self.topDivider) {
             [self.topDivider removeFromSuperview];
             self.topDivider = nil;
             [self.backgroundView removeFromSuperview];
             self.backgroundView = nil;
         }

         */
    }


protected:
    View* _backgroundView;
    View* _topDivider;
    COLOR _dividerColor;



/*
- (void)layoutWithContainingRect:(CGRect)rect {
    [super layoutWithContainingRect:rect];
    
    // Add space for top divider
    CGFloat extraSpace = IS_IPAD ? 24 : 12;
    if (self.frame.size.height > 0) {
        CGRect rect = self.frame;
        rect.size.height += extraSpace;
        self.frame = rect;
        rect = self.labelInfo.bounds;
        rect.origin.y += extraSpace;
        self.labelInfo.bounds = rect;
    }
}
*/


};

DECLARE_DYNCREATE(BNRelatedContentTitleModule, const variant&);
