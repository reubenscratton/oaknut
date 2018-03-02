//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class BitmapProvider : public Object {
public:
    class Callback {
    public:
        virtual void onBitmapChanged() =0;
    };

    virtual Bitmap* getBitmap() const =0;
    virtual void addCallback(Callback* callback) =0;
    virtual void removeCallback(Callback* callback) =0;
};

class SimpleBitmapProvider : public BitmapProvider {
public:
    SimpleBitmapProvider(Bitmap* bitmap);
    virtual Bitmap* getBitmap() const;
    virtual void addCallback(Callback* callback);
    virtual void removeCallback(Callback* callback);

private:
    ObjPtr<Bitmap> _bitmap;
};

class AsyncBitmapProvider : public BitmapProvider {
public:

    AsyncBitmapProvider(const char* assetPath);

    virtual Bitmap* getBitmap() const;
    virtual void addCallback(Callback* callback);
    virtual void removeCallback(Callback* callback);

protected:
    list<Callback*> _callbacks;
    ObjPtr<Bitmap> _bitmap;
};

class TextureRenderOp : public RenderOp, public BitmapProvider::Callback {
public:
    ObjPtr<BitmapProvider> _bitmapProvider;
    RECT _rectTex;
    
    TextureRenderOp(View* view);
    TextureRenderOp(View* view, const RECT& rect, Bitmap* texture, const RECT* rectTex, COLOUR tintColour);
    TextureRenderOp(View* view, const char* assetPath, int tintColour);
    virtual bool canMergeWith(const RenderOp* op);
    virtual void render(Window* window, Surface* surface);
    virtual void asQuads(QUAD* quad);
    
    virtual void setAlpha(float alpha);
    virtual void setColour(COLOUR colour);
    virtual void setTexRect(const RECT& texRect);
    virtual void setBitmap(Bitmap* bitmap);
    virtual void setBitmapProvider(BitmapProvider* bitmapProvider);

    // BitmapProvider::Callback
    virtual void onBitmapChanged();

};


