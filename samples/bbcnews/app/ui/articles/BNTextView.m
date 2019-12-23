#if 0
//
//  BNTextView.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNTextView.h"
#import "BNLayoutManager.h"
#import "BNTextContainer.h"
#import "BNElementText.h"


@interface BNTextViewAccessibilityElement : UIAccessibilityElement

@property (nonatomic) BNElement* element;

@end

@implementation BNTextViewAccessibilityElement

- (NSString*)accessibilityLabel {
	if ([self.element isKindOfClass:[BNElementText class]]) {
		BNElementText* elementText = (BNElementText*)self.element;
		return elementText.text;
	}
	return nil;
}

- (NSString *)accessibilityHint
{
	if ([self.element isKindOfClass:[BNElementText class]])
	{
		
		BNElementText* elementText = (BNElementText*)self.element;
		
		for(BNTextTrait *bt in elementText.traits)
		{
			if([bt isKindOfClass:[BNTextTraitLink class]])
			{
				BNTextTraitLink *tl = (BNTextTraitLink *)bt;
				
				if([tl.platform isEqualToString:@"newsapps"])
				{
					return [NSString stringWithFormat:@"Double tap to open story"];
				} else {
					return [NSString stringWithFormat:@"Double tap to open %@ in a browser", [tl.url host]];
				}
			}
		}
	}
	return [super accessibilityHint];
}

- (UIAccessibilityTraits)accessibilityTraits
{
	if ([self.element isKindOfClass:[BNElementText class]])
	{
		
		BNElementText* elementText = (BNElementText*)self.element;
		
		for(BNTextTrait *bt in elementText.traits)
		{
			if([bt isKindOfClass:[BNTextTraitLink class]])
			{
				BNTextTraitLink *tl = (BNTextTraitLink *)bt;
				
				if([tl.platform isEqualToString:@"newsapps"])
				{
					return UIAccessibilityTraitButton;
				} else {
					return UIAccessibilityTraitLink;
				}
			}
		}
	}
	return [super accessibilityTraits];

}

- (BOOL)isAccessibilityElement {
	if ([self.element isKindOfClass:[BNElementText class]]) {
		return YES;
	}
	if ([self.element isKindOfClass:[BNElementMedia class]]) {
		BNElementMedia* elementMedia = (BNElementMedia*)self.element;
		if (elementMedia.view) {
			return YES;
		}
	}
	return NO;
}

- (CGRect)accessibilityFrame {
	BNTextView* textView = self.accessibilityContainer;
	if ([self.element isKindOfClass:[BNElementText class]]) {
		BNElementText* elementText = (BNElementText*)self.element;
		CGRect rect = [textView.layoutManager boundingRectForGlyphRange:NSMakeRange(self.element.charIndex, elementText.text.length) inTextContainer:textView.textContainer];
		return [textView convertRect:rect toView:textView.window];
	}
	if ([self.element isKindOfClass:[BNElementMedia class]]) {
		BNElementMedia* elementMedia = (BNElementMedia*)self.element;
		if (elementMedia.view) {
			return [elementMedia.view convertRect:elementMedia.view.bounds toView:textView.window];
		}
	}
	return CGRectMake(0,0,textView.frame.size.width,10);
}

@end

@interface BNTextView ()

@property (nonatomic) NSMutableArray* accessibilityElements;

@end

@implementation BNTextView

- (void)setContentOffset:(CGPoint)contentOffset {
	
	// God only knows what UIKit is doing here but this little test stops the contentOffset going wrong
	// when coming out of fullscreen video on iOS 8.x. Some weird interplay with our TextKit use, no doubt.
	// NB: iPads seem unaffected and to not need this ultraweird hack.
	int y = (int)(contentOffset.y*1000);
	if (y == 34111) { // iPhone 4
		return;
	}
	if (y == 33697) { // iPhone 5
		return;
	}
	if (y == 33702) { // iPhone 5S
		return;
	}
	if (y == 34067) { // iPhone 6
		return;
	}
	if (y == 34071) { // iPhone 6+
		return;
	}
	
	[super setContentOffset:contentOffset];
}

- (instancetype)initWithFrame:(CGRect)frame {
	
	// TextKit setup
	BNLayoutManager* layoutManager = [[BNLayoutManager alloc] init];
	NSTextStorage* storage = [[NSTextStorage alloc] init];
	[storage addLayoutManager:layoutManager];
	BNTextContainer *container = [[BNTextContainer alloc] initWithSize:CGSizeMake(frame.size.width, CGFLOAT_MAX)];
	//layoutManager.delegate = container;
	[layoutManager addTextContainer:container];
	
	// Have to use this initializer for custom TextKit fun
	self = [super initWithFrame:frame textContainer:container];
	if (self) {
		container.widthTracksTextView = NO;
		container.heightTracksTextView = NO;
		self.translatesAutoresizingMaskIntoConstraints = NO;
		self.autoresizesSubviews = YES;
		self.editable = NO;
		self.selectable = YES;
		self.textContainerInset = UIEdgeInsetsZero;
		self.userInteractionEnabled = YES;
		self.scrollsToTop = NO;
	}
	return self;
}


