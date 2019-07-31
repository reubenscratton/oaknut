//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "statictitlemodule.h"
/*#import "BNStyles.h"
#import "BNContainerModule.h"
#import "BNCellsModule.h"
#import "BNStaticTitleModule.h"
#import "BNLinearLayout.h"
#import "BNAppDelegate.h"
#import "BBCNURLHandler.h"
*/


BNStaticTitleModule::BNStaticTitleModule(const variant& json) : BNModule(json) {
    _text = json.stringVal("title");
    _titleColor = [BNStyles parseColor:json[@"titleColor"]];
    _contentId = json.stringVal("contentLink");
    string styleName = json.stringVal("textAttrs");
    if (!styleName.size()) {
        styleName = "title";
    }
    _style = app.getStyle(styleName);
    _numLines = json.intVal("numLines");
}

BNStaticTitleModule::BNStaticTitleModule(BNStaticTitleModule* source) : BNModule(source) {
	_text = source->_text;
	_titleColor = source->_titleColor;
	_contentId = source->_contentId;
	_style = source->_style;
	_numLines = source->_numLines;
}

void BNStaticTitleModule::layoutWithContainingRect(const RECT& rect) override;

	// If module following this one is empty, then we occupy no space
	NSUInteger myIndex = [self.container.modules indexOfObject:self];
	if (myIndex < self.container.modules.count-1) {
		BNModule* nextModule = self.container.modules[myIndex+1];
		if ([nextModule isKindOfClass:[BNCellsModule class]]) {
			BNCellsModule* cellsModule = (BNCellsModule*)nextModule;
			if (cellsModule.cells.count == 0) {
				self.frame = CGRectZero;
				return;
			}
		} else if ([nextModule isKindOfClass:[BNStaticTitleModule class]]) {
			[nextModule layoutWithContainingRect:rect]; // slightly inefficient
			if (nextModule.frame.size.height == 0.f) {
				self.frame = CGRectZero;
				return;
			}
		} else if ([nextModule isKindOfClass:[BNLinearLayout class]]) {
			// Suggested (RS) fix for 'Your Questions Answered'
			BNLinearLayout *linearLayout = (BNLinearLayout*)nextModule;
			
			BNModule *module = [linearLayout.modules firstObject];
			
			if ([module isKindOfClass:[BNCellsModule class]]) {
				BNCellsModule* cellsModule = (BNCellsModule*)module;
				if (cellsModule.cells.count == 0) {
					self.frame = CGRectZero;
					return;
				}
			}
			
	
		}
	}

	rect = UIEdgeInsetsInsetRect(rect, self.padding);
	if (self.labelInfo == nil) {
		NSMutableDictionary* attrs = self.attrs.mutableCopy;
		if (self.titleColor != nil) {
			attrs[NSForegroundColorAttributeName] = self.titleColor;
		}
		self.labelInfo = [[BNLabelInfo alloc] initWithString:self.text attributes:attrs numLines:1];
		self.labelInfo.useFullWidth = YES;
		self.labelInfo.numLines = self.numLines;
	}
	rect = UIEdgeInsetsInsetRect(rect, self.textPadding);
	[self.labelInfo measureForWidth:rect.size.width offset:rect.origin];
	self.labelInfo.bounds = UIEdgeInsetsUninsetRect(self.labelInfo.bounds, self.textPadding);
	//rect.size.height = self.labelInfo.bounds.size.height;
	//self.frame = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height + self.padding.bottom);
	self.frame = UIEdgeInsetsUninsetRect(self.labelInfo.bounds, self.padding);
}

- (void)updateSubviews:(UIView *)superview {
	BOOL visible = CGRectIntersectsRect(superview.bounds, self.frame) && self.frame.size.height>0;
	if (visible == self.visible) {
		return;
	}
	self.visible = visible;
	if (visible && self.frame.size.height>0) {
		[self createLabelView:superview];
	} else {
		[self removeLabelView];
	}
}
- (void)removeAllViews {
	self.visible = NO;
	[self removeLabelView];
}

- (BOOL)showBackgroundForLinks {
	return YES;
}

- (void)createLabelView:(UIView*)superview {
	if (self.contentId) {
		[self.labelInfo createButton:superview];
		
		UIButton* button = ((UIButton*)self.labelInfo.label);
		if ([self showBackgroundForLinks]) {
			NSMutableAttributedString* s = button.titleLabel.attributedText.mutableCopy;
			//[s addAttribute:NSForegroundColorAttributeName value:[UIColor bbcNewsLiveRed] range:NSMakeRange(0, s.length)];
			[button setAttributedTitle:s forState:UIControlStateNormal];
			s = button.titleLabel.attributedText.mutableCopy;
			[s addAttribute:NSForegroundColorAttributeName value:[UIColor lightGrayColor] range:NSMakeRange(0, s.length)];
			[button setAttributedTitle:s forState:UIControlStateHighlighted];
		} else {			
			UIGraphicsBeginImageContext(CGSizeMake(1,1));
			CGContextSetFillColorWithColor(UIGraphicsGetCurrentContext(), [UIColor defaultSelectedFillColor].CGColor);
			CGContextFillRect(UIGraphicsGetCurrentContext(), CGRectMake(0,0,1,1));
			[button setBackgroundImage:UIGraphicsGetImageFromCurrentImageContext() forState:UIControlStateHighlighted];
			UIGraphicsEndImageContext();
		}
		
		[button addTarget:self action:@selector(onTitleTapped:) forControlEvents:UIControlEventTouchUpInside];
		
		[self.labelInfo.label setAccessibilityTraits:UIAccessibilityTraitHeader | UIAccessibilityTraitButton];

	} else {
		[self.labelInfo createLabel:superview];
		[self.labelInfo.label setAccessibilityTraits:UIAccessibilityTraitHeader];
	}
}

- (void)removeLabelView {
	[self.labelInfo removeLabel];
}

- (void)onTitleTapped:(id)sender {

	if ([self.contentId hasPrefix:@"/"]) {
		[[[BBCNURLHandler alloc] initWithURL:[BNAppDelegate URLforResourceSpec:self.contentId]] openURL:YES];
	} else {
		NSURL *url = [NSURL URLWithString:self.contentId];
		[[[BBCNURLHandler alloc] initWithURL:url] openURL:YES];
	}

}


