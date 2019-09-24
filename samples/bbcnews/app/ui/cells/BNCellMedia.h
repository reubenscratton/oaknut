//
//  BNCellMedia.h
//  BBCNews
//
//  Copyright (c) 2013 British Broadcasting Corporation. All rights reserved.
//

#import "BNCellContent.h"
#import "BNLabel.h"
#import "BNItem.h"


@interface BNCellMedia : BNCellContent

- (void)setMediaObject:(BNBaseModel*)mediaObject andContainingItem:(BNItem*)containingItem;

@property (nonatomic) CGSize imageSize;

@end
