//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCell.h"
#import "BNTextView.h"

@interface BNCellArticleText : BNCell

@property (nonatomic, strong) BNTextView *textView;

- (id)initWithItem:(BNItem*)item andModule:(BNCellsModule *)module;

@end
