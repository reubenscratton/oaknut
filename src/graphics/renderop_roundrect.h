//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class RoundRectRenderOp : public RenderOp {
public:
    RECT _rectTex;
    float _radii[2];
    COLOUR _strokeColour;
    float _strokeWidth;
    
    RoundRectRenderOp(View* view, COLOUR fillColour, float strokeWidth, COLOUR strokeColour, float radius);
    RoundRectRenderOp(View* view, COLOUR fillColour, float strokeWidth, COLOUR strokeColour, float radii[2]);
    virtual bool canMergeWith(const RenderOp* op);
    virtual void setRect(const RECT& rect);
    virtual void setStrokeColour(COLOUR strokeColour);
    virtual void asQuads(QUAD* quad);
    virtual void render(Canvas* canvas, Surface* surface);
};

