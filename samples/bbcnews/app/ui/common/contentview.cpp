//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "contentview.h"
#include "layoutsgroupmanager.h"
#include "label.h"
/*
#import "BNHUDView.h"
#import "BNCellItem.h"
#import "BNSharingHelper.h"
#import "BNMetrics.h"
#import "BNElementText.h"
#import "BNVerticalStack.h"
#import "BNCellMedia.h"
#import "BNNavigationController.h"
#import "BNWalkThroughStatsControl.h"
#import "BNAManager.h"
#import "BNAAnalyticObject.h"
#import "BNLiveEvent.h"
#import "BNCommentaryModule.h"
#import "BNContentViewPagerController.h"
#import "BNTabBarController.h"
#import "BNFollowScreenView.h"
#import "BNContent.h"
#import "BNContentStub.h"
*/

/*
static NSDictionary* s_placeholderImagesDictionary;
static UIImage* s_placeholderImageDefault;

+ (void)load {
    UIImage* imageItem = [UIImage imageNamed:@"PlaceholderItem"];
    UIImage* imageIDX = [UIImage imageNamed:@"PlaceholderCollection"];
    UIImage* imageMAP = [UIImage imageNamed:@"PlaceholderMediaItem"];
    UIImage* imageLEP = [UIImage imageNamed:@"PlaceholderLiveEvent"];
    UIImage* imagePGL = [UIImage imageNamed:@"PlaceholderPhotoGallery"];
    s_placeholderImagesDictionary = @{
        BNContentFormat.textual : imageItem,
        BNContentFormat.index : imageIDX,
        BNContentFormat.audio : imageMAP,
        BNContentFormat.video : imageMAP,
        BNContentFormat.photoGallery : imagePGL,
        BNContentFormat.liveevent : imageLEP
    };
    s_placeholderImageDefault = imageIDX;
    
}
*/


DECLARE_DYNCREATE(BNContentView);


BNContentView::BNContentView() : View(), _contentStub("","") {
    _directionalLockEnabled = true;
    // TODO
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onNewLayouts:) name:BNNewLayoutsNotification object:nil];
}

/*- (void)onNewLayouts:(NSNotification*)notification {
    self.currentLayout = nil;
    self.layout = nil;
    self.cachedFrame = CGRectZero;
    [self setNeedsLayout];
}*/

void BNContentView::setContentStub(const BNContent::stub& stub, bool onlyShowPlaceholder/*=false*/) {
    
    // Reset layout
    _cachedFrame = RECT::zero();
    setCurrentLayout(NULL);
    setNeedsLayout();
    _onlyShowPlaceholder = onlyShowPlaceholder;
    _hasRecordedView = false;
    
    requestContent(false);
    _contentStub = stub;
    if (_window) {
        requestContent(true);
    }
}

void BNContentView::requestContent(bool request) {
    if (!request && _req) {
        _req->cancel();
        _req = NULL;
    }
    
    if (request && !_onlyShowPlaceholder) {
        _req = BNURLRequest::requestContent(_contentStub.modelId, 0, _requestPriority);
        _req->onHandleContent = [=](BNBaseModel* object) {
            if (!object->isContent()) {
                app->warn("BNContentView.onRequestLoadedObject received non-BNContent type, ignoring");
                return;
            }
            updateWithNewContentObject((BNContent*)object);
        };
    }
}

void BNContentView::updateWithNewContentObject(BNContent* content) {
    _pendingContentObject = nullptr;
    _contentObject = content;
    _currentLayout = nullptr;
    
    removeAllSubviews();

    if (_emptyView) {
        addSubview(_emptyView);
    }
    
    _layoutValid = true; // TODO: it is retarded that this is necessary. Fix the API.
    setNeedsLayout();
    
    if (_isOnScreen) {
        //[self.owningViewController invalidateNav];
        
        //conditionallyRecordContentViewEvent();
    }
    
}

