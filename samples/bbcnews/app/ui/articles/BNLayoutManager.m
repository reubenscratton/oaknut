#if 0
//
//  BNLayoutManager.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNLayoutManager.h"
#import "BNStyles.h"
#import "BNAManager.h"
#import "BBCNURLHandler.h"

@implementation BNLayoutManager


- (void)drawGlyphsForGlyphRange:(NSRange)glyphsToShow atPoint:(CGPoint)origin {

    NSTextStorage *textStorage = self.textStorage;
    NSRange glyphRange = glyphsToShow;
    while (glyphRange.length > 0) {
        NSRange charRange = [self characterRangeForGlyphRange:glyphRange actualGlyphRange:NULL];
        NSRange attributeCharRange, attributeGlyphRange;
        BNTextTraitLink* linkTrait = [textStorage attribute:BNLinkAttributeName
													atIndex:charRange.location longestEffectiveRange:&attributeCharRange
													inRange:charRange];
        attributeGlyphRange = [self glyphRangeForCharacterRange:attributeCharRange actualCharacterRange:NULL];
        attributeGlyphRange = NSIntersectionRange(attributeGlyphRange, glyphRange);
        if (linkTrait != nil && linkTrait == self.selectedLinkTrait) {
			CGContextRef context = UIGraphicsGetCurrentContext();
			CGContextSetFillColorWithColor(context, [UIColor defaultSelectedFillColor].CGColor);
			
			// Get the rects to draw
			NSMutableArray* rects = [NSMutableArray arrayWithCapacity:2];
			[self enumerateEnclosingRectsForGlyphRange:attributeGlyphRange withinSelectedGlyphRange:NSMakeRange(NSNotFound,0) inTextContainer:self.textContainers[0] usingBlock:^void (CGRect rect, BOOL *stop) {
				[rects addObject:[NSValue valueWithCGRect:rect]];
			}];
			
			// Draw the rects
			CGFloat cornerRadius = 8.f;
			for (NSUInteger i = 0; i < rects.count; i++) {
				NSValue *rectVal = rects[i];
				CGRect rect = [rectVal CGRectValue];
				rect.origin.x += origin.x;
				rect.origin.y += origin.y;
				rect = CGRectInset(rect, -6, -3);
				
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wassign-enum"
				UIRectCorner corners = 0;
				if (i == 0) {
					corners |= UIRectCornerTopLeft | UIRectCornerTopRight;
				}
				if (i == rects.count-1) {
					corners |= UIRectCornerBottomLeft | UIRectCornerBottomRight;
				}
#pragma clang diagnostic pop
				
				// Penultimate rect can also have a bottom-right corner
				if (i == rects.count-2) {
					NSValue* nextRectVal = rects[i+1];
					if (CGRectGetMaxX(rect) - CGRectGetMaxX([nextRectVal CGRectValue]) > 8) {
						corners |= UIRectCornerBottomRight;
					}
				}
				// Similarly the 2nd rect can have a top-left corner
				if (i == 1) {
					NSValue* prevRectVal = rects[0];
					if (CGRectGetMaxX([prevRectVal CGRectValue]) - CGRectGetMaxX(rect) > 8) {
						corners |= UIRectCornerTopLeft;
					}
				}
				UIBezierPath* path = [UIBezierPath bezierPathWithRoundedRect:rect byRoundingCorners:corners cornerRadii:CGSizeMake(cornerRadius, cornerRadius)];
				[path fill];
			}
        }
        glyphRange.length = NSMaxRange(glyphRange) - NSMaxRange(attributeGlyphRange);
        glyphRange.location = NSMaxRange(attributeGlyphRange);
    }
	[super drawGlyphsForGlyphRange:glyphsToShow atPoint:origin];
}



- (BOOL)touchesBegan:(CGPoint)touchLocation textContainer:(NSTextContainer*)textContainer {
	NSUInteger touchedChar = [self glyphIndexForPoint:touchLocation inTextContainer:textContainer];
	if (touchedChar < self.textStorage.length) {
		BNTextTraitLink* linkTrait = [self.textStorage attribute:BNLinkAttributeName atIndex:touchedChar effectiveRange:nil];
		if (linkTrait != nil) {
			self.selectedLinkTrait = linkTrait;
			[self invalidateDisplayForCharacterRange:NSMakeRange(linkTrait.start, linkTrait.end-linkTrait.start)];
			return YES;
		}
	}
	return NO;
}

- (void)touchesCancelled {
	if (self.selectedLinkTrait != nil) {
		[self invalidateDisplayForCharacterRange:NSMakeRange(self.selectedLinkTrait.start, self.selectedLinkTrait.end-self.selectedLinkTrait.start)];
		self.selectedLinkTrait = nil;
	}
}

- (void)touchesEnded {
	if (self.selectedLinkTrait != nil) {

		[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromInlineLink];
		
		bool useInAppBrowser = YES;
		if([[UIApplication sharedApplication] canOpenURL:self.selectedLinkTrait.url])
		{
			//todo check platform traits as doesnt appear to be therw
			if(self.selectedLinkTrait.platform){
				if(![self.selectedLinkTrait.platform isEqualToString:@"newsapps"]){
					[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeExtLink name:BNAActionNameNA labels:@{BNAActionAdditionalKeyExtLinkURL:[self.selectedLinkTrait.url absoluteString]}];
				}
				else{
					[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameInlineLink labels:nil];
				}
			
				// Some links must *always* be opened in the external browser
				if ([self.selectedLinkTrait.platform isEqualToString:@"newsapps-ext"]) {
					useInAppBrowser = NO;
				}
			}
			
			
			[[[BBCNURLHandler alloc] initWithURL:self.selectedLinkTrait.url] openURL:useInAppBrowser];
		}

		[self invalidateDisplayForCharacterRange:NSMakeRange(self.selectedLinkTrait.start, self.selectedLinkTrait.end-self.selectedLinkTrait.start)];
		self.selectedLinkTrait = nil;
	}
}
@end
#endif
