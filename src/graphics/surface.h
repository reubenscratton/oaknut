//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class RenderList : public Object {
public:
    uint32_t _renderOrder; // set by Surface::renderPass1, read in Surface::renderPass2.
    list<sp<RenderOp>> _ops;
    list<RenderList*>::iterator _surfaceIt;
    
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
    REGION _invalidRegion; // unused on primary surface
    MATRIX4 _mvp;
    POINT _savedOrigin;
    list<RenderList*> _renderListsList;
    list<RenderList*>::iterator _renderListsInsertionPos;
    list<RenderOp*> _opsNeedingValidation;
    list<sp<RenderBatch>> _listBatches;
    int _mvpNum, _mvpNumPeak;
    sp<Texture> _texture; // null on primary surface
    
    Surface(Renderer* renderer, bool isPrivate);

    virtual void render(View* view, Renderer* renderer);
    virtual void setSize(const SIZE& size);

    void detachRenderList(RenderList* list);
    void attachRenderList(RenderList* list);

    void addRenderOp(RenderOp* op);
    void removeRenderOp(RenderOp* op);
    
    void validateRenderOps(Renderer* renderer);
    void batchRenderOp(RenderOp* op);
    void unbatchRenderOp(RenderOp* op);
    
    // Private surface support
    bool _isPrivate;
    bool _supportsPartialRedraw;
    sp<RenderOp> _op;
    void markPrivateRenderQuadDirty();
    

private:
    void renderPhase1(Renderer* renderer, View* view, RECT surfaceRect);
    void renderPhase2(Renderer* renderer);
    void renderPhase3(Renderer* renderer, View* view, Surface* prevsurf);
    
    int _renderOrder;

#ifdef DEBUG
    string _renderLog;
#endif
};



