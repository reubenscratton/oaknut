//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class RenderList : public Object {
public:
    RenderList();
    list<sp<RenderOp>> _ops;
    
    int _renderListsIndex; // as determined by the Surface.
    list<RenderList*>::iterator _renderListsPos;

    void addRenderOp(RenderOp* renderOp, bool atFront=false);
    void removeRenderOp(RenderOp* renderOp);
};

/**
 * @ingroup graphics_group
 * @class Surface
 * @brief Rendering target, currently backed by a GL texture.
 
 Surfaces are owned by Views, all visible Views in the owning view's hierarchy
 will be rendered to the surface.*/

class Surface : public RenderResource {
public:
	SIZE _size;
    COLOR _clearColor;
    bool _clearNeeded;
    REGION _invalidRegion; // unused on primary surface
    MATRIX4 _mvpR, _mvpP;
    POINT _savedOrigin;
    list<RenderList*> _renderLists;
    list<RenderList*>::iterator _renderListsLastTouchedIterator;
    list<RenderOp*> _opsNeedingValidation;
    list<sp<RenderBatch>> _listBatches;
    int _mvpNum, _mvpNumPeak;
    sp<Texture> _texture; // null on primary surface

    Surface(Renderer* renderer, bool isPrivate);

    virtual void bindToNativeWindow(long nativeWindowHandle) =0;

    virtual void render(View* view, RenderTask* r);
    virtual void setSize(const SIZE& size);

    void ensureRenderListAttached(RenderList* list);
    void detachRenderList(RenderList* list);

    void addRenderOp(RenderOp* op);
    void detachRenderListOp(RenderOp* op);
    
    void validateRenderOps(RenderTask* r);
    void batchRenderOp(RenderOp* op);
    void unbatchRenderOp(RenderOp* op);
    
    // Private surface support
    bool _isPrivate;
    bool _supportsPartialRedraw;
    sp<RenderOp> _op;
    void markPrivateRenderQuadDirty();
    
    void checkSanity(View* view, bool dump);
    
private:
    void renderPhase1(RenderTask* r, View* view, RECT surfaceRect);
    void renderPhase2(RenderTask* r);
    void renderPhase3(RenderTask* r, View* view, Surface* prevsurf);

#ifdef DEBUG
    string _renderLog;
#endif
};



