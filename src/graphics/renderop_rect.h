//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class RectRenderOp : public RenderOp {
public:
    RectRenderOp();

    // API
    COLOR getFillColor() const;
    void setFillColor(COLOR fillColor);
    COLOR getStrokeColor() const;
    void setStrokeColor(COLOR strokeColor);
    float getStrokeWidth() const;
    void setStrokeWidth(float strokeWidth);
    VECTOR4 getCornerRadii() const;
    void setCornerRadius(float radius);
    void setCornerRadii(const VECTOR4& cornerRadii);

    // Overrides
    void setAlpha(float alpha) override;
    bool canMergeWith(const RenderOp* op) override;
    void asQuads(QUAD* quad) override;
    void validateShader(Renderer* renderer) override;
    
//protected:
    COLOR _strokeColor;
    float _strokeWidth;
    VECTOR4 _radii;
    
#define OPFLAG_FILL 1
#define OPFLAG_STROKE 2
#define OPFLAG_CORNER1 4
#define OPFLAG_CORNER4 8
    int _flags;

};


