//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


namespace oak {
class PrivateSurfaceRenderOp : public TextureRenderOp {
public:
    ItemPool::Alloc* _alloc;
    bool _dirty;
    
    PrivateSurfaceRenderOp(Renderer* renderer, View* view, const RECT& rect) : TextureRenderOp(rect, NULL, NULL, 0) {
        _view = view;
        _alloc = renderer->allocQuads(1, NULL);
        _dirty = true;
        validateShader(renderer);
    }
    ~PrivateSurfaceRenderOp() {
        _alloc->pool->free(_alloc);
        _alloc = NULL;
    }

    // Overrides
    void rectToSurfaceQuad(RECT rect, QUAD* quad) override {
        rect.origin += _view->_surfaceOrigin;
        *quad = QUAD(rect, 0);
    }
    void validateShader(Renderer* renderer) override {
        //TextureRenderOp::validateShader(renderer);
        ShaderFeatures features;
        features.sampler0 = 1; //_bitmap->_texture->getSampler();
        features.alpha = (_alpha<1.0f);
        features.tint = (_color!=0);
        _shader = renderer->getShader(features);
        _shaderValid = true;
    }
    void prepareToRender(Renderer* renderer, Surface* surface) override {
        RenderOp::prepareToRender(renderer, surface);

        if (_dirty) {
            _dirty = false;
            QUAD* quad = (QUAD*)_alloc->addr();
            asQuads(quad);
            renderer->uploadQuad(_alloc);
        }

        // Bind to the private surface texture
        renderer->renderPrivateSurface(_view->_surface, _alloc);
    }
};
}


/**
 
 A deep dive into rendering
 ==========================
 
 The view tree maintains a doubly-linked list of each view in render order. Each view contains
 its list of render ops. In this way we have a list of all the render ops in the logical
 order that they should be rendered.
 
 We also maintain a list of renderop *batches*, i.e. groups of related ops that can be drawn in
 a single OpenGL draw call. The renderer walks the view tree and the render op list, and for
 each op it finds, if it hasn't yet been drawn then it gets the batch it belongs to and draws
 as much as possible of that batch without breaking render order (a break would be to draw a
 later op that clips an earlier op before the earlier op has drawn).
 
 If a view has scrollbars those must be drawn after all child views.
 
 
 */

MATRIX4 setOrthoFrustum(float l, float r, float b, float t, float n, float f) {
    MATRIX4 mat;
    mat[0]  = 2 / (r - l);
    mat[5]  = 2 / (t - b);
    mat[10] = -2 / (f - n);
    mat[12] = -(r + l) / (r - l);
    mat[13] = -(t + b) / (t - b);
    mat[14] = -(f + n) / (f - n);
    return mat;
}


Surface::Surface(bool isPrivate) {
    _supportsPartialRedraw = isPrivate;
    _isPrivate = isPrivate;
}


void Surface::setSize(const SIZE& size) {
    _size = size;
    _mvp = setOrthoFrustum(0, size.width, size.height, 0, -1, 1);
    if (_supportsPartialRedraw) {
        _invalidRegion.rects.clear();
        _invalidRegion.addRect(RECT(0,0,size.width,size.height));
    }
}


void Surface::detachRenderList(RenderList *list) {
    for (auto it=list->_ops.begin() ; it!= list->_ops.end() ; it++) {
        removeRenderOp(*it);
    }
    if (list->_renderOrder > 0) {
        _renderListsList.erase(list->_surfaceIt);
    }
}

void Surface::attachRenderList(RenderList* list) {
    list->_renderOrder = 0; // indicates list needs insertion into _renderListsList during renderPhase1.
    for (auto it=list->_ops.begin() ; it!= list->_ops.end() ; it++) {
        addRenderOp(*it);
    }
}


void Surface::addRenderOp(RenderOp* op) {
    assert(op->_view);
    op->_mustRedraw = true;
    assert(!op->_batch);
    _opsNeedingValidation.push_back(op);
}

void Surface::validateRenderOps(Renderer* renderer) {
    if (!_opsNeedingValidation.size()) {
        return;
    }
    
    // First validate any ops that need it
    list<RenderOp*> opsValid;
    auto i = _opsNeedingValidation.begin();
    while (i != _opsNeedingValidation.end()) {
        RenderOp* op = (*i);
        op->validateShader(renderer);
        if (op->_shaderValid) {
            opsValid.push_back(op);
            i = _opsNeedingValidation.erase(i);
        }
        else {
            ++i;
        }
    }
    
    // Then embatch them
    for (auto& op : opsValid) {
        assert(op->_shaderValid && !op->_batch);
        batchRenderOp(op);
    }
}

