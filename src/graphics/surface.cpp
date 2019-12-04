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
    void asQuads(QUAD *quad) override {
        rectToSurfaceQuad(_rect, quad);
        quad->tl.s = quad->bl.s = _rectTex.left();
        quad->tr.s = quad->br.s = _rectTex.right();
#if RENDERER_GL
        quad->tl.t = quad->tr.t = _rectTex.bottom();
        quad->bl.t = quad->br.t = _rectTex.top();
#else
        quad->tl.t = quad->tr.t = _rectTex.top();
        quad->bl.t = quad->br.t = _rectTex.bottom();
#endif
    }
    void validateShader(Renderer* renderer) override {
        //TextureRenderOp::validateShader(renderer);
        Shader::Features features;
        features.textures[0] = Texture::Type::Normal;
        features.alpha = (_alpha<1.0f);
        features.tint = (_color!=0);
        _shader = renderer->getStandardShader(features);
    }
    void prepareToRender(Renderer* renderer, Surface* surface) override {
        RenderOp::prepareToRender(renderer, surface);

        if (_dirty) {
            _dirty = false;
            QUAD* quad = (QUAD*)_alloc->addr();
            asQuads(quad);
            renderer->uploadQuad(_alloc);
            renderer->invalidateQuads(_alloc);
        }

        renderer->setCurrentTexture(_view->_surface->_texture);
        renderer->drawQuads(1, _alloc->offset);
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

RenderList::RenderList() : _renderListsIndex(-1) {
}

Surface::Surface(Renderer* renderer, bool isPrivate) : RenderResource(renderer) {
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
    if (list->_renderListsIndex >= 0) {

        for (auto it=list->_ops.begin() ; it!= list->_ops.end() ; it++) {
            detachRenderListOp(*it);
        }
        for (auto it = list->_renderListsPos ; it!=_renderLists.end() ; it++) {
            (*it)->_renderListsIndex--;
        }
        bool isLastTouched = _renderListsLastTouchedIterator == list->_renderListsPos;
        auto newIt = _renderLists.erase(list->_renderListsPos);
        if (isLastTouched) {
            _renderListsLastTouchedIterator = newIt;
        }
        list->_renderListsPos = _renderLists.end();
        list->_renderListsIndex = -1;
    }
    int i=0;
    for (auto& it : _renderLists) {
        assert(i == it->_renderListsIndex);
        i++;
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
        if (op->_shader) {
            opsValid.push_back(op);
            i = _opsNeedingValidation.erase(i);
        }
        else {
            ++i;
        }
    }
    
    // Then embatch them
    for (auto& op : opsValid) {
        assert(op->_shader && !op->_batch);
        batchRenderOp(op);
    }
}

void Surface::detachRenderListOp(RenderOp* op) {
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
    
    int renderOrder = op->getRenderOrder();
    
    // No compatible batch exists, so create it
    if (!batch) {
        batch = new RenderBatch();
        batch->_surface = this;
        batch->_renderBatchListIterator = _listBatches.insert(_listBatches.end(), batch);
    }
    
    // Find the insertion point in the batch that respects the overall render order
    assert(!op->_batch);
    op->_batch = batch;
    batch->_dirty = true;
    for (auto it=batch->_ops.begin() ; it!=batch->_ops.end() ;it++) {
        if (renderOrder < (*it)->getRenderOrder()) {
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
    renderOp->_listIndex = atFront ? 0 : (int)_ops.size();
    if (atFront) {
        for (auto it : _ops) {
            it->_listIndex++;
        }
    }
    renderOp->_listIterator = _ops.insert(atFront ? _ops.begin() : _ops.end(), renderOp);
}
void RenderList::removeRenderOp(RenderOp* renderOp) {
    assert(renderOp->_list == this);
    renderOp->_list = NULL;
    _ops.erase(renderOp->_listIterator);
}

void Surface::ensureRenderListAttached(RenderList* list) {
    if (list->_renderListsIndex < 0) {
        
        // This function is called by renderPhase1 which walks the view tree and tracks the
        // *previous* list entry in _renderListsLastTouchedIterator. When we find
        // a list not yet attached we want to add it *after* the last touched one
        bool isFirst = _renderListsLastTouchedIterator == _renderLists.end();
        list->_renderListsPos = _renderLists.insert(std::next(_renderListsLastTouchedIterator), list);
        if (isFirst) {
            list->_renderListsIndex = 0;
        } else {
            list->_renderListsIndex = (*_renderListsLastTouchedIterator)->_renderListsIndex + 1;
        }
        
        // Increment index of everything after the newly inserted list
        auto tmp = std::next(list->_renderListsPos);
        while (tmp != _renderLists.end()) {
            (*tmp)->_renderListsIndex++;
            tmp++;
        }
        for (auto& it : list->_ops) {
            addRenderOp(it);
        }
    }
    for (auto r : list->_ops) {
        r->_mvpNum = _mvpNum;
    }
    _renderListsLastTouchedIterator = list->_renderListsPos;
    
    int i=0;
    for (auto& it : _renderLists) {
        assert(i == it->_renderListsIndex);
        i++;
    }
}


void Surface::renderPhase1(Renderer* renderer, View* view, RECT surfaceRect) {

    // If view not visible, early exit
    if (view->_visibility != Visible) {
        return;
    }
    
    // If view has a private surface, do some special stuff
    if (view->_ownsSurface) {

        // This is pretty fugly... basically check if we're parent-drawing-child or child-drawing-self.
        if (this != view->_surface) {
            if (!view->_surface) {
                view->_surface = view->_window->_renderer->createPrivateSurface();
            }

            // Render the view tree to its private surface
            view->_surface->render(view, renderer);
            return;
        }
        else {

            // This probably belongs in render().
            bool sizeChanged = (_size.width != view->_rect.size.width || _size.height != view->_rect.size.height);
            if (sizeChanged) {
                setSize(view->_rect.size);
            }

            // Create the private surface op, allocating vertex space from the host surface's vbo
            if (!_op) {
                RECT rect = view->getOwnRect();
                _op = new PrivateSurfaceRenderOp(renderer, view, rect);
            } else {
                if (sizeChanged) {
                    _op->setRect(view->getOwnRect());
                }
            }
        }
    } else {
        if (!view->_surface) {
            view->_surface = this;
        }
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


    // Ensure view's main renderlist is in the surface list
    if (view->_renderList) {
        ensureRenderListAttached(view->_renderList);
    }
    
    // Recurse subviews
    surfaceRect.origin += view->_contentOffset;
    for (auto it = view->_subviews.begin(); it!=view->_subviews.end() ; it++) {
        sp<View>& subview = *it;
        RECT subviewRect = subview->getOwnRect();
        subviewRect.origin += subview->_surfaceOrigin;
        bool subviewIsVisible = subviewRect.intersectWith(surfaceRect);
        if (view->_clipsContents && !subviewIsVisible) {
            if (subview->_surface) {
                subview->detachFromSurface();
            }
            continue;
        }
        renderPhase1(renderer, subview, subviewRect);
    }
    
    // Ensure view decor renderlist is in the surface list
    if (view->_renderListDecor) {
        ensureRenderListAttached(view->_renderListDecor);
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
        if (!op->_shader) {
            continue;
        }
        
        // If private surface then we only draw those ops that need it
        if (surface->_supportsPartialRedraw && !op->_mustRedraw) {
            continue;
        }
        
        // If op not drawn yet, draw it (and as many others in the batch as can be done now)
        if (op->_renderCounter != renderer->_renderCounter) {
            if (surface != renderer->_currentSurface) {
                renderer->setCurrentSurface(surface);
                renderer->_currentSurface = surface;
            }
            RenderBatch* batch = op->_batch;
            assert(batch);
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
    
    bool clips = view->_clipsContents;
    bool didClip = false;
    if (clips) {
        clips = (view->_contentSize.height > view->_rect.size.height)
             || (view->_contentSize.width > view->_rect.size.width);
    }
    clips = false;
    if (clips) {
        RECT clip = view->getOwnRect();
        clip.origin = view->_surfaceOrigin;
        // clip.origin += view->_contentOffsetAccum;
#if RENDERER_GL
        clip.origin.y = surface->_size.height - clip.bottom(); /* flip Y */
#endif
        clip.intersectWith({0,0,_size.width,_size.height});
        if (clip.size.width <= 0 || clip.size.height <= 0) {
            goto skipDraw;
        }
        renderer->pushClip(clip);
        didClip = true;
    }
    
    // Draw view content, if there is any
    if (view->_renderList) {
        renderRenderList(view->_renderList, surface, renderer);
    }
    
    // Recurse subviews
    for (auto it=view->_subviews.begin() ; it != view->_subviews.end() ; it++) {
        if ((*it)->_surface) {
            surface->renderPhase3(renderer, *it, surface);
        }
    }
skipDraw:
    
    // Draw view decor content, if there is any
    if (view->_renderListDecor) {
        renderRenderList(view->_renderListDecor, surface, renderer);
    }

    // Pop draw state
    if (didClip) {
        renderer->popClip();
    }
    if (changesMvp) {
        surface->_mvp = savedMatrix;
    }
    
    // If rendered a child surface then we must now render the child surface onto its parent
    if (!surfaceIsCurrent) {
        if (prevsurf) {
            if (prevsurf != renderer->_currentSurface) {
                renderer->setCurrentSurface(prevsurf);
                renderer->_currentSurface = prevsurf;
            }
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


#if DEBUG


static void debugDump(Surface* surface) {
    static int s_frame=0;
    app->log("Frame %d lists=%d batches=%d", ++s_frame, (int)surface->_renderLists.size(), (int)surface->_listBatches.size());
    for (auto it : surface->_renderLists) {
        RenderList* list = it;
        app->log("> list order=%d size=%d", list->_renderListsIndex, list->_ops.size());
    }
    for (auto it : surface->_listBatches) {
        RenderBatch* batch = it;
        app->log("> batch size=%d", batch->_ops.size());
        for (auto jt : batch->_ops) {
            RenderOp* op = jt;
            string str = op->debugDescription();
            app->log(" op=%s", str.c_str());
        }
    }
    app->log("");
}
#endif

void Surface::checkSanity(View* view, bool dump) {
    list<View*> viewsToCheck = {view};
    int lastValidRenderIndex = -1;
    while (viewsToCheck.size()) {
        View* viewToCheck = *viewsToCheck.begin();
        viewsToCheck.erase(viewsToCheck.begin());
        if (dump) {
#if DEBUG
            string log = viewToCheck->debugDescription();
            if (viewToCheck->_renderList) {
                log.append(string::format(" %d", viewToCheck->_renderList->_renderListsIndex));
            }
            app->log(log.c_str());
#endif
        } else {
            if (viewToCheck->_renderList && -1 != viewToCheck->_renderList->_renderListsIndex) {
                if (viewToCheck->_renderList->_renderListsIndex <= lastValidRenderIndex) {
                    checkSanity(view, true);
                    assert(false);
                }
                lastValidRenderIndex = viewToCheck->_renderList->_renderListsIndex;
            }
        }
        /*if (viewToCheck->_renderListDecor && -1 != viewToCheck->_renderListDecor->_renderListsIndex) {
         assert(viewToCheck->_renderListDecor->_renderListsIndex > lastValidRenderIndex);
         lastValidRenderIndex = viewToCheck->_renderListDecor->_renderListsIndex;
         }*/
        viewsToCheck.insert(viewsToCheck.begin(), viewToCheck->_subviews.begin(), viewToCheck->_subviews.end());
    }

}
void Surface::render(View* view, Renderer* renderer) {

    _mvpNum = _mvpNumPeak = 0;

    /** PHASE 1: ENSURE ALL RENDER LISTS ARE VALID **/
    _renderListsLastTouchedIterator = _renderLists.end();
    RECT surfaceRect = {0, 0, _size.width, _size.height};
    // checkSanity(view, false);
    renderPhase1(renderer, view, surfaceRect);
    // checkSanity(view, false);

    /** PHASE 2: VALIDATE ALL SHADERS, ENSURE VERTEX BUFFER IS UP TO DATE */
    renderPhase2(renderer);
    
    /** PHASE 3: SEND BATCHED RENDEROPS TO GPU **/
    renderPhase3(renderer, view, NULL);
}



void Surface::markPrivateRenderQuadDirty() {
    if (_op) {
        _op.as<PrivateSurfaceRenderOp>()->_dirty = true;
    }
}




