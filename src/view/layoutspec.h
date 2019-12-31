//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//



/**
LAYOUTSPEC defines the position (or size relative to the companion position) of one view edge.

A LAYOUTSPEC contains 3 components that are summed to derive the final edge position in pixels:
 - A reference property value
 - The view's own content size
 - A constant value.
 
 */

typedef struct LAYOUTSPEC {
    View* view;
    enum ViewPropertyType {
        PropNone,
        PropLeftOrTop,
        PropRightOrBottom,
        PropWidthOrHeight,
        PropHeightOrWidth
    } propertyType;
    float multProp;
    float multContentSize;
    float constant;
    
    LAYOUTSPEC();
    LAYOUTSPEC(View* view, enum ViewPropertyType propertyType, float multRef, float multContentSize, float constant);

    // Sizes
    static LAYOUTSPEC Fill(float multParentSize=1.0f, float constant=0.0f);
    static LAYOUTSPEC Wrap();
    static LAYOUTSPEC Abs(float x);
    static LAYOUTSPEC Match(View* sibling);
    // Positions within parent view
    static LAYOUTSPEC Left();
    static LAYOUTSPEC Top();
    static LAYOUTSPEC Right();
    static LAYOUTSPEC Bottom();
    // Positions relative to a sibling
    static LAYOUTSPEC ToLeftOf(View* view, float margin);
    static LAYOUTSPEC ToRightOf(View* view, float margin);
    static LAYOUTSPEC Above(View* view, float margin);
    static LAYOUTSPEC Below(View* view, float margin);
    static LAYOUTSPEC Aspect(View* view, float mult);
    
    /*static ALIGNSPEC Center();
    
    float calc(float measuredSize, float containingOrigin, float containingSize) const;*/
    
    static LAYOUTSPEC fromSizeStyle(const variant* style, View* view);
    static LAYOUTSPEC fromAlignStyle(const variant* style, View* view);

} LAYOUTSPEC;

