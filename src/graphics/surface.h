//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class PrivateSurfaceRenderOp : public TextureRenderOp {
public:
    ItemPool::Alloc* _alloc;
    bool _dirty;
    
    PrivateSurfaceRenderOp(View* view, const RECT& rect);
    ~PrivateSurfaceRenderOp();
    
    // Overrides
    void rectToSurfaceQuad(RECT rect, QUAD* quad) override;
    void render(Renderer* renderer, Surface* surface) override;
    void validateShader(Renderer* renderer) override;
};

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

class Surface : public Object {
public:
	SIZE _size;
    REGION _invalidRegion; // unused on primary surface
    MATRIX4 _mvp;
    POINT _savedOrigin;
    list<RenderList*> _renderListsList;
    list<RenderList*>::iterator _renderListsInsertionPos;
    list<RenderOp*> _opsNeedingValidation;
    list<sp<RenderBatch>> _listBatches;
    sp<PrivateSurfaceRenderOp> _op;
    int _mvpNum, _mvpNumPeak;
    bool _renderInProgress;
    bool _isPrivate;
    bool _supportsPartialRedraw;

    Surface(bool isPrivate);

    virtual void render(View* view, Renderer* renderer);
    virtual void setSize(const SIZE& size);
	virtual void use() =0;

    void detachRenderList(RenderList* list);
    void attachRenderList(RenderList* list);

    void addRenderOp(RenderOp* op);
    void removeRenderOp(RenderOp* op);
    
    void validateRenderOps(Renderer* renderer);
    void batchRenderOp(RenderOp* op);
    void unbatchRenderOp(RenderOp* op);
    
private:
    void renderPhase1(View* view, Renderer* renderer, POINT origin);
    void renderPhase2(Surface* prevsurf, View* view, Renderer* renderer);
    
    int _renderOrder;
#ifdef DEBUG
    string _renderLog;
#endif
};