/*
void BNContentView::onRequestLoadedObject(BNURLRequest* request, BNBaseModel* object, bool isCacheData) {
    
 
    // If the cache is returning the exact same object we already have, ignore the event. The
    // exception to the rule is My News which is always the same object but whose contents change.
    if (object == _contentObject && ![self.contentStub.modelId hasPrefix:modelIdMyNews]) {
        [self endRefreshing];
        return;
    }
    
    if (!isCacheData) {
        self.hasARequestError = NO;
    }
    
    
    // Ignore cached data if we've already got it
    if (isCacheData && self.contentObject) {
        return;
    }
    
    // If it's a VIVO Live Event page then panic
    BNContent* content = (BNContent*)object;
    if ([content.modelType isEqualToString:BNModelType.liveEventExt]) {
        [self.owningViewController onCannotRenderContentPleaseShowInBrowserKThxBai:content.shareUrl];
        return;
    }
    
    // If user has scrolled down a bit, don't just update but show an ambient notification instead.
    if (self.pendingContentObject) {
        self.pendingContentObject = object;
        return;
    }
    if (self.contentObject && self.scrollView.contentOffset.y > 64  && self.isOnScreen && self.window)
    {
        self.pendingContentObject = object;
        BNAmbientNotification* ambientNotification = [[BNAmbientNotification alloc] initWithText:@"This page has updated. Tap here to show the updated page."
                                                                                      buttonText:@"X"
                                                                                      andTimeout:-1];
        ambientNotification.dismissedBlock = ^() {
            [self updateWithNewContentObject:self.pendingContentObject];
            [BNNavigationController get].navBarHidden = NO;
        };
        [[BNNavigationController get] showAmbientNotification:ambientNotification];
        
        return;
    }
    
    [self updateWithNewContentObject:object];
}
*/
void BNContentView::setCurrentLayout(BNLayout* layout) {
	if (_currentLayout) {
		_currentLayout->_rootContainer->cleanup();
	}
	_currentLayout = layout;
}

