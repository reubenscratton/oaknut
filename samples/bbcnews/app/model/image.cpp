//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"
#include "../policy/policy.h"
//#import "BBCNEndpoint.h"





BNImage::BNImage(const variant& json) : BNBaseModel(json) {
    _altText = json.stringVal("altText");
    _caption = json.stringVal("caption");
    _copyrightHolder = json.stringVal("copyrightHolder");
    _width = json.intVal("width");
    _height = json.intVal("height");
    _positionHint = json.stringVal("positionHint");
}



string BNImage::urlForWidth(float width) {
	if (width <= 0) {
		width = _width;
	}
    vector<int> availableWidths = BNPolicy::current()->_imageChefRecipes;
    for (int availableWidth : availableWidths) {
		if (availableWidth >= width) {
			width = availableWidth;
			break;
		}
	}
    string fmt = BNPolicy::current()->_endpointImageChef->getHREF();
    fmt.replace("%@", "%s");
    string strWidth = string::format("%d", (int)width);
    return string::format(fmt.data(), strWidth.data(), _modelId.data());
}

SIZE BNImage::sizeForImageInView(BNImage* image, View* view) {
    
    float viewWidth = view->getWidth();
    float viewHeight = view->getHeight();
    
    float imageWidth = image->_width;
    float imageHeight = image->_height;
    
    float articleWidth = view->getWidth();
    
    SIZE size;
    
    float scaleFactor;
    
    size = SIZE(imageWidth, imageHeight);
    
    if (size.width > viewWidth) {
        scaleFactor = articleWidth / imageWidth;
        size = SIZE(imageWidth * scaleFactor, imageHeight * scaleFactor);
    }
    
    if (size.height > viewHeight * 0.90) {
        scaleFactor = (viewWidth * 0.45) / imageWidth;
        size = SIZE(imageWidth * scaleFactor, imageHeight * scaleFactor);
    }
    
    // Since new rules: https://confluence.dev.bbc.co.uk/display/newsapps/Image+Resizing+Logic
    //    if (size.height > viewHeight * 0.50) {
    //        scaleFactor = (viewHeight * 0.50) / imageHeight;
    //        size = SIZE(imageWidth * scaleFactor, imageHeight * scaleFactor);
    //    }
    
    if (size.width > viewWidth * 0.45 && size.width < viewWidth) {
        scaleFactor = (viewWidth * 0.45) / imageWidth;
        size = SIZE(imageWidth * scaleFactor, imageHeight * scaleFactor);
    }
    
    return size;
}

// Inline meaning "in line" i.e. text attachment.
bool BNImage::isInlineImage(SIZE imageSize, SIZE viewSize) {
    return floorf(imageSize.width) == floorf(viewSize.width);
}

bool BNImage::isProbablyAHorizontalRule() {
	return _height <= 5;
}





string BNStaticImage::urlForWidth(float width) {
    return "todo: load static image";// [[NSBundle mainBundle] URLForResource: self.modelId withExtension:@"png"];
}

