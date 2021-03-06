//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "../../model/_module.h"

/**
 ImageCacheKey - a helper class that uniquely identifies a decompressed image (i.e. URL plus bitmap size)
 and which also serves as the keytype for NSCache* imageCache.
 */
struct ImageCacheKey {
    string url;
    uint32_t width;
    uint32_t height;
};

class BNImageView : public ImageView {
public:
    //void attachToWindow(Window *window) override;
    void attachToSurface() override;
    void detachFromSurface() override;
    void layout(RECT constraint) override;

    BNImageView();
    void setBNImage(BNImage* image);
    BNImage* getBNImage();
    void updateImageKeyDimensions();
    void tryUpdateImage();

    TIMESTAMP _timeImageUrlSet;
    BNImage* _bnimage;
    ImageCacheKey _imageKey;
    bool _shouldFade;
    bool _errorDisplay;
    ImageView* _errorView;

};

