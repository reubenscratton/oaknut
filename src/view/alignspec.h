//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//



//
// View alignment/positioning
//
// View alignment is always relative to an anchor, which defaults to the parent view or (for root views) the screen.
//
// View alignment along a single axis involves two multipliers and a margin. The first multiplier is
// that of the anchor's width, the second is that of one's own width, and the margin is a delta in pixels.
//

#define NO_ANCHOR ((View*)-1)

typedef struct ALIGNSPEC {
    View* anchor;
    float multiplierAnchor;
    float multiplierSelf;
    float margin;
    
    ALIGNSPEC();
    ALIGNSPEC(View* anchor, float multiplierAnchor, float multiplierOwn, float margin);
    ALIGNSPEC(const variant& value, View* view);

    static ALIGNSPEC None(); // Use when view positioning is done in code
    static ALIGNSPEC Left();
    static ALIGNSPEC Center();
    static ALIGNSPEC Right();
    static ALIGNSPEC Top();
    static ALIGNSPEC Bottom();
    static ALIGNSPEC ToLeftOf(View* view, float margin);
    static ALIGNSPEC ToRightOf(View* view, float margin);
    static ALIGNSPEC Above(View* view, float margin);
    static ALIGNSPEC Below(View* view, float margin);
    
    float calc(float measuredSize, float containingOrigin, float containingSize) const;
    
} ALIGNSPEC;

