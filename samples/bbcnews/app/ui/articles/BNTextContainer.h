//
//  BNTextContainer.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

@import UIKit;


@interface BNTextContainer : NSTextContainer //<NSLayoutManagerDelegate>

@property NSArray* nonTextElements;
@property (nonatomic, readonly) CGFloat measuredHeight;

@end

