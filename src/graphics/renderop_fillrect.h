//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ColorRectFillRenderOp : public RenderOp {
public:
    
    ColorRectFillRenderOp(View* view);
    ColorRectFillRenderOp(View* view, const RECT& rect, COLOUR colour);
    
    // Overrides
    virtual void setAlpha(float alpha);
};

