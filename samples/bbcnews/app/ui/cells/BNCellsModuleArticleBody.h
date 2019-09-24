//
//  BNCellsModuleArticleBody.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCellsModule.h"
#import "BNItem.h"

@interface BNCellsModuleArticleBody : BNCellsModule

@property (nonatomic) BNItem *item;
@property (nonatomic) NSAttributedString* attributedText;
@property (nonatomic) NSArray* nontextElements;

- (void)realLayoutWithContainingRect:(CGRect)rect;

@end
