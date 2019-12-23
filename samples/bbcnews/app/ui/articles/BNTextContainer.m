#if 0
//
//  BNTextContainer.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNTextContainer.h"
#import "BNElementMedia.h"
#import "BNImage.h"

@interface BNTextContainer ()

@property (nonatomic) CGFloat nextLineTop;
@property (nonatomic) CGFloat currentBottomNTE;
@property BOOL newLayoutPass;
@property int nextNTEToPosition;
@end

@implementation BNTextContainer

- (id)initWithSize:(CGSize)size {
	self = [super initWithSize:size];
	if (self) {
		self.lineFragmentPadding = 0.f;
	}
	return self;
}


- (void)setSize:(CGSize)size {
	self.newLayoutPass = YES;
	[super setSize:size];
}

- (CGFloat)measuredHeight {

	// Position any images that come after the last bit of text laid out via lineFragEtc
	[self positionMediaElementsThatOccurBeforeCharIndex:INT32_MAX];
	
	return MAX(self.nextLineTop, self.currentBottomNTE);
}

- (void)positionMediaElementsThatOccurBeforeCharIndex:(NSUInteger)characterIndex {

	while (self.nextNTEToPosition < self.nonTextElements.count) {
		BNElementNonText* nte = self.nonTextElements[self.nextNTEToPosition];
		if (nte.charIndex <= characterIndex) {
			CGFloat y = MAX(self.currentBottomNTE, self.nextLineTop);
			// Update the mediaCell.frame (which excludes margins)
			CGRect frame = nte.frame;
			frame.origin.y = nte.frameOffset.y + y;
			nte.frame = frame;
			CGPoint cellFrameOrigin = nte.contentFrameOrigin;
			cellFrameOrigin.y =  frame.origin.y + nte.marginTop;
			nte.contentFrameOrigin = cellFrameOrigin;
			//NSLog(@"Positioning nte at %@  %@", NSStringFromCGPoint(nte.mediaCell.frameOrigin), NSStringFromCGSize(nte.mediaCell.frameSize));
			y += nte.frame.size.height;
			
			// If this is a full-width NTE then subsequent text cannot flow around it, so we move currentBottomText down.
			// Moreover, if we are preceded by any half-width images then they may need to be promoted to full-width
			// to avoid huge empty spaces. Android has to go to similar trouble here.
			if (nte.normalizedWidth == 1.f) {
				CGFloat growth = 0;
				for (int i=0 ; i<=self.nextNTEToPosition ; i++) {
					BNElementNonText* ntePrev = self.nonTextElements[i];
					if (growth > 0) {
						CGRect frame = ntePrev.frame;
						frame.origin.y += growth;
						ntePrev.frame = frame;
						CGPoint origin = ntePrev.contentFrameOrigin;
						origin.y += growth;
						ntePrev.contentFrameOrigin = origin;
					}
					
					if ((ntePrev.frame.origin.y - nte.frameOffset.y) >= self.nextLineTop) {
						CGFloat prevY = ntePrev.frame.origin.y;
						CGFloat currentHeight = ntePrev.frame.size.height;
						if (ntePrev.normalizedWidth <1.f && !ntePrev.disableForceFullWidth) {
							//NSLog(@"Promote to fullwidth: nte=%@  nextLineTop=%f", NSStringFromCGRect(ntePrev.mediaCell.frame), self.nextLineTop);
							[ntePrev measureForContainingRect:CGRectMake(nte.frame.origin.x+nte.marginLeft,prevY,self.size.width - (nte.marginLeft+nte.marginRight), MAXFLOAT)];
							CGRect cellFrame = ntePrev.contentFrame;
							cellFrame.origin.x -= ntePrev.marginLeft;
							cellFrame.size.width += ntePrev.marginLeft + ntePrev.marginRight;
							cellFrame.origin.y -= ntePrev.marginTop;
							cellFrame.size.height += ntePrev.marginTop + ntePrev.marginBottom;
							ntePrev.frame = cellFrame;
						}
						growth += ntePrev.frame.size.height - currentHeight;
						 
					}
				}
				y+= growth;
				self.nextLineTop = y;
			}
			self.currentBottomNTE = y;
			self.nextNTEToPosition++;
		} else {
			break;
		}
	}
}