void Surface::removeRenderOp(RenderOp* op) {
    auto it = std::find(_opsNeedingValidation.begin(), _opsNeedingValidation.end(), op);
    if (it != _opsNeedingValidation.end()) {
        _opsNeedingValidation.erase(it);
    }
    if (op->_batch) {
        unbatchRenderOp(op);
    }
}

void Surface::batchRenderOp(RenderOp* op) {

    // Find a compatible batch
    RenderBatch* batch = NULL;
    for (auto it : _listBatches) {
        RenderOp* existingBatchedOp = *(it->_ops.begin());
        if (existingBatchedOp->canMergeWith(op)) {
            batch = it;
            break;
        }
    }
    
    // No compatible batch exists, so create it
    if (!batch) {
        batch = new RenderBatch();
        batch->_surface = this;
        batch->_renderBatchListIterator = _listBatches.insert(_listBatches.end(), batch);
    }
    
    // Find the insertion point in the batch so we respect render order
    assert(!op->_batch);
    op->_batch = batch;
    batch->_dirty = true;
    for (auto it=batch->_ops.begin() ; it!=batch->_ops.end() ;it++) {
        if (op->_list->_renderOrder < (*it)->_list->_renderOrder) {
            op->_batchIterator = batch->_ops.insert(it, op);
            return;
        }        
    }
    op->_batchIterator = batch->_ops.insert(batch->_ops.end(), op);
}
void Surface::unbatchRenderOp(RenderOp* op) {
    RenderBatch* batch = op->_batch;
    op->_batch = NULL;
    assert(batch);
    assert(batch->_surface == this);
    batch->_ops.erase(op->_batchIterator);
    batch->_dirty = true;
    if (!batch->_ops.size()) {
        _listBatches.erase(batch->_renderBatchListIterator);
    }
}

void RenderList::addRenderOp(RenderOp* renderOp, bool atFront/*=false*/) {
    assert(!renderOp->_list);
    renderOp->_list = this;
    renderOp->_listIterator = _ops.insert(atFront ? _ops.begin() : _ops.end(), renderOp);
}
void RenderList::removeRenderOp(RenderOp* renderOp) {
    assert(renderOp->_list == this);
    renderOp->_list = NULL;
    _ops.erase(renderOp->_listIterator);
}


void Surface::renderPhase1(Renderer* renderer, View* view, POINT origin) {

    if (view->_visibility != Visible) {
        return;
    }
    Surface* surface = this;

    // If view has a private surface, ensure surface has same size as view
    bool usesPrivateSurface = (view->_surface != surface);
    if (usesPrivateSurface) {
        bool sizeChanged = false;
        view->_surface->_renderOrder = 1;
        view->_surface->_renderListsInsertionPos = view->_surface->_renderListsList.end();
        if (view->_surface->_size.width != view->_rect.size.width || view->_surface->_size.height != view->_rect.size.height) {
            view->_surface->setSize(view->_rect.size);
            sizeChanged = true;
        }
        
        // Create the private surface op, allocating vertex space from the host surface's vbo
        if (!view->_surface->_op) {
            RECT rect = view->getOwnRect();
            view->_surface->_op = new PrivateSurfaceRenderOp(renderer, view, rect);
        } else {
            if (sizeChanged) {
                view->_surface->_op->setRect(view->getOwnRect());
            }
        }

        // Reset origin
        origin = {0,0};
        surface = view->_surface;

    } else {

        // Adjust origin
        origin.x += view->_rect.origin.x;
        origin.y += view->_rect.origin.y;
    }
    
    bool changesMvp = view->_matrix || !view->_contentOffset.isZero();
    int mvpNumToRestore;
    if (changesMvp) {
        mvpNumToRestore = _mvpNum;
        _mvpNumPeak++;
        _mvpNum = _mvpNumPeak;
    }
    
    // Give the view a chance to update its renderOps now that layout is complete
    if (view->_updateRenderOpsNeeded) {
        view->updateRenderOps();
        view->_updateRenderOpsNeeded = false;
    }
        
    // Draw content renderlist
    if (view->_renderList) {
        if (!view->_renderList->_renderOrder) {
            view->_renderList->_surfaceIt = surface->_renderListsList.insert(surface->_renderListsInsertionPos, view->_renderList);
        }
        view->_renderList->_renderOrder = surface->_renderOrder++;
        surface->_renderListsInsertionPos = view->_renderList->_surfaceIt;
        for (auto r:view->_renderList->_ops) {
            r->_mvpNum = _mvpNum;
        }
    }
    
    // Recurse subviews
    for (auto it = view->_subviews.begin(); it!=view->_subviews.end() ; it++) {
        sp<View>& subview = *it;
        surface->renderPhase1(renderer, subview, origin);
    }
    
    // Draw decor renderlist
    if (view->_renderListDecor) {
        if (!view->_renderListDecor->_renderOrder) {
            view->_renderListDecor->_surfaceIt = surface->_renderListsList.insert(surface->_renderListsInsertionPos, view->_renderListDecor);
        }
        view->_renderListDecor->_renderOrder = surface->_renderOrder++;
        for (auto r:view->_renderListDecor->_ops) {
            r->_mvpNum = _mvpNum;
        }
        surface->_renderListsInsertionPos = view->_renderListDecor->_surfaceIt;
    }

    // If this is a private surface then got to ensure vertex buffer is up to date
    // Wondering if surfaces should own their own vertex buffers...
    if (usesPrivateSurface) {
        surface->renderPhase2(renderer);
    }

    if (changesMvp) {
        _mvpNum = mvpNumToRestore;
    }

}