void BNContentView::attachToWindow(Window *window) {
    View::attachToWindow(window);
    requestContent(true);
    /*if (!self.onlyShowPlaceholder) {
        if (self.superview) {
            //NSLog(@"Page request for %@", self.contentStub.modelId);
            [BNContentRequest request:self.contentStub.modelId delegate:self flags:0 priority:self.requestPriority];
        } else {
            //NSLog(@"Page unrequest for %@", self.contentStub.modelId);
            [BNContentRequest unrequest:self.contentStub.modelId delegate:self];
            [self.currentLayout.rootContainer cleanup];
        }
    }*/
}
void BNContentView::detachFromWindow() {
    View::detachFromWindow();
    requestContent(false);
}
void BNContentView::layout(RECT constraint) {

    // Show loading screen + HUD if got no content
    if (!_contentObject && !_onlyShowPlaceholder) {
        if (!_loadingImageView) {
            _loadingImageView = new ImageView();
            _loadingImageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
            addSubview(_loadingImageView);
        }
        
        // TODO: Show an appropriate placeholder image
        /*if (_contentStub.format.length()) {
            _loadingImageView->setImage ( s_placeholderImagesDictionary[self.contentStub.format]);
        }
        if (!_loadingImageView.image) {
            self.loadingImageView.image = s_placeholderImageDefault;
        }*/
        
        // TODO: HUD
        /*if (!self.onlyShowPlaceholder && !self.hasARequestError) {
            if (internetStatus != NotReachable) {
                if (!self.loadingHUD) {
                    self.loadingHUD = [[BNHUDView alloc] initWithFrame:CGRectMake(0.0f, 0.0f, 100.0f, 100.0f)];
                    [self addSubview:self.loadingHUD];
                }
            } else {
                if (self.loadingHUD) {
                    [self.loadingHUD removeFromSuperview];
                    self.loadingHUD = nil;
                }
            }
        }
        
        
        // Position loading hud in center
        CGRect frame = self.loadingHUD.frame;
        frame.origin.x = (self.frame.size.width - frame.size.width) / 2;
        frame.origin.y = (self.frame.size.height - frame.size.height) / 2;
        self.loadingHUD.frame = frame;*/
        return;
    }
    
    // If we're only showing the placeholder image (i.e. page is an intermediate
    // one during a fast-scroll) then exit now
    if (_onlyShowPlaceholder) {
        return;
    }
    
    // Hide loading screen since we have content
    if (_loadingImageView && _loadingImageView->getWindow()) {
        _loadingImageView->removeFromParent();
        _loadingImageView = NULL;
    }
    /*TODO if (_loadingHUD) {
        _loadingHUD->removeFromParent();
        _loadingHUD = NULL;
    }*/

    
    // Ensure we have a layout object
    if (!_currentLayout) {
        if (_layout) {
            _currentLayout = _layout;
        } else {
            bool portrait = _window->getHeight() >= _window->getWidth();
            string orientationString =  portrait ? "portrait"_S : "landscape"_S;
            _currentLayout = ((BBCNewsApp*)app)->layoutWithContent(_contentStub.modelId,
                                                                 _contentObject->_site,
                                                               _contentObject->_format,
                                                              orientationString);
            assert(_currentLayout);//, "Missing layout! Content=%s", _contentStub.url());
        }
        
        // Recreate cells
        setBackgroundColor(_currentLayout->_rootContainer->getBackgroundColor());
        _currentLayout->_rootContainer->_moduleHolder = this;
        _currentLayout->_rootContainer->setIsOnScreen(_isOnScreen);
        _currentLayout->_rootContainer->_topInset = _topInset;
        _currentLayout->_rootContainer->_displayTitle = _contentStub.name;
        _currentLayout->_rootContainer->updateLayoutWithContentObject(_contentObject);
        _cachedFrame = RECT::zero();
        _currentLayout->_rootContainer->addToView(this);


        // Copyright
        BNLabel* label = new BNLabel();
        label->applyStyle("copyright");
        label->setText("Copyright © 2019 BBC.");
        _subviews[0]->addSubview(label);
    }
    
    View::layout(constraint);

    /*
    // Only do this if the bounds changed
    if (!_cachedFrame.equal(_rect)) {
        _cachedFrame = _rect; // TODO: bounds?
        
        if (_contentObject->isEmpty()) {
            if (_currentLayout->_emptyView.length() && !_emptyView) {
                app->warn("TODO: inflate empty view");
                //UINib* nib = [UINib nibWithNibName:self.currentLayout.emptyView bundle:nil];
                //self.emptyView = [[nib instantiateWithOwner:nil options:nil] firstObject];
                //CGFloat topInset = self.topInset + navbarHeight;
                //CGRect frame = self.bounds;
                //frame.origin.y = topInset;
                //frame.size.height -= topInset;
                //self.emptyView.frame = frame;
                //self.emptyView.center = CGPointMake(self.frame.size.width/2,  topInset + frame.size.height/2);
                //[self addSubview:self.emptyView];
            }
        } else {
            if (_emptyView) {
                _emptyView->removeFromParent();
                _emptyView = NULL;
            }
        }
        
        // Update the layout (i.e. recreate, measure and position all cells).
        if (!_emptyView) {
            RECT bounds = getOwnRect();
            bounds.origin.y = 0;
            _currentLayout->_rootContainer->layoutWithContainingRect(bounds);
            
            // Show the visible part of the content
            _currentLayout->_rootContainer->updateSubviews(this);
            setNeedsLayout();
            
            // Keep a ref. on the root scrollview
            if (_currentLayout->_rootContainer->isScrollableContainer()) {
                _scrollView = _currentLayout->_rootContainer.as<BNScrollableContainer>()->_scrollView;
            }
            
            if (_savedScrollOffset != 0.f) {
                _currentLayout->_rootContainer->applyState(_savedScrollOffset);
                _savedScrollOffset = 0;
            }
            
            // Initial selection (for autoplaying MAP media)
            if (_isInitialContent) {
                _currentLayout->_rootContainer->onIsInitialContent();
                _isInitialContent = false;
            }
        }
        
    }*/


}

void BNContentView::presentEmptyView() {
	//no content
/*	CGFloat topInset = self.topInset + navbarHeight;
	CGRect frame = self.bounds;
	frame.origin.y = topInset;
	frame.size.height -= topInset;
	
	self.scrollView = [[UIScrollView alloc] initWithFrame:frame];
	self.scrollView.backgroundColor = [UIColor whiteColor];
	self.scrollView.opaque = YES;
	self.scrollView.showsVerticalScrollIndicator = YES;
	self.scrollView.userInteractionEnabled = YES;
	
	UINib* nib = [UINib nibWithNibName:@"BNNoContentEmptyViewController" bundle:nil];
	UIView *placeView = [[nib instantiateWithOwner:nil options:nil] firstObject];
	placeView.frame = CGRectMake(0, 0, frame.size.width, frame.size.height);
	placeView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
	
	[self.scrollView addSubview:placeView];
	[self addSubview:self.scrollView];
	frame.size.height +=10;
	self.scrollView.contentSize = frame.size;
 */
}