- (CGRect)lineFragmentRectForProposedRect:(CGRect)proposedRect atIndex:(NSUInteger)characterIndex writingDirection:(NSWritingDirection)baseWritingDirection remainingRect:(CGRect*)remainingRect {
	
	if (self.size.width <= 0) {
		*remainingRect = CGRectZero;
		return proposedRect;
	}

	// Start of a new pass, set all media elements' positionNeeded's to YES
	if (self.newLayoutPass) {
		self.newLayoutPass = NO;
		self.nextLineTop = proposedRect.origin.y;
		self.currentBottomNTE = proposedRect.origin.y;
		self.nextNTEToPosition = 0;
	}
	
	
	//NSLog(@"lineFrag %@ char=%d botText=%f botNTE=%F", NSStringFromCGRect(proposedRect), characterIndex, self.currentBottomText, self.currentBottomNTE);

	// We've probably advanced downwards since the previous call to this function
	if (proposedRect.origin.y > self.nextLineTop) {
		self.nextLineTop = proposedRect.origin.y;
	}

	// Position any NTEs that are not yet positioned and which occur before the current character index.
	[self positionMediaElementsThatOccurBeforeCharIndex:characterIndex];
	
	// Call super implementation to adjust to basic bounds
	proposedRect.origin.y = self.nextLineTop;
	CGRect resultRect = [super lineFragmentRectForProposedRect:proposedRect atIndex:characterIndex writingDirection:baseWritingDirection remainingRect:remainingRect];


	// Iterate over all positioned NTEs. If an non-full-width NTE intersects the proposed rect then we must
	// adjust the proposed rect for it. This assumes that NTEs will be iterated in text order.
	for (int i=0 ; i<self.nextNTEToPosition ; i++) {
		BNElementNonText* nte = self.nonTextElements[i];
		if (nte.normalizedWidth != 1.f) {
			CGRect nteFrame = nte.frame;
			nteFrame.origin.x -= nte.frameOffset.x;
			nteFrame.origin.y -= nte.frameOffset.y;
			if (CGRectIntersectsRect(nteFrame, resultRect)) {
				resultRect = [self adjustFragmentRectForHoleRect:resultRect holeRect:nteFrame remainingRect:remainingRect];
			}
		}
	}
	
	// Negative-width rects are meaningless and cause infinite loops in the typesetter
	if (resultRect.size.width < 0) {
		resultRect.size.width = 0;
	}
	
	//NSLog(@"lineFrag result=%@ rem=%@", NSStringFromCGRect(resultRect), NSStringFromCGRect(*remainingRect));
	self.nextLineTop = CGRectGetMinY(resultRect);
	return resultRect;
}





/**
 This useful helper method came from https://developer.apple.com/library/mac/qa/qa1687/_index.html
 */

- (CGRect)adjustFragmentRectForHoleRect:(CGRect)proposedRect holeRect:(CGRect)holeRect remainingRect:(CGRect *)remainingRect {

    // Adjust proposedRect for any intersection with holeRect, with any valid
    // non-clipped portion of proposedRect returned in remainingRect. The
    // following example assumes a movementDirection of NSLineMovesDown, and a
    // sweepDirection of NSLineSweepsRight. A full implementation should
    // support all NSLineSweepDirection and NSLineMovementDirection
    // possibilities.
	
    // This example assumes a 0,0 origin container
    CGRect containerRect = {{0,0}, self.size};
    CGRect adjustedProposedRect = proposedRect;
	
    // See if proposed rect overlaps at all with hole rect
    CGRect overlapRect = CGRectIntersection(adjustedProposedRect, holeRect);
    if (!CGRectEqualToRect(overlapRect, CGRectZero)) {
		
        CGFloat rightsideWidth;
		
        // Given our assumption of sweepDirection being NSLineSweepsRight, we
        // are only interested in adjusting our resultRect horizontally. Our
        // hole will either intersect with the left side of our rect (in which
        // case we want to move proposedRect's origin to the right side of the
        // hole) or the right side (in which case we want to clip proposedRect
        // against the left side of the hole and calculate the remainingRect
        // from the right side of the hole to the container rightmost bounds).
		
        if (adjustedProposedRect.origin.x >= holeRect.origin.x) {
            // Left side of adjustedProposedRect intersects with holeRect,
            // adjust adjustedProposedRect to right side of hole, no
            // remainingRect needed. We may need to adjust
            // adjustedProposedRect.width to reflect the smaller width (up to
            // the container rightmost bounds).
            adjustedProposedRect.origin.x = holeRect.origin.x + holeRect.size.width;
            rightsideWidth = self.size.width - (holeRect.origin.x + holeRect.size.width);
            if (adjustedProposedRect.size.width > rightsideWidth) {
                adjustedProposedRect.size.width = rightsideWidth;
            }
            // In this case there is no remainingRect -- adjustedProposedRect
            // will always either fit inside the portion of the container to the
            // right of the hole, or hit the edge of the container, meaning
            // there's no "remaining" space in the container for this line
            *remainingRect = CGRectZero;
        } else {
            // Right side (or middle) intersects with holeRect. Clip portion
            // that intersects with hole and calculate remainingRect if any.
            // remainingRect is whatever portion of adjustedProposedRect that
            // extends outside of hole -- in other words, whatever portion of
            // proposedRect that can still be used in a future call to
            // lineFragmentRectForProposedRect.
			
            rightsideWidth = adjustedProposedRect.size.width - ((holeRect.origin.x + holeRect.size.width) - adjustedProposedRect.origin.x);
            adjustedProposedRect.size.width = (holeRect.origin.x - adjustedProposedRect.origin.x);
			
            if (rightsideWidth > 0) {
                // Shift remainingRect to the "right" of hole, first clipping width against container bounds if necessary
                CGFloat widthBetweenHoleAndContainerBounds = (containerRect.origin.x + containerRect.size.width) - (holeRect.origin.x + holeRect.size.width);
                if (rightsideWidth > widthBetweenHoleAndContainerBounds) {
                    rightsideWidth = widthBetweenHoleAndContainerBounds;
                }
                *remainingRect = CGRectMake(holeRect.origin.x + holeRect.size.width,
                                            adjustedProposedRect.origin.y,
                                            rightsideWidth,
                                            adjustedProposedRect.size.height);
            } else {
                *remainingRect = CGRectZero;
            }
        }
    } else {
        // No intersection with hole, so ensure remainingRect is zero
        *remainingRect = CGRectZero;
    }
    return adjustedProposedRect;
}


- (void)layoutManagerDidInvalidateLayout:(NSLayoutManager *)sender {
//	self.newLayoutPass = YES;
}

- (void)dealloc {
	for (BNElementNonText* nte in self.nonTextElements) {
		[nte onTextKitDealloc];
	}
	self.nonTextElements = nil;
}


@end
#endif