void Surface::renderPhase2(Renderer* renderer) {
    validateRenderOps(renderer);
    for (RenderBatch* batch : _listBatches) {
        batch->updateQuads(renderer);
    }
    renderer->flushQuadBuffer();
}

static inline void renderRenderList(RenderList* renderList, Surface* surface, Renderer* renderer) {
    for (auto it=renderList->_ops.begin() ; it!=renderList->_ops.end() ; it++) {
        RenderOp* op = *it;
        if (!op->_shaderValid) {
            continue;
        }
        
        // If private surface then we only draw those ops that need it
        if (surface->_supportsPartialRedraw && !op->_mustRedraw) {
            continue;
        }
        
        // If op not drawn yet, draw it (and as many others in the batch as can be done now)
        if (op->_renderCounter != renderer->_renderCounter) {
            renderer->setCurrentSurface(surface);
            RenderBatch* batch = op->_batch;
            batch->render(renderer, surface, op);
        }
    }

}


void Surface::renderPhase3(Renderer* renderer, View* view, Surface* prevsurf) {
    if (view->_visibility != Visible) {
        return;
    }
    
    Surface* surface = this;
    bool surfaceIsCurrent = view->_surface == surface;
    if (!surfaceIsCurrent) {
        surface = view->_surface;
    }
    

    MATRIX4 savedMatrix;
    bool changesMvp = view->_matrix || !view->_contentOffset.isZero();
    if (changesMvp) {
        savedMatrix = surface->_mvp;
    }
    if (view->_matrix) {
        surface->_mvp *= *view->_matrix;
    }
    if (!view->_contentOffset.isZero()) {
        MATRIX4 tm;
        tm.translate(-view->_contentOffset.x, -view->_contentOffset.y, 0);
        surface->_mvp *= tm;
    }
    
    if (view->_clipsContent) {
        RECT clip = view->getOwnRect();
        clip.origin = view->_surfaceOrigin;
        clip.origin.y = surface->_size.height - clip.bottom(); /* surface -> viewport coords */
        renderer->pushClip(clip);
    }
    
    // Draw view content, if there is any
    if (view->_renderList) {
        renderRenderList(view->_renderList, surface, renderer);
    }
    
    // Recurse subviews
    for (auto it=view->_subviews.begin() ; it != view->_subviews.end() ; it++) {
        surface->renderPhase3(renderer, *it, surface);
    }

    // Draw view decor content, if there is any
    if (view->_renderListDecor) {
        renderRenderList(view->_renderListDecor, surface, renderer);
    }

    // Pop draw state
    if (view->_clipsContent) {
        renderer->popClip();
    }
    if (changesMvp) {
        surface->_mvp = savedMatrix;
    }
    
    // If rendered a child surface then we must now render the child surface onto its parent
    if (!surfaceIsCurrent) {
        if (prevsurf) {
            renderer->setCurrentSurface(prevsurf);
            surface->_op->prepareToRender(renderer, prevsurf);
            if (surface->_supportsPartialRedraw) {
                surface->_invalidRegion.rects.clear();
            }
        }
    }
    
    if (view->_needsFullRedraw) {
        view->_needsFullRedraw = false;
    }
    
    // Update scroll offset if scroller is in use
    view->updateScrollOffsets();
    
}


void Surface::render(View* view, Renderer* renderer) {

    _mvpNum = _mvpNumPeak = 0;
    
    /** PHASE 1: ENSURE ALL RENDER LISTS ARE VALID **/
    _renderListsInsertionPos = _renderListsList.end();
    _renderOrder = 1;
    renderPhase1(renderer, view, {0,0});
    
    /** PHASE 2: VALIDATE ALL SHADERS, ENSURE VERTEX BUFFER IS UP TO DATE */
    renderPhase2(renderer);

    /** PHASE 3: SEND BATCHED RENDEROPS TO GPU **/
    renderPhase3(renderer, view, NULL);
}



void Surface::markPrivateRenderQuadDirty() {
    if (_op) {
        ((PrivateSurfaceRenderOp*)_op._obj)->_dirty = true;
    }
}




