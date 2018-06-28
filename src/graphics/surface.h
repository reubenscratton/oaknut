//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
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
    virtual void rectToSurfaceQuad(RECT rect, QUAD* quad);
    virtual void render(class Window* window, Surface* surface);
};

/*
 * @ingroup graphics_group
 * \class Surface
*/
class Surface : public Object {
public:
	SIZE _size;
    REGION _invalidRegion; // unused on primary surface
    Matrix4 _mvp;
    GLuint _fb;
    GLuint _tex;
    GLint _pixelType;
    GLint _pixelFormat;
    POINT _savedOrigin;
    list<ObjPtr<RenderBatch>> _listBatches;
    bool _supportsPartialRedraw;
    ObjPtr<PrivateSurfaceRenderOp> _op;
    int _mvpNum;
    bool _renderInProgress;
    
    Surface();
    Surface(View* owningView);
    ~Surface();
    void render(View* view, Window* window);
	void setSize(SIZE size);
	void setupPrivateFbo();
	void use();
    void cleanup();
  
    void detachViewOps(View* view);
    void attachViewOps(View* view);

    void addRenderOp(RenderOp* op);
    void removeRenderOp(RenderOp* op);
    
private:
    void renderPhase1(View* view, Window* window, POINT origin);
    void renderPhase2(Surface* prevsurf, View* view, Window* window);
    static int renderOrder(View* view1, View* view2);
    
#ifdef DEBUG
    string _renderLog;
#endif
};



