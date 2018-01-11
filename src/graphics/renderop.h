//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define BLENDMODE_NONE 0
#define BLENDMODE_NORMAL 1
#define BLENDMODE_PREMULTIPLIED 2

class RenderOp : public Object {
public:
    class View* _view;
    RECT _rect;
    GLProgram* _prog;
    list<ObjPtr<RenderOp>>::iterator _viewListIterator; // view.renderList linked list entry
    int _mvpNum;
    ObjPtr<class RenderBatch> _batch; // non-NULL when attached to surface
    list<ObjPtr<RenderOp>>::iterator _batchIterator; // batch's linked list entry, valid when attached to surface
    int _renderBase; // offset into the batch vbo allocation
    int _renderCounter;
    bool _batchGeometryValid;
    bool _mustRedraw;
protected:
    int _blendMode;
    float _alpha;
    COLOUR _colour;

public:
    RenderOp(View* view);
    ~RenderOp();
    virtual int numQuads();
    virtual bool canMergeWith(const RenderOp* op);
    virtual void setRect(const RECT& rect);
    virtual void render(class Canvas* canvas, class Surface* surface);
    virtual bool intersects(RenderOp* op);
    virtual void asQuads(QUAD* quad);
    virtual void rectToSurfaceQuad(RECT rect, QUAD* quad);
    void invalidateBatchGeometry();
    RECT surfaceRect();
    
    // Property setters that trigger rebatch
    void setBlendMode(int blendMode);
    virtual void setAlpha(float alpha);
    virtual void setColour(COLOUR colour);
    void rebatchIfNecessary();
};

class RenderOpMultiRect : public RenderOp {
public:
    vector<RECT> _rects; // TODO: this is specific to text really, makes no sense for others now we have batches
    
    RenderOpMultiRect(View* view);
    virtual int numQuads();
    virtual void asQuads(QUAD* quad);
    //virtual bool intersects(RenderOp* op);
    

};

