//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



RenderBatch::RenderBatch() {
}

RenderBatch::~RenderBatch() {
    if (_alloc) {
        _alloc->pool->free(_alloc);
        _alloc = NULL;
    }
}


void RenderBatch::invalidateGeometry(RenderOp* op) {
    _dirty = true;
}

void RenderBatch::render(Window* window, Surface* surface, RenderOp* firstOp) {

    // Upload any changed vertex data
    if (_dirty) {
        _dirty = false;
        _numQuads = 0;
        for (auto it=_ops.begin() ; it!=_ops.end() ; it++) {
            RenderOp* op = (*it);
            _numQuads += op->numQuads();
        }
        if (!_alloc || _alloc->count<_numQuads) {
            _alloc = window->_quadBuffer->alloc(_numQuads, _alloc);
        }
        QUAD* quad = (QUAD*)_alloc->addr();
        int renderBase = 0;
        for (auto it=_ops.begin() ; it!=_ops.end() ; it++) {
            RenderOp* op = (*it);
            op->asQuads(quad);
            op->_renderBase = renderBase;
            size_t numQuads = op->numQuads();
            quad += numQuads;
            renderBase += numQuads;
            op->_batchGeometryValid = true;
        }
        //app.log("Buffering %d quads", _alloc->count);
        check_gl(glBufferSubData, GL_ARRAY_BUFFER, _alloc->offset*sizeof(QUAD), _alloc->count*sizeof(QUAD), _alloc->addr());
    }
    
    // Use and configure the shader for this batch
    firstOp->render(window, surface);
    if (firstOp->_prog) {
        firstOp->_prog->lazyLoadUniforms();
    }
    
    // Determine how much many ops we can draw right now without breaking render order
    int numQuadsThisChunk = 0;
    auto it = firstOp->_batchIterator;
    RenderOp* currentOp = firstOp;
    RenderOp* nextOpInBatch = firstOp;
    
    REGION region;
    
    // Walk the render tree in render order. For each op after the current one we need to look
    // for anything else in the rendertree that is currently unrendered and which must
    // render before the next op in this batch.
    for (;;) {

        // If we've found the next batch op
        if (currentOp == nextOpInBatch) {
            
            // Cannot draw nextOpInBatch now, it clips something earlier in the render tree
            if (region.intersects(currentOp->surfaceRect())) {
                break;
            }

            // Do not render across a MVP change
            if (nextOpInBatch->_mvpNum != firstOp->_mvpNum) {
                break;
            }
            
            // TODO: We must disallow batching outside the current clip rect

            // This batch op can be rendered now!
            numQuadsThisChunk += currentOp->numQuads();
            currentOp->_mustRedraw = false;
            assert(currentOp->_renderCounter != window->_renderCounter);
            currentOp->_renderCounter = window->_renderCounter;

            // Update nextOpInBatch to point to the next one
            if (++it == _ops.end()) {
                break;
            }
            nextOpInBatch = *it;
        } else {
            // Add op to region of stuff thats yet to render, unless of course it already has been rendered
            if (currentOp->_renderCounter != window->_renderCounter) {
                region.addRect(currentOp->surfaceRect());
            }
        }
        
        // Move to next op in render order
        View* currentOpView = currentOp->_view;
        auto jt = currentOp->_viewListIterator;
        jt++;
        while (currentOpView && jt == currentOpView->_renderList.end()) {
            currentOpView = currentOpView->_nextView;
            if (currentOpView)
                jt = currentOpView->_renderList.begin();
        }
        if (!currentOpView) {
            break;
        }
        currentOp = *jt;
        
    }

    // If redrawing an invalid region, start iterating the region rects here
    RECT rect;
    vector<RECT>::iterator invalidRectIt;
    if (surface->_supportsPartialRedraw) {
        invalidRectIt = surface->_invalidRegion.rects.begin();
        if (invalidRectIt == surface->_invalidRegion.rects.end()) {
            return;
        }
    nextInvalidRect:
        rect = *invalidRectIt;
        rect.origin.y = surface->_size.height - rect.bottom(); /* surface -> viewport coords */
        window->pushClip(rect);
    }

    //   app.log("Drawing %d quads at once", numQuadsThisChunk);
    check_gl(glDrawElements, GL_TRIANGLES, 6 * numQuadsThisChunk, GL_UNSIGNED_SHORT, (void*)((_alloc->offset+firstOp->_renderBase)*6*sizeof(GLshort)));

    // Iterate next rect of invalid region, if there is any
    if (surface->_supportsPartialRedraw) {
        window->popClip();
        if (++invalidRectIt != surface->_invalidRegion.rects.end()) {
            goto nextInvalidRect;
        }
    }

}

#ifdef DEBUG

string RenderBatch::debugDescription() {
    string str = "{";
    for (auto it=_ops.begin() ; it!=_ops.end() ; it++) {
        RenderOp* op = (*it);
        str += op->debugDescription();
    }
    str += "}";
    return str;
}

#endif

