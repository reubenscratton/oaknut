//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 BitmapProvider is a companion class to RenderOp that provides async image loading.
 */
class BitmapProvider : public Object {
public:
    void dispatch(Bitmap* bitmap);
    void dispatch(AtlasNode* node);
    
    friend class TextureRenderOp;
private:
    list<class TextureRenderOp*> _ops;
    Bitmap* _bitmap;
    AtlasNode* _node;
};

class TextureRenderOp : public RenderOp {
public:
    sp<Bitmap> _bitmap;
    sp<BitmapProvider> _bitmapProvider;
    RECT _rectTex;
    
    TextureRenderOp();
    TextureRenderOp(const RECT& rect, Bitmap* texture, const RECT* rectTex, COLOR tintColor);
    TextureRenderOp(const char* assetPath, int tintColor);
    
    // API
    virtual void setBitmap(Bitmap* bitmap);
    virtual void setBitmap(AtlasNode* node);
    virtual void setBitmap(BitmapProvider* bitmapProvider);
    virtual void setTexRect(const RECT& texRect);
    

    // Overrides
    bool canMergeWith(const RenderOp* op) override;
    void prepareToRender(Renderer* renderer, Surface* surface) override;
    void asQuads(QUAD* quad) override;
    void validateShader(Renderer* renderer) override;
    
    
};


