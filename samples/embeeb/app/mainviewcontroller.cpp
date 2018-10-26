//
//  MainViewController.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "mainviewcontroller.h"
#include "beebview.h"
#include "controllerview.h"
#include "disksviewcontroller.h"
#include "diskcontrols.h"
#include "snapshotsviewcontroller.h"
//#import "DiskImageRequest.h"
//#import "RecentDisks.h"


void MainViewController::LEDsChanged() {
    keyboardController->setShiftLockLED(_beeb->isShiftLockLEDon());
    keyboardController->setCapsLockLED(_beeb->isCapsLockLEDon());
}



MainViewController::MainViewController() {
    
    // Inflate layout
    View* view = app.layoutInflate("layout/main.res");
    _beebView = (BeebView*)view->findViewById("beebView");
    _controllerView = (ControllerView*)view->findViewById("controllerView");
    setView(view);
    
    // Create our Beeb instance
    _beeb = new Beeb();
    _beeb->keyboardCallbacks = this;
    _beebView->setBeeb(_beeb);
    _controllerView->_beeb = _beeb;
    keyboardController = new BeebKeyboardController();
    _controllerView->setController(keyboardController);

    
	//ImageView* titleView = new ImageView();// initWithImage:[UIImage imageNamed:@"Logo"]];
	//titleView.tintColor = [Styles colorForNavTint];
	//[titleView sizeToFit];
	//titleView.userInteractionEnabled = YES;
	//[titleView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onCodeCoverageButtonPressed:)]];
	//self.navigationItem.titleView = titleView;

    

	setTitle("emBeeb");
    //self.controllersButton.enabled = NO;
   // self.nextControllerButton.enabled = NO;
    addNavButton(false, "images/controller.png", [=]() {
    });
    _nextControllerButton = addNavButton(false, "images/next_layout.png", [=]() {
    });

    addNavButton(true, "images/time_machine.png", [=]() {
        sp<SnapshotsViewController> vc = new SnapshotsViewController(_beeb, _beebView, _currentDiskInfo, [=](Snapshot* snapshot){
            _currentSnapshot = snapshot;
            _beeb->serialize(false, (uint8_t*)snapshot->_data.data());
            // TODO: load controller
        });
        _navigationController->pushViewController(vc);
    });
    addNavButton(true, "images/floppy_disk.png", [=]() {
        sp<DisksViewController> vc = new DisksViewController([&](Game* game) {
            _currentSnapshot = NULL;
            _currentDiskInfo = game->defaultDiskInfo();
            URLRequest::get(_currentDiskInfo->diskUrl())->handleData([&](URLRequest* req) {
                if (req->error()) {
                    // TODO: show user error
                    return;
                }
                // Hold shift key down for 2 seconds across the reset
                _beeb->postKeyboardEvent(ScanCode_Shift, true);
                Timer::start([=]() {
                    _beeb->postKeyboardEvent(ScanCode_Shift, false);
                }, 2000, false);
                auto data = req->getResponseData();
                _beeb->loadDisc(data.data(), data.size(), 1);
            });
        });
        _navigationController->pushViewController(vc);
    });
	
	
/*
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onLoadDiskNotification:) name:kLoadDiskNotification object:nil];
 
	[[NSNotificationCenter defaultCenter] addObserver: self
											 selector: @selector(onAppBecameActive:)
												 name: UIApplicationDidBecomeActiveNotification
											   object: nil];
	[[NSNotificationCenter defaultCenter] addObserver: self
											 selector: @selector(onAppEnteredBackground:)
												 name: UIApplicationDidEnterBackgroundNotification
											   object: nil];
	*/

}

void MainViewController::onDidResume() {
	ViewController::onDidResume();
	_navigationController->_navBar->setBlurEnabled(false);
    _controllerView->setFocused(true);
	//if (_beebView->_paused) {
		_beebView->_paused = false;
		//_beebView->tick();
	//}
	_beebView->_beeb->_audioOutput->resume();
}
void MainViewController::onWillPause() {
	ViewController::onWillPause();
	// Stop beeb going here so it's not running through the nav push anim
	_beebView->_paused = true;
}
void MainViewController::onDidPause() {
	ViewController::onDidPause();
    _navigationController->_navBar->setBlurEnabled(true);
	_beebView->_beeb->_audioOutput->pause();
}



