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
    
    PrivateSurfaceRenderOp(RenderTask* r, View* view, const RECT& rect) : TextureRenderOp(rect, NULL, NULL, 0) {
        _view = view;
        _alloc = r->_renderer->allocQuads(1, NULL);
        _dirty = true;
        validateShader(r);
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
//#if RENDERER_GL
//        quad->tl.t = quad->tr.t = _rectTex.bottom();
//        quad->bl.t = quad->br.t = _rectTex.top();
//#else
        quad->tl.t = quad->tr.t = _rectTex.top();
        quad->bl.t = quad->br.t = _rectTex.bottom();
//#endif
    }
    void validateShader(RenderTask* r) override {
        //TextureRenderOp::validateShader(renderer);
        RectShader::Features features;
        features.tex0 = Texture::Type::Normal;
        features.alpha = (_alpha<1.0f);
        features.tint = (_color!=0);
        _shader = RectShader::get(r->_renderer, features);
    }
    void prepareToRender(RenderTask* r, Surface* surface) override {
        RenderOp::prepareToRender(r, surface);

        if (_dirty) {
            _dirty = false;
            QUAD* quad = (QUAD*)_alloc->addr();
            asQuads(quad);
            r->_renderer->uploadQuad(_alloc);
            r->_renderer->invalidateQuads(_alloc);
        }

        r->setCurrentTexture(_view->_surface->_texture);
        r->draw(Quad, 1, _alloc->offset);
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


RenderList::RenderList() : _renderListsIndex(-1) {
}

Surface::Surface(Renderer* renderer, bool isPrivate) : RenderResource(renderer) {
    _supportsPartialRedraw = isPrivate;
    _isPrivate = isPrivate;
}


void Surface::setSize(const SIZE& size) {
    _size = size;
    _mvpR = MATRIX4::makeOrtho(0, size.width, size.height, 0, -1, 1);
    _mvpP.identity();
    if (_supportsPartialRedraw) {
        _invalidRegion.rects.clear();
        _invalidRegion.addRect(RECT(0,0,size.width,size.height));
    }
    if (_texture) {
        _texture->setSize({static_cast<int>(size.width), static_cast<int>(size.height)});
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
    /*int i=0;
    for (auto& it : _renderLists) {
        assert(i == it->_renderListsIndex);
        i++;
    }*/
}



void Surface::addRenderOp(RenderOp* op) {
    assert(op->_view);
    op->_mustRedraw = true;
    assert(!op->_batch);
    _opsNeedingValidation.push_back(op);
}

void Surface::validateRenderOps(RenderTask* r) {
    if (!_opsNeedingValidation.size()) {
        return;
    }
    
    // First validate any ops that need it
    list<RenderOp*> opsValid;
    auto i = _opsNeedingValidation.begin();
    while (i != _opsNeedingValidation.end()) {
        RenderOp* op = (*i);
        op->validateShader(r);
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


void Surface::renderPhase1(RenderTask* r, View* view, RECT surfaceRect) {

    // If view not visible, early exit
    if (view->_visibility != Visible) {
        return;
    }
    
    // If view has a private surface, do some special stuff
    if (view->_ownsSurface) {

        // This is pretty fugly... basically check if we're parent-drawing-child or child-drawing-self.
        if (this != view->_surface) {
            if (!view->_surface) {
                view->_surface = view->_window->_renderer->createSurface(true);
                view->attachToSurface();
            }

            // Render the view tree to its private surface
            view->_surface->render(view, r);
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
                _op = new PrivateSurfaceRenderOp(r, view, rect);
            } else {
                if (sizeChanged) {
                    _op->setRect(view->getOwnRect());
                }
            }
        }
    } else {
        if (!view->_surface) {
            view->_surface = this;
            view->attachToSurface();
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
        renderPhase1(r, subview, subviewRect);
    }
    
    // Ensure view decor renderlist is in the surface list
    if (view->_renderListDecor) {
        ensureRenderListAttached(view->_renderListDecor);
    }


    if (changesMvp) {
        _mvpNum = mvpNumToRestore;
    }

}

void Surface::renderPhase2(RenderTask* r) {
    validateRenderOps(r);
    for (RenderBatch* batch : _listBatches) {
        batch->updateQuads(r->_renderer);
    }
    r->_renderer->flushQuadBuffer();
}

static inline void renderRenderList(RenderList* renderList, Surface* surface, RenderTask* r, int renderCounter) {
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
        if (op->_renderCounter != renderCounter) {
            r->setCurrentSurface(surface);
            RenderBatch* batch = op->_batch;
            assert(batch);
            batch->render(r, surface, op, renderCounter);
        }
    }

}


void Surface::renderPhase3(RenderTask* r, View* view, Surface* prevsurf) {
    if (view->_visibility != Visible) {
        return;
    }
    
    Surface* surface = this;
    bool surfaceIsCurrent = view->_surface == surface;
    if (!surfaceIsCurrent) {
        surface = view->_surface;
    }
    

    MATRIX4 savedMvpR, savedMvpP;
    bool changesMvp = view->_matrix || !view->_contentOffset.isZero();
    if (changesMvp) {
        savedMvpR = surface->_mvpR;
        savedMvpP = surface->_mvpP;
    }
    if (view->_matrix) {
        surface->_mvpR *= *view->_matrix;
        surface->_mvpP *= *view->_matrix;
    }
    
    // Apply content offset to the renderspace matrix
    if (!view->_contentOffset.isZero()) {
        MATRIX4 tm = MATRIX4::makeTranslate(-(int)view->_contentOffset.x, -(int)view->_contentOffset.y);
        surface->_mvpR *= tm;
        //surface->_mvpP *= tm; not done since getSurfaceRect() below tracks content offset changes
    }
    
    bool clips = view->_clipsContents;
    bool didClip = false;
    if (clips) {
        clips = (view->_contentSize.height > view->_rect.size.height)
             || (view->_contentSize.width > view->_rect.size.width);
    }
    //clips = false;
    if (clips) {
        // Get the clip rect without any animation (mvp) transforms. 
        RECT clip = view->getSurfaceRect();
        // Apply current mvp (pixelspace) transform
        clip.transform(surface->_mvpP);
        // Clamp to surface area
        clip.intersectWith({0,0,_size.width,_size.height});
        if (clip.size.width <= 0 || clip.size.height <= 0) {
            goto skipDraw;
        }
        r->pushClip(clip);
        didClip = true;
    }
    
    // Draw view content, if there is any
    if (view->_renderList) {
        renderRenderList(view->_renderList, surface, r, view->_window->_renderCounter);
    }
    
    // Recurse subviews
    for (auto it=view->_subviews.begin() ; it != view->_subviews.end() ; it++) {
        if ((*it)->_surface) {
            surface->renderPhase3(r, *it, surface);
        }
    }
skipDraw:
    
    // Draw view decor content, if there is any
    if (view->_renderListDecor) {
        renderRenderList(view->_renderListDecor, surface, r, view->_window->_renderCounter);
    }

    // Pop draw state
    if (didClip) {
        r->popClip();
    }
    if (changesMvp) {
        surface->_mvpR = savedMvpR;
        surface->_mvpP = savedMvpP;
    }
    
    // If rendered a child surface then we must now render the child surface onto its parent
    if (!surfaceIsCurrent) {
        if (prevsurf) {
            r->setCurrentSurface(prevsurf);
            surface->_op->prepareToRender(r, prevsurf);
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


/*
#if DEBUG
static void debugDump(Surface* surface) {
    static int s_frame=0;
    log_info("Frame %d lists=%d batches=%d", ++s_frame, (int)surface->_renderLists.size(), (int)surface->_listBatches.size());
    for (auto it : surface->_renderLists) {
        RenderList* list = it;
        log_info("> list order=%d size=%d", list->_renderListsIndex, list->_ops.size());
    }
    for (auto it : surface->_listBatches) {
        RenderBatch* batch = it;
        log_info("> batch size=%d", batch->_ops.size());
        for (auto jt : batch->_ops) {
            RenderOp* op = jt;
            string str = op->debugDescription();
            log_info(" op=%s", str.c_str());
        }
    }
    log_info("");
}
#endif
*/
void Surface::checkSanity(View* view, bool dump) {
    list<View*> viewsToCheck = {view};
    int lastValidRenderIndex = -1;
    while (viewsToCheck.size()) {
        View* viewToCheck = *viewsToCheck.begin();
        viewsToCheck.erase(viewsToCheck.begin());
        if (dump) {
#if DEBUG
            string logmsg = viewToCheck->debugDescription();
            if (viewToCheck->_renderList) {
                logmsg.append(string::format(" %d", viewToCheck->_renderList->_renderListsIndex));
            }
            log_dbg(logmsg.c_str());
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
void Surface::render(View* view, RenderTask* r) {

    _mvpNum = _mvpNumPeak = 0;

    /** PHASE 1: ENSURE ALL RENDER LISTS ARE VALID **/
    _renderListsLastTouchedIterator = _renderLists.end();
    RECT surfaceRect = {0, 0, _size.width, _size.height};
    // checkSanity(view, false);
    renderPhase1(r, view, surfaceRect);
    // checkSanity(view, false);

    /** PHASE 2: VALIDATE ALL SHADERS, ENSURE VERTEX BUFFER IS UP TO DATE */
    renderPhase2(r);
    
    /** PHASE 3: SEND BATCHED RENDEROPS TO GPU **/
    renderPhase3(r, view, NULL);
}



void Surface::markPrivateRenderQuadDirty() {
    if (_op) {
        _op.as<PrivateSurfaceRenderOp>()->_dirty = true;
    }
}




