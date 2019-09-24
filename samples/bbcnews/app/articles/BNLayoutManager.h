//
//  BNLayoutManager.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

@import UIKit;
#import "BNTextTraits.h"


/**
 A custom NSLayoutManager that can draw the selected area when a BNTextTraitLink is pressed.
 */

@interface BNLayoutManager : NSLayoutManager
@property (nonatomic) BNTextTraitLink* selectedLinkTrait;

- (BOOL)touchesBegan:(CGPoint)touchLocation textContainer:(NSTextContainer*)textContainer;
- (void)touchesCancelled;
- (void)touchesEnded;

@end
