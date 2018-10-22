//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
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
    EDGEINSETS _inset;
    GLProgram* _prog;
    list<sp<RenderOp>>::iterator _viewListIterator; // view.renderList linked list entry
    int _mvpNum;
    sp<class RenderBatch> _batch; // non-NULL when attached to surface
    list<sp<RenderOp>>::iterator _batchIterator; // batch's linked list entry, valid when attached to surface
    int _renderBase; // offset into the batch vbo allocation
    int _renderCounter;
    bool _batchGeometryValid;
    bool _mustRedraw;
//protected:
    int _blendMode;
    float _alpha;
    COLOR _color;
    
public:
    RenderOp();
    ~RenderOp();
    virtual int numQuads();
    virtual bool canMergeWith(const RenderOp* op);
    virtual void setRect(const RECT& rect);
    virtual void render(class Window* window, class Surface* surface);
    virtual bool intersects(RenderOp* op);
    virtual void asQuads(QUAD* quad);
    virtual void rectToSurfaceQuad(RECT rect, QUAD* quad);
    void invalidateBatchGeometry();
    RECT surfaceRect();

    // The op is 'valid' once it has chosen a shader that can satisfy its render params. If the
    // render params change then the shader needs to be validated and possibly generated on demand
    bool _shaderValid;
    void invalidate();
    virtual void validateShader()=0;
    void rebatchIfNecessary();

    // Property setters that trigger rebatch
    void setBlendMode(int blendMode);
    virtual void setAlpha(float alpha);
    virtual void setColor(COLOR color);
    virtual void setInset(EDGEINSETS inset);
};


