//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
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
    ObjPtr<Timer> _timer;
    
    
    BeebView();
    void setBeeb(Beeb* beeb);
    void attachToWindow(Window* window);
    void detachFromWindow();
    void layout();
    
    // IBeebDisplayCallbacks
    virtual void setVisibleArea(int left, int top, int width, int height);
    virtual void drawFrame();
};


#endif /* beebview_hpp */
