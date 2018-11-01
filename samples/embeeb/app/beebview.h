//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#ifndef beebview_hpp
#define beebview_hpp

#include "app.h"

class BeebView : public View, public IBeebDisplayCallbacks {
public:
    
    Beeb* _beeb;
    Bitmap* _bitmap;
    PIXELDATA _bitmapData;
    TextureRenderOp* _renderOp;
    RECT _visibleArea;
    bool _paused;
    sp<Timer> _timer;
    
    
    BeebView();
    void setBeeb(Beeb* beeb);
    
    // Overrides
    void attachToWindow(Window* window) override;
    void detachFromWindow() override;
    void layout(RECT constraint) override;
    
    // IBeebDisplayCallbacks
    virtual void setVisibleArea(int left, int top, int width, int height);
    virtual void drawFrame();
};


#endif /* beebview_hpp */
