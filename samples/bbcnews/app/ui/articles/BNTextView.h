//
//  BNTextView.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

@import UIKit;

#import "BNElementMedia.h"



@interface BNTextView : UITextView <UITextViewDelegate, UIScrollViewDelegate>

@property (nonatomic, weak) BNItem* item; // only used for Accessibility

- (void)setAttributedText:(NSAttributedString *)attributedText andNonTextElements:(NSArray*)nonTextElements;

@end
