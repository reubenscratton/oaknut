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
    sp<Shader> _shader;
    class RenderList* _list;
    int _listIndex;
    list<sp<RenderOp>>::iterator _listIterator; 
    int _mvpNum;
    sp<RenderBatch> _batch; // non-NULL when attached to surface
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
    virtual void prepareToRender(RenderTask* r, class Surface* surface);
    virtual bool intersects(RenderOp* op);
    virtual void asQuads(QUAD* quad);
    virtual void rectToSurfaceQuad(RECT rect, QUAD* quad);
    virtual void invalidateVertexes();
    RECT surfaceRect();
    int getRenderOrder();

    // If an op is 'batched' it is attached to the surface and may be part of a batch
    // of ops that can be drawn in a single call. If the shader changes (i.e. shader
    // feature change) or any uniform data changes, you must call invalidateBatch().
    virtual void invalidateBatch();
    
    // The op is 'valid' once it has chosen a shader that can satisfy its render params. If the
    // render params change then the shader needs to be validated and possibly generated on demand
    virtual void invalidateShader();
    virtual void validateShader(RenderTask* r)=0;
    virtual void rebatchIfNecessary();

    // Uniform setters. Changing a uniform will trigger a rebatch.
    virtual void setBlendMode(int blendMode);
    virtual void setAlpha(float alpha);
    virtual void setColor(COLOR color);
    virtual void setInset(EDGEINSETS inset);
    
    // Cleanup (TODO: only called by TextLayout at present, should be called whenever detached from surface)
    virtual void reset();
    
    // Overrides
#if DEBUG
    string debugDescription() override;
#endif
};