/*
- (void)onRequestStatusChanged:(BNURLRequest*)request {
	//NSLog(@"cv:%@ req.status=%@ %@", self.contentStub.modelId, @(request.status),request.response);
	if (request.status == kStatusInactive) {
		[self endRefreshing];
	
		if (self.contentObject == nil || [self.contentObject isEmpty])  {
			[self presentEmptyView];
		}
	}
	[self setNeedsLayout];
}




- (void)conditionallyRecordContentViewEvent {
	if (self.isOnScreen && self.contentObject && !self.hasRecordedView) {
		
		self.hasRecordedView = YES;

		if ([self.contentObject isKindOfClass:[BNItem class]])
		{
			[[BNWalkThroughStatsControl sharedInstance] shownAnArticle];
		}
		
		[BNMetrics recordContentViewed:self.contentObject];
		
		[self recordAnalyticForPageView];
	}
}


-(void)recordAnalyticForPageView{
	BNAAnalyticObject *bo = [[BNAAnalyticObject alloc] init];
	bo.pageCounterName = self.contentObject.iStatsCounterName;
	
		
	if(self.contentObject.pageType == nil){
		
		if([self.contentStub.modelId isEqualToString:modelIdMostRead]){
			bo.pageType = BNAEventCPSPageTypeValueIndex;
			bo.pageCounterName = BNACounterNameValueMostRead;
		}
		else if ([self.contentStub.modelId isEqualToString:modelIdMostWatched]){
			bo.pageType = BNAEventCPSPageTypeValueIndex;
			bo.pageCounterName = BNACounterNameValueMostWatched;
		}
		else if ([self.contentStub.modelId isEqualToString:modelIdMostPopular]){
			bo.pageType = BNAEventCPSPageTypeValueIndex;
			bo.pageCounterName = BNACounterNameValueMostPopular;
		}
		else if ([self.contentObject.modelId hasPrefix:modelIdMyNews]){
			bo.pageType = BNAEventCPSPageTypeValueMyNews;
			bo.pageCounterName =BNACounterNameValueMyNewsByTopic;
			if([[NSUserDefaults standardUserDefaults] integerForKey:@"myNewsTabIndex"] == 0 ){
				bo.pageCounterName = BNACounterNameValueMyNewsByTime;
			}
		}
		else{
			bo.pageType = BNAEventCPSPageTypeValueIndex;
		}
	}
	else{
		bo.pageType = self.contentObject.pageType;
	}
	
	if(self.contentObject.cpsAssetId){
		bo.pageAssetId = self.contentObject.cpsAssetId;
	}
	
	if(self.contentObject.nation){
		bo.pageNation = self.contentObject.nation;
	}
	
	bo.pageFormat = self.contentObject.format;
	
	if ([self.contentObject isKindOfClass:[BNCollection class]])
	{
		bo.pageIsFollowed = [BNCollections isFollowed:self.contentObject.stub]; //only for indexes
	}
	
	bo.pageLanguage = self.contentObject.language;
	bo.pageName = self.contentObject.name;
	
	[[BNAManager sharedInstance] logPageView:bo];
}

- (void)onRequestError:(BNURLRequest*)request error:(NSError*)error httpStatus:(NSInteger)httpStatus {
	
	// If we already have a valid content object AND this wasn't pull-to-refresh, then
	// no need to show an error.
	if (self.contentObject && !(request.flags & BNURLRequestFlagForceUpdate)) {
		return;
	}
	
	// Similarly if there is *no* current content object AND this wasn't pull-to-refresh AND it was
	// a network timeout or other fail rather than a proper HTTP error, then don't show anything
	// because the empty view is explanation enough.
	if (!self.contentObject && !(request.flags & BNURLRequestFlagForceUpdate) && 0==httpStatus) {
		return;
	}
	
	

	// Default error message
	self.hasARequestError = YES;
	NSString *message = @"Unable to display content";
	BOOL dismissable = YES;

	// All network failures will have a zero HTTP status code
	if (httpStatus == 0) {
		message = @"Please check your internet connection and try again later";
		dismissable = NO;
	}
	
	// Context-aware error messages for HTTP errors
	if(httpStatus >=400){
		if([self.contentStub.format isEqualToString:BNContentFormat.index]){
			message = @"We were unable to retrieve the latest news stories for this page";
		} else if ([self.contentStub.format isEqualToString:BNContentFormat.liveevent]){
			message = @"Unable to display live updates";
		}
	}

	// Show the error message
	BNAmbientNotification* an = [[BNAmbientNotification alloc] initWithText:message
																 buttonText:dismissable?@"X":nil
																 andTimeout:dismissable?20:5];
	
	[[BNNavigationController get] showAmbientNotification:an];

	if (self.loadingHUD) {
		[self.loadingHUD removeFromSuperview];
		self.loadingHUD = nil;
	}
}




- (void)didMoveToWindow {

	// Listen or stop listening to dynamic text size changes
	if (self.window) {
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didChangePreferredContentSize:)
													 name:UIContentSizeCategoryDidChangeNotification
												   object:nil];
	} else {
		[[NSNotificationCenter defaultCenter] removeObserver:self
														name:UIContentSizeCategoryDidChangeNotification
													  object:nil];
		}
	}

- (void)didChangePreferredContentSize:(NSNotification*)notification {
	self.cachedFrame = CGRectZero;
	self.currentLayout = nil;
	[self setNeedsLayout];
}

- (void)refreshCommentaryModule:(BNModule*)module {
	if ([module isKindOfClass:[BNCommentaryModule class]]) {
		[((BNCommentaryModule*)module) forceRefresh];
		return;
	}
	if ([module isKindOfClass:[BNContainerModule class]]) {
		BNContainerModule* container = (BNContainerModule*)module;
		for (BNModule* module in container.modules) {
			[self refreshCommentaryModule:module];
		}
	}
}


- (void)endRefreshing {

	if (self.refreshControl.isRefreshing) {
		[self.refreshControl endRefreshing];
	}
}

- (void)safeReleaseRefreshControl {
	
	// From crash reports in HockeyApp it seems that iOS can dispatch setRefreshControl messages to
	// dealloc'd UIRefreshControls, crashing the app with a SIGSEGV. Here we use the dispatch queue
	// to keep our UIRefreshControls alive a little longer.
	if (self.refreshControl) {
		[self.refreshControl removeTarget:self action:@selector(onUserRefreshed:) forControlEvents:UIControlEventValueChanged];
		__block UIRefreshControl* ref = self.refreshControl;
		dispatch_async(dispatch_get_main_queue(), ^{
			if ([ref isRefreshing]) {
				[ref endRefreshing];
			}
			[ref removeFromSuperview];
		});
		self.refreshControl = nil;
	}
}

- (void)dealloc {
	[self endRefreshing];
	[self safeReleaseRefreshControl];
	//NSLog(@"BNContentView dealloc %@", self.contentStub.modelId);
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}



-(void)pinched:(UIGestureRecognizer *)recog{
	
	if(recog.state == UIGestureRecognizerStateEnded){
		[BNLayoutsGroupManager get].compactMode = ![BNLayoutsGroupManager get].compactMode;
		
		self.cachedFrame = CGRectZero;
		self.currentLayout = nil;
		[self setNeedsLayout];
	}
}
*/

