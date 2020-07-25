//
//  BNImageView.m
//  BBC News
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

/**
 An ImageView class that can download and display arbitrary images. It is optimized for BBC News
 in the following ways:
 
 - Opaque JPEGs are assumed (i.e. no transparency)
 
 - A single large RAM caches is kept for decompressed images (i.e. bitmaps) which correspond
 to a particular URL *and* a certain size. This approach was judged necessary because
 BBC News images are generally full-size (i.e. >=640 pixels width) but are most often
 rendered at a much smaller size (i.e. as a thumbnail in an index).
 
 */

#include "imageview.h"
//#import "BNURLRequestManager.h"
//#import "BNStyles.h"



BNImageView::BNImageView() {
	_opaque = true;
    setContentMode(ContentMode::AspectFill);
    setBackgroundColor(0xFFCCCCCC);//todo [UIColor lightGrayColor];

}


void BNImageView::detachFromSurface() {
    ImageView::detachFromSurface();
    cancelLoad();
    setBitmap(NULL);
}

void BNImageView::attachToSurface() {
    ImageView::attachToSurface();
    if (_imageKey.url.length()) {
        tryUpdateImage();
    }
}

void BNImageView::layout(RECT constraint) {
    ImageView::layout(constraint);
    
    if (_imageKey.width<=0 || _imageKey.height<=0) {
        if (getWidth()>0 && getHeight()>0) {
            tryUpdateImage();
        }
    }
}

/*- (void)setImage:(UIImage *)image {
	if (self.image != image) {
		[self.layer removeAllAnimations];
		[super setImage:image];
		[self setNeedsDisplay];
	}
	
	// If we had to show the activity view then fade the image in rather than show it immediately
	if (self.shouldFade && image!=nil) {
		self.shouldFade = NO;
		CATransition *transition = [CATransition animation];
		transition.duration = .5f;
		transition.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
		transition.type = kCATransitionFade;
		[self.layer addAnimation:transition forKey:nil];
		
	}
}*/

BNImage* BNImageView::getBNImage() {
	return _bnimage;
}

void BNImageView::setBNImage(BNImage* image) {
    if (!image) {
        cancelLoad();
        _imageKey.url = "";
        _bnimage = nullptr;
        setBitmap(nullptr);
        _errorDisplay = false;
        return;
    }

    
    // Cancel any extant HTTP request. Note that the request will only really get cancelled
    // if this is the only UIImageView that wants the current _imageURL.
    cancelLoad();
    
	// Update URL and reset image
	//_imageKey.url = url;
    setBitmap(NULL);
	_bnimage = image;
	_errorDisplay = false;
	
	if (/*_imageKey.url.length() &&*/ _surface) {
        tryUpdateImage();
	}
	
}


