//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class RenderBatch : public Object {
public:
    list<ObjPtr<RenderOp>> _ops;
    //QuadBuffer* _quadBuffer;
    QuadBuffer::Alloc* _alloc;
    int _numQuads;
    int _head;
    bool _dirty;
    bool _log;
    class Surface* _surface;
    list<ObjPtr<RenderBatch>>::iterator _renderBatchListIterator;
    //list<ObjPtr<RenderOp>>::iterator _renderIterator;
    //int _renderBase;

    
    RenderBatch();
    ~RenderBatch();
    void render(Canvas* canvas, Surface* surface, RenderOp* firstOp);

    
    void invalidateGeometry(RenderOp* op);

#ifdef DEBUG
    string debugDescription();
#endif
};
