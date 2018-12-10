//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class PrivateSurfaceRenderOp : public TextureRenderOp {
public:
    QuadBuffer::Alloc* _alloc;
    bool _dirty;
    
    PrivateSurfaceRenderOp(View* view, const RECT& rect);
    ~PrivateSurfaceRenderOp();
    
    // Overrides
    void rectToSurfaceQuad(RECT rect, QUAD* quad) override;
    void render(class Window* window, Surface* surface) override;
    void validateShader() override;
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
    GLuint _fb;
    GLuint _tex;
    GLint _pixelType;
    GLint _pixelFormat;
    POINT _savedOrigin;
    list<RenderList*> _renderListsList;
    list<RenderList*>::iterator _renderListsInsertionPos;
    list<RenderOp*> _opsNeedingValidation;
    list<sp<RenderBatch>> _listBatches;
    bool _isPrivate;
    bool _supportsPartialRedraw;
    sp<PrivateSurfaceRenderOp> _op;
    int _mvpNum, _mvpNumPeak;
    bool _renderInProgress;
    
    Surface();
    Surface(View* owningView);
    ~Surface();
    void render(View* view, Window* window);
	void setSize(SIZE size);
	void setupPrivateFbo();
	void use();
    void cleanup();
  
    void detachRenderList(RenderList* list);
    void attachRenderList(RenderList* list);

    void addRenderOp(RenderOp* op);
    void removeRenderOp(RenderOp* op);
    
    void validateRenderOps();
    void batchRenderOp(RenderOp* op);
    void unbatchRenderOp(RenderOp* op);
    
private:
    void renderPhase1(View* view, Window* window, POINT origin);
    void renderPhase2(Surface* prevsurf, View* view, Window* window);
    //static int renderOrder(View* view1, View* view2);
    
    int _renderOrder;
#ifdef DEBUG
    string _renderLog;
#endif
};



