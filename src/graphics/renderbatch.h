//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class RenderBatch : public Object {
public:
    list<sp<RenderOp>> _ops;
    ItemPool::Alloc* _alloc;
    int _numQuads;
    int _head;
    bool _dirty;
    bool _log;
    class Surface* _surface;
    list<sp<RenderBatch>>::iterator _renderBatchListIterator;
    
    RenderBatch();
    ~RenderBatch();
    void render(Renderer* renderer, Surface* surface, RenderOp* firstOp);
    
    void invalidateGeometry(RenderOp* op);
    void updateQuads(Renderer* renderer);
    
#ifdef DEBUG
    string debugDescription();
#endif
};