/*- (void)setScrollView:(UIScrollView *)scrollView {
	if (scrollView == _scrollView) {
		return;
	}
	
	// Cleanup existing refresh control
	[self endRefreshing];
	[self safeReleaseRefreshControl];
	if (_scrollView) {
		[_scrollView removeFromSuperview];
	}
	
	// Create new refresh control and add to scrollview
	_scrollView = scrollView;
	if (scrollView) {
	scrollView.alwaysBounceVertical = YES;
	self.refreshControl = [[UIRefreshControl alloc] initWithFrame:CGRectZero];
	[self.refreshControl addTarget:self action:@selector(onUserRefreshed:) forControlEvents:UIControlEventValueChanged];
	[scrollView addSubview:self.refreshControl];
}
}

- (void)onUserRefreshed:(id)sender {
	
	// NEWSAPPS-4435
	if (internetStatus == NotReachable) {
		[self endRefreshing];
		NSString *message = @"Please check your internet connection and try again later";
		[[BNNavigationController get] showSimpleAmbientNotification:message timeout:3.5];
		return;
	}
	
	[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromPullRefresh];
	[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeDrag name:BNAActionNameRefresh labels:nil];
	
	[BNContentRequest unrequest:self.contentStub.modelId delegate:self];
	[BNContentRequest request:self.contentStub.modelId delegate:self flags:BNURLRequestFlagForceUpdate priority:BNDownloadPriorityHigh];
	
	// If it's a live event page, force-update the commentary too
	if ([self.contentObject isKindOfClass:[BNLiveEvent class]]) {
		[self refreshCommentaryModule:self.currentLayout.rootContainer];
	}

}




- (void)setIsOnScreen:(BOOL)isOnScreen {
	if (_isOnScreen == isOnScreen) {
		return;
	}
	_isOnScreen = isOnScreen;
	self.currentLayout.rootContainer.isOnScreen = isOnScreen;
	[self conditionallyRecordContentViewEvent];
}

- (void)onIsInitialSelection {
	self.isInitialContent = YES;
}

- (void)saveState:(NSMutableDictionary*)stateDictionary {
	stateDictionary[self.contentStub.modelId] = @([self.currentLayout.rootContainer getState]);
}
- (void)restoreState:(NSDictionary*)stateDictionary {
	id<NSObject> state = stateDictionary[self.contentStub.modelId];
	if (state) {
		self.savedScrollOffset = ((NSNumber*)state).floatValue;
	}
}

- (void)onSuperviewDidAppear:(BOOL)viewControllerIsMovingToParent {
	[self.currentLayout.rootContainer onSuperviewDidAppear:viewControllerIsMovingToParent];
}

- (void)onSuperviewDidDisappear:(BOOL)viewControllerIsMovingFromParent {
	[self.currentLayout.rootContainer onSuperviewDidDisappear:viewControllerIsMovingFromParent];
	if (viewControllerIsMovingFromParent) {
		[self removeFromSuperview];
	}
}


- (void)scrollToTopAnimated:(BOOL)anim {
	CGPoint pt = CGPointZero;
	pt.y -= self.scrollView.scrollIndicatorInsets.top;
	[self.scrollView setContentOffset:pt animated:anim];
}

extern BOOL s_landscapeLock;
BOOL s_disableOrientationChange;

- (void)setFrame:(CGRect)frame {
	if (frame.size.width>0 && frame.size.height>0 && !s_landscapeLock && !s_disableOrientationChange) {
		BOOL isPortrait = self.frame.size.height >= self.frame.size.width;
		BOOL willBePortrait = frame.size.height >= frame.size.width;
		if (isPortrait != willBePortrait) {
			//NSLog(@"rotation changed! %d", willBePortrait);
			self.currentLayout = nil;
			self.loadingImageView = nil;
			[self.subviews makeObjectsPerformSelector:@selector(removeFromSuperview)];
			self.emptyView = nil;
		}
	}
	[super setFrame:frame];
}

- (BOOL)isArticle {
    return [self.contentObject isKindOfClass:[BNItem class]];
}

- (BOOL)accessibilityPerformEscape {
    if (![self isArticle]) {
        [self scrollToTopAnimated:NO];
        BNNavigationController *controller = [BNNavigationController get];
        for (UIView *subview in controller.navigationBar.subviews) {
            if ([subview.accessibilityLabel isEqualToString:@"Menu"]) {
                UIAccessibilityPostNotification(UIAccessibilityAnnouncementNotification, @"Scrolled to top");
                UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, subview);
                return YES;
            }
        };
    }
    return YES;
}
*/

/**
 IBNModuleHolder
 */
void BNContentView::invalidateModuleSize(BNModule* module) {
    _cachedFrame = RECT::zero();
    setNeedsLayout();
}