- (CGSize)sizeThatFits:(CGSize)size {
	
	// This bit triggers the measuring (not the positioning) of all NTEs, and then runs
	// a text layout pass which positions the NTEs as it goes.
	NSRange range = NSMakeRange(0,self.textStorage.length);
	[self.layoutManager invalidateGlyphsForCharacterRange:range changeInLength:0 actualCharacterRange:nil];
	[self.layoutManager invalidateLayoutForCharacterRange:range actualCharacterRange:nil];
	BNTextContainer *container = (BNTextContainer*)self.textContainer;
	container.size = CGSizeMake(size.width, CGFLOAT_MAX);
	
	// Layout all done, get the bounding rect
	CGRect rect = [self.layoutManager boundingRectForGlyphRange:range inTextContainer:container];
	rect.size.width = size.width;
	rect.size.height = container.measuredHeight;
	
	[self setContentSize:rect.size];
	return rect.size;
}

- (void)setAttributedText:(NSAttributedString *)attributedText andNonTextElements:(NSArray*)nonTextElements {
	BNTextContainer *container = (BNTextContainer*)self.textContainer;
	container.nonTextElements = nonTextElements;
	NSAttributedString *text = [BNStyles applyDynamicTextSize:attributedText];
	[self invalidateIntrinsicContentSize];
	[self setNeedsLayout];
	[self setNeedsDisplay];
	[self setAttributedText:text];	
	[self.layoutManager invalidateDisplayForGlyphRange:NSMakeRange(0,text.length)];
}




- (BOOL)canBecomeFirstResponder {
	return YES;
}


- (BOOL)canPerformAction:(SEL)action withSender:(id)sender {
    if (action == @selector(select:)) {
        return YES;
    } else if (action == @selector(copy:)) {
        return YES;
    } else if (action == @selector(selectAll:)) {
		return YES;
    }
    return [super canPerformAction:action withSender:sender];
}

- (void)copy:(id)sender {
	
	UIPasteboard *board = [UIPasteboard generalPasteboard];
	UITextRange *textRange = [self selectedTextRange];
	
	NSString *text = [self textInRange:textRange];
	
	[board setString:text];
	//self.highlighted = NO;
	[self resignFirstResponder];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    return NO;
}

- (void)selectAll:(id)sender {
	[self setSelectedRange:NSMakeRange(0, self.text.length)];
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    CGPoint touchLocation = [[touches anyObject] locationInView:self];
	BNLayoutManager* layoutManager = (BNLayoutManager*)self.layoutManager;
	if ([layoutManager touchesBegan:touchLocation textContainer:self.textContainer]) {
		return;
	}
	[super touchesBegan:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	BNLayoutManager* layoutManager = (BNLayoutManager*)self.layoutManager;
	[layoutManager touchesCancelled];
	[super touchesCancelled:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	BNLayoutManager* layoutManager = (BNLayoutManager*)self.layoutManager;
	[layoutManager touchesEnded];
	[super touchesEnded:touches withEvent:event];
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer {
	
	// Disable UITextView's built-in pan detection so it doesn't interfere with parent scrollviews
	if (gestureRecognizer.view == self && [NSStringFromClass(gestureRecognizer.class) isEqualToString:@"UIScrollViewPanGestureRecognizer"]) {
		return NO;
	}
	return YES;
}




- (void)didMoveToWindow {
	[super didMoveToWindow];
	
	// Re-apply the textview transform when becoming active. If we don't do this the textview
	// goes wrong after they return from another VC, presumably thanks to iOS resetting transforms.
	if (self.window) {
		self.layer.transform = CATransform3DMakeTranslation(0, self.contentOffset.y, 0);
	}

}






/*
 Accessibility
 */

- (BOOL)isAccessibilityElement {
	return NO;
}

- (NSInteger)accessibilityElementCount {
	if (!self.accessibilityElements) {
		self.accessibilityElements = [[NSMutableArray alloc] initWithCapacity:self.item.elements.count];
		for (BNElement* element in self.item.elements) {
			if ([element isKindOfClass:[BNElementText class]]) { // TODO: support media elements
				BNTextViewAccessibilityElement* tvae = [[BNTextViewAccessibilityElement alloc] initWithAccessibilityContainer:self];
				tvae.element = element;
				[self.accessibilityElements addObject:tvae];
			}
		}
	}
	return self.accessibilityElements.count;
}
- (id)accessibilityElementAtIndex:(NSInteger)index {
	return self.accessibilityElements[index];
}
- (NSInteger)indexOfAccessibilityElement:(id)element {
	return [self.accessibilityElements indexOfObject:element];
}

@end
#endif
