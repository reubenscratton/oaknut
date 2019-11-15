//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "scrollablecontainer.h"
/*#import "BNContentRequest.h"
#import "BNContentView.h"
#import "BNNavigationController.h"
#import "BNStyles.h"
#import "BNVerticalStack.h"
#import "BNCellContentView.h"

#import "BNAManager.h"
*/

/*@interface BNContainerModuleScrollView : UIScrollView <UIScrollViewDelegate>

@property (nonatomic, weak) BNScrollableContainer* containerModule;
@property (nonatomic) BOOL beingRemovedFromSuperview;

- (id)initWithContainer:(BNScrollableContainer*)containerModule andFrame:(CGRect)frame;

@end
*/

/*
- (void)setFrame:(CGRect)frame {
	[super setFrame:frame];
	if (self.scrollView != nil) {
		self.scrollView.contentSize = frame.size;
	}
}*/

BNScrollableContainer::BNScrollableContainer(const variant& json) : BNContainerModule(json) {
}


BNScrollableContainer::BNScrollableContainer(BNScrollableContainer* source) : BNContainerModule(source) {
}
BNModule* BNScrollableContainer::clone() {
    return new BNScrollableContainer(this);
}

void BNScrollableContainer::updateSubviewsForModules(vector<BNModule*> modules, View* superview) {

	bool visible = superview->getOwnRect().intersects(_frame);
	
	if (visible && !_scrollView) {
		
		// The scrollview's visible size (as opposed to its content size) is the intersection
		// of superview and bounds rect when the origins are the same.
		RECT scrollViewRect;
		scrollViewRect.origin = _frame.origin;
		if (_container) {
			scrollViewRect.size.width = MIN(_frame.size.width, superview->getWidth());
			scrollViewRect.size.height = MIN(_frame.size.height, superview->getHeight());
		} else {
			scrollViewRect.size = superview->getRect().size;
		}
		
		// Create a scrollview to hold the container's content
        _scrollView = new View();
        _scrollView->setRect(scrollViewRect);
        _scrollView->setLayoutSize(MEASURESPEC::Abs(scrollViewRect.size.width), MEASURESPEC::Abs(scrollViewRect.size.height));
        _scrollView->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
		/*BNContainerModuleScrollView* scrollView = [[BNContainerModuleScrollView alloc] initWithContainer:self andFrame:scrollViewRect];
		self.scrollView = scrollView;
		scrollView.contentSize = self.frame.size;
		scrollView.backgroundColor = self.backgroundColor;
		if (!self.container || (!self.container.container && ![self.container isKindOfClass:[BNScrollableContainer class]])) { // if root container, inset by navbar
			CGFloat topInset = navbarHeight + self.topInset;
			CGFloat bottomInset = 0;
			if([BBCNFeatureController tabBarEnabled]){
				//bottomInset = 49;
			}
			scrollView.contentInset = UIEdgeInsetsMake(topInset, 0, bottomInset, 0);
			scrollView.scrollIndicatorInsets = UIEdgeInsetsMake(topInset, 0, bottomInset, 0);
			scrollView.contentOffset = CGPointMake(0,-topInset);
			[self updateScrollsToTop];
		}*/
		superview->addSubview(_scrollView);
	}
	
	if (_scrollView) {
        for (BNModule* submodule :_modules) {
			submodule->updateSubviews(_scrollView);
		}
	}
	
	if (!visible && _scrollView) {
        removeAllViews();
	}
}

/*- (void)removeAllViews {
	[super removeAllViews];
	[self.scrollView removeFromSuperview];
	self.scrollView = nil;
}

- (CGFloat)getState {
	return self.scrollView.contentOffset.y;
}
- (void)applyState:(CGFloat)state {
	CGFloat max = self.scrollView.contentSize.height - self.scrollView.bounds.size.height;
	if (max > 0) {
		state = MIN(state, max);
	}
	self.scrollView.contentOffset = CGPointMake(0,state);
}

- (void)setIsOnScreen:(BOOL)isOnScreen {
	[super setIsOnScreen:isOnScreen];
	[self updateScrollsToTop];
}

*/

void BNScrollableContainer::updateScrollsToTop() {
    app->log("TODO: updateScrollsToTop");
    //self.scrollView.scrollsToTop = NO;
}

/*

@implementation BNContainerModuleScrollView


- (id)initWithContainer:(BNScrollableContainer*)containerModule andFrame:(CGRect)frame {
	self = [super initWithFrame:frame];
	if (self) {
		self.containerModule = containerModule;
		if ([containerModule isKindOfClass:[BNVerticalStack class]]) {
			self.delegate = self;
		}
		self.scrollsToTop = NO;
	}
	return self;
}


- (void)setContentOffset:(CGPoint)contentOffset {
	CGFloat dy = contentOffset.y - self.contentOffset.y;
	
	[super setContentOffset:contentOffset];
	
	if (self.window) {
	//if (self.isDragging) {
		if (self.contentSize.height > self.bounds.size.height) {
			[[BNNavigationController get] onScrollVertically:dy inScrollView:self];
		}
	}
	if (!self.beingRemovedFromSuperview) {
		if (self.window != nil) {
			[self.containerModule updateSubviews:self.superview];
		}
	}
}

// addSubview and insertSubview are overridden to stop adding views above the scroll indicator imageviews
// because it looks weird and wrong.
- (void)addSubview:(UIView *)view {
	NSUInteger c = self.subviews.count;
	if (c >= 2) {
		[self insertSubview:view atIndex:c-2];
	} else {
		[super addSubview:view];
	}
}

- (void)insertSubview:(UIView *)view atIndex:(NSInteger)index {
	NSUInteger c = self.subviews.count;
	if (index > c-2) {
		index = c-2;
	}
	[super insertSubview:view atIndex:index];
}

- (void)removeFromSuperview {
	id __attribute__((unused)) s = self; // works around an ARC bug. Remove this and crashy crashy when paging
	self.beingRemovedFromSuperview = YES;
	[super removeFromSuperview];
	self.beingRemovedFromSuperview = NO;
}

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView {
	if (scrollView.contentSize.height > scrollView.bounds.size.height) {
		[[BNNavigationController get] resetVerticalScrollTracking];
	}
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate {
	if (scrollView.contentSize.height > scrollView.bounds.size.height) {
		[[BNNavigationController get] onVerticalScrollEnded:scrollView];
	}
	
}

- (BOOL)scrollViewShouldScrollToTop:(UIScrollView *)scrollView {
	BOOL should = self.scrollsToTop && self.contentOffset.y > 0;
	if (should) {
		[BNNavigationController get].navBarHidden = NO;
	}
	return should;
}


@end
*/