/*

- (void)onTapped:(id)sender {
	if ([self.controllerView isFirstResponder]) {
		[self.controllerView resignFirstResponder];
	}
}

- (void)viewWillLayoutSubviews {
	[super viewWillLayoutSubviews];
	UIScreen* screen = [UIScreen mainScreen];
	
	// Size and position the BBC Micro view
	CGRect rect = self.view.bounds;
	rect.origin.y = 64;
    //rect.size.width = 200;
	rect.size.height = screen.bounds.size.height - (64+216);
    rect = CGRectInset(rect, 8, 8);
	self.glkView.frame = rect;
	
	// Let libbeeb know the view area
	CGFloat scale = screen.scale;
	if ([screen respondsToSelector:@selector(nativeScale)]) {
		scale = [UIScreen mainScreen].nativeScale;
	}
	Beeb_setViewSize(self.hbeeb, rect.size.width*scale, rect.size.height*scale);

	rect.origin.y += rect.size.height;
	rect.size.height = self.view.bounds.size.height - rect.origin.y;
	rect.size.height = MIN(rect.size.height, 216); // max height is iOS keyboard default height
	rect.origin.y = self.view.bounds.size.height - rect.size.height;
	self.controllerView.frame = rect;
}

- (void)onAppBecameActive:(NSNotification*)notification {
    Beeb_resume(self.hbeeb);
}
- (void)onAppEnteredBackground:(NSNotification*)notification {
    Beeb_pause(self.hbeeb);
}

- (void)viewDidAppear:(BOOL)animated {
	[super viewDidAppear:animated];
    Beeb_resume(self.hbeeb);
}

- (void)viewDidDisappear:(BOOL)animated {
	[super viewDidDisappear:animated];
    Beeb_pause(self.hbeeb);
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
    if ([EAGLContext currentContext] == self.glkView.context) {
        [EAGLContext setCurrentContext:nil];
    }
}



- (void)onControllersButtonPressed:(id)sender {
	DiskControlsViewController* vc = [[DiskControlsViewController alloc] initWithDiskInfo:self.currentDiskInfo];
	[self.navigationController pushViewController:vc animated:YES];
}
- (void)onDiskButtonPressed:(id)sender {
	DisksViewController* vc = [DisksViewController new];
	//vc.delegate = self;
	[self.navigationController pushViewController:vc animated:YES];
}

- (void)onLoadDiskNotification:(NSNotification*)notification {
	self.currentDiskInfo = (DiskInfo*)notification.object;
	
	[[NSNotificationCenter defaultCenter] removeObserver:self name:kNotificationDiskControlsChanged object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDiskControlsChanged:) name:kNotificationDiskControlsChanged object:self.currentDiskInfo.title];

	// Load disk controls
	self.controllerView.diskControls = nil;
	self.controllerView.controller = self.keyboardController;
	self.controllersButton.enabled = YES;
	self.nextControllerButton.enabled = NO;
	self.pendingControllerToActivate = notification.userInfo[@"controllerId"];
	[DiskControls loadDiskControlsForDiskId:self.currentDiskInfo.title];

	// Get the disk data from the URL system
	BOOL shouldBootDisk = [notification.userInfo[@"bootDisk"] boolValue];
	if (shouldBootDisk) {
		[[SSURLRequestManager sharedInstance] requestURL:[NSURL URLWithString:self.currentDiskInfo.diskUrl] delegate:self flags:SSURLRequestFlagCacheNeverExpires priority:SSDownloadPriorityHigh ttl:INT32_MAX requestClass:[DiskImageRequest class]];
		[RecentDisks addDisk:self.currentDiskInfo];
	}
}

void TriggerCallback(const void* param1, void* param2) {
    MainViewController* vc = (__bridge MainViewController*)param1;
    Controller* controller = (__bridge Controller*)param2;
    NSLog(@"Boom!");
    vc.controllerView.controller = controller;

}


- (void)onDiskControlsChanged:(NSNotification*)notification {
	NSString* currentName = self.controllerView.controller.name;
	self.controllerView.diskControls = notification.userInfo[@"diskControls"];
	if (currentName) {
		[self.controllerView activateControllerByName:currentName];
	} else {
		Controller* initialController = self.controllerView.diskControls.initialController;
		self.controllerView.controller = initialController;
	}
	
	if (self.pendingControllerToActivate) {
		self.controllerView.controller = [self.controllerView.diskControls controllerById:self.pendingControllerToActivate];
		self.pendingControllerToActivate = nil;
	}

	// Default to keyboard
	if (!self.controllerView.controller) {
		self.controllerView.controller = self.keyboardController;
	}

	
	self.nextControllerButton.enabled = self.controllerView.diskControls.controllers.count>0;
	
	// Set triggers
	Beeb_resetTriggers(self.hbeeb);
	__weak MainViewController* weakself = self;
	for (Controller* controller in self.controllerView.diskControls.controllers) {
		if (controller.trigger) {
			NSScanner *scanner = [NSScanner scannerWithString:controller.trigger];
			[scanner setScanLocation:0];
			unsigned result = 0;
			[scanner scanHexInt:&result];
			uint16_t trigger = (uint16_t)result;
            Beeb_addTrigger(self.hbeeb, trigger, TriggerCallback, (__bridge void *)(weakself), (__bridge void *)(controller));
		}
	}
}

- (void)onCodeCoverageButtonPressed:(id)sender {
	if (Beeb_codeCoverageMode(self.hbeeb) == 0) {
		Beeb_setCodeCoverageMode(self.hbeeb, 2);
	} else {
		Beeb_setCodeCoverageMode(self.hbeeb, 0);
	}
}

- (void)onNextLayoutButtonPressed:(id)sender {
	NSInteger nextIndex = 0;
	if (self.controllerView.controller != self.keyboardController) {
		nextIndex = [self.controllerView.diskControls.controllers indexOfObject:self.controllerView.controller];
		if (nextIndex == NSNotFound) nextIndex=0; else nextIndex++;
		if (nextIndex >= self.controllerView.diskControls.controllers.count) {
			self.controllerView.controller = self.keyboardController;
			return;
		}
	}
	if (nextIndex < self.controllerView.diskControls.controllers.count) {
		self.controllerView.controller = self.controllerView.diskControls.controllers[nextIndex];
	}
}



- (BOOL)onRequestLoadedObject:(SSURLRequest*)request object:(id)object isCacheData:(BOOL)isCacheData {
	NSData* diskImage = (NSData*)object;
    
 
    // Load the disk. Performs a reset/break
	Beeb_bbcLoadDisc(self.hbeeb, (uint8_t*)diskImage.bytes, (int)diskImage.length, 1);
    [[NSNotificationCenter defaultCenter] postNotificationName:kResetCurrentSnapshot object:nil];

	return FALSE; // never need an update cos disk images don't change
}
- (BOOL)onRequestError:(SSURLRequest*)request error:(NSError*)error httpStatus:(NSInteger)httpStatus {
	[[SSURLRequestManager sharedInstance] unrequestURL:request.URL delegate:self];
	// TODO: show error
	return TRUE;
}

*/


