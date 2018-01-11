//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class TextureRenderOp : public RenderOp {
public:
    ObjPtr<Bitmap> _bitmap;
    RECT _rectTex;
    
    TextureRenderOp(View* view);
    TextureRenderOp(View* view, const RECT& rect, Bitmap* texture, const RECT* rectTex, COLOUR tintColour);
    TextureRenderOp(View* view, const char* assetPath, int tintColour);
    virtual bool canMergeWith(const RenderOp* op);
    virtual void render(Canvas* canvas, Surface* surface);
    virtual void asQuads(QUAD* quad);
    
    virtual void setAlpha(float alpha);
    virtual void setTexRect(const RECT& texRect);
};


