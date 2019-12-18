//
//  IdaaS
//
//  Copyright © 2018 PQ. All rights reserved.
//

#pragma once
#include <oaknut.h>

class MaskView : public View {
public:
    
    MaskView();
    
    enum HoleShape {
        None,
        Rect,
        Oval
    };
    
    void setHoleShape(HoleShape shape);
    void setHoleStrokeColour(COLOR color);
    void setHoleFillColour(COLOR color);
    RECT getHoleRect() const;
    
    // Overrides
    virtual void layout(RECT constraint) override;
    //virtual void setBackgroundColour(COLOR color, int duration);

protected:
    
    bool applySingleStyle(const string& name, const style& value) override;

    COLOR _backgroundColour;
    HoleShape _holeShape;
    MEASURESPEC _holeWidthMeasureSpec;
    MEASURESPEC _holeHeightMeasureSpec;
    ALIGNSPEC _holeAlignSpecX;
    ALIGNSPEC _holeAlignSpecY;
    RECT _holeRect;
    COLOR _holeStrokeColour;
    COLOR _holeFillColour;
    float _holeStrokeWidth;
    float _holeCornerRadius;
    sp<Animation> _strokeColorAnim;
  
    friend class MaskRenderOp;
};