/*
- (void)decodeImageInBackground :(NS_VALID_UNTIL_END_OF_SCOPE NSData*)imageData forKey:(ImageCacheKey*)key {
	
	NS_VALID_UNTIL_END_OF_SCOPE UIImage* fullImage = [UIImage imageWithData:imageData];
	
	// Calculate the area of image we're actually interested in. We only support
	// two modes: aspect fill (the default, used by almost everything) and aspect
	// *fit* (used by photo galleries).
	CGFloat viewAspect = key.width / (CGFloat)key.height;
	CGFloat imageAspect = fullImage.size.width / fullImage.size.height;
	CGFloat xscale = key.width / fullImage.size.width;
	CGFloat yscale = key.height / fullImage.size.height;
	CGFloat scale = xscale;
	if (self.contentMode == UIViewContentModeScaleAspectFit) {
		if (imageAspect < viewAspect) {
			scale = yscale;
		}
	} else {
		if (imageAspect > viewAspect) {
			scale = yscale;
		}
	}
	CGSize cropSize = CGSizeMake(key.width / scale, key.height / scale);
	CGRect cropRect = CGRectMake(0,0, cropSize.width, cropSize.height);
	cropRect.origin.x += (fullImage.size.width - cropSize.width) / 2;
	cropRect.origin.y += (fullImage.size.height - cropSize.height) / 2;
	
	//NSLog(@"\nOriginal image url %@", key.url.absoluteString);
	//NSLog(@"Full image size %@", NSStringFromCGSize(fullImage.size));
	//NSLog(@"Desired size %d %d", key.width, key.height);
	//NSLog(@"Cropped image size %@\n", NSStringFromCGSize(cropSize));
	
	// Create the subimage
	CGImageRef imageRef =  CGImageCreateWithImageInRect(fullImage.CGImage, cropRect);
	
	// Rasterize the subimage into a bitmap
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    int stride = ((key.width * 4) + 15) & ~15;
    CGContextRef context = CGBitmapContextCreate(NULL, key.width, key.height, 8, stride, colorSpace, (CGBitmapInfo)(kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst));
    CGColorSpaceRelease(colorSpace);
    if (!context) {
		NSLog(@"Warning! Failed to create bitmap from image for %@", key.url);
		CGImageRelease(imageRef);
		return;
	}
	
	// Scale the bitmap down to the required size
    CGContextDrawImage(context, CGRectMake(0,0,key.width,key.height), imageRef);
	CGImageRelease(imageRef);
	
	// Create a CGImage around the bitmap and a UIImage around the CGImage
    CGImageRef decompressedImageRef = CGBitmapContextCreateImage(context);
    CGContextRelease(context);
    UIImage* image = [UIImage imageWithCGImage:decompressedImageRef];
	CGImageRelease(decompressedImageRef);
	
	// Put the decompressed bitmap into the cache
	[imageCache setObject:image forKey:key cost:stride*key.height];
	
	// Update the view's image property on the main thread
	dispatch_async(dispatch_get_main_queue(), ^{
		[self updateImageKeyDimensions];
		if ([self.imageKey isEqual:key]) {
			self.errorDisplay = NO;
			self.image = image;
		} else {
			NSLog(@"Warning! Wasted time decoding image %@  (%@,%@) vs (%@,%@)", key.url, @(key.width), @(key.height), @(self.imageKey.width), @(self.imageKey.height));
			
			// If imageview resized during decoding, just try again
			if ([key.url isEqual:self.imageKey.url]) {
				[self tryUpdateImage:NO];
			}
		}
	});
	
}
*/



void BNImageView::updateImageKeyDimensions() {
    if (_contentMode == ContentMode::AspectFit) { // used in photogalleries where user can pinch-zoom
        _imageKey.width = _bnimage->_width;
        _imageKey.height = _bnimage->_height;
    } else {
        _imageKey.width = (int)(_rect.size.width);
        _imageKey.height = (int)(_rect.size.height);
    }
}


void BNImageView::tryUpdateImage() {
    
    updateImageKeyDimensions();
    
    // If view is invisible, exit
    if (!_bnimage || _imageKey.width <=0 || _imageKey.height <=0) {
        return;
    }

    float width = _imageKey.width; // app->_window->getWidth();
    string url = _bnimage->urlForWidth(width);
    if (url == _imageKey.url && _renderOp->_bitmap) {
        return;
    }
    _imageKey.url = url;
    
            
    // Image data has to be downloaded. Schedule the activity view to appear
    //[self performSelector:@selector(setShouldFade:) withObject:@YES afterDelay:0.5];
    _shouldFade = true;
    
    // Create or attach to an HTTP request
    _timeImageUrlSet = app->currentMillis();
    setImage(_imageKey.url);

}


/*

- (void)setFrame:(CGRect)frame {
	BOOL wasInvisible = self.frame.size.width<=0 || self.frame.size.height<=0;
	[super setFrame:frame];
	if (wasInvisible && self.imageKey.url && !self.image) {
		[self tryUpdateImage:YES];
	}
}

- (void)onRequestError:(BNURLRequest*)request error:(NSError*)error httpStatus:(NSInteger)httpStatus {
	//NSLog(@"Image download error! %@", error);
	self.errorDisplay = YES;
	self.isLoading = NO;
}

- (void)setErrorDisplay:(BOOL)errorDisplay {
	if (errorDisplay == self.errorDisplay) {
		return;
	}
	_errorDisplay = errorDisplay;
	if (errorDisplay) {
		if (!self.errorView) {
			self.errorView = [[UIImageView alloc] initWithFrame:self.bounds];
			self.errorView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
			self.errorView.contentMode = UIViewContentModeCenter;
			self.errorView.image = [UIImage imageNamed:@"BrokenImage"];
			[self insertSubview:self.errorView atIndex:0];
		}
	} else {
		if (self.errorView) {
			[self.errorView removeFromSuperview];
			self.errorView = nil;
		}
	}
}



*/
