//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

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


Surface::Surface() {
    _supportsPartialRedraw = false;
}


Surface::Surface(View* owningView) : Surface() {

    _supportsPartialRedraw = true;
    _isPrivate = true;
    
}

void Surface::setupPrivateFbo() {

    if (!_tex) {
#ifndef PLATFORM_MACOS
        check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_TYPE, &_pixelType);
        check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_FORMAT, &_pixelFormat);
#endif
        if(_pixelFormat == 0) {
            _pixelFormat = GL_RGBA;
            _pixelType = GL_UNSIGNED_BYTE;
        }
        check_gl(glGenTextures, 1, &_tex);
        check_gl(glGenFramebuffers, 1, &_fb);
    }
    GLint oldFBO, oldTex;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
    check_gl(glGetIntegerv, GL_TEXTURE_BINDING_2D, &oldTex);
    check_gl(glBindTexture, GL_TEXTURE_2D, _tex);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLint pixelFormat = _pixelFormat;
    if (pixelFormat == GL_BGRA) pixelFormat = GL_RGBA; // WebGL no like iOS format
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, pixelFormat, _size.width, _size.height, 0, pixelFormat, _pixelType, NULL);
    check_gl(glBindTexture, GL_TEXTURE_2D, oldTex);
    
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    GLenum x = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(x==GL_FRAMEBUFFER_COMPLETE);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
}

void Surface::setSize(SIZE size) {
    if (_size.width != size.width  ||  _size.height!=size.height) {
        _size = size;
        _mvp = setOrthoFrustum(0, size.width, size.height, 0, -1, 1);
        if (_supportsPartialRedraw) {
            _invalidRegion.rects.clear();
            _invalidRegion.addRect(RECT(0,0,size.width,size.height));
        }
        if (_isPrivate) {
            setupPrivateFbo();
        }
    }
}

void Surface::use() {
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _tex ? _fb :
#if TARGET_OS_IOS || defined(PLATFORM_LINUX)
             1
#else
             0
#endif
             );
    check_gl(glViewport, 0, 0, _size.width, _size.height);
}

void Surface::detachViewOps(View* view) {
    for (auto it=view->_renderList.begin() ; it!= view->_renderList.end() ; it++) {
        removeRenderOp(*it);
    }
}

void Surface::attachViewOps(View* view) {
    for (auto it=view->_renderList.begin() ; it!= view->_renderList.end() ; it++) {
        addRenderOp(*it);
    }
}


/*
 returns  0 if view1==view2;
 returns -1 if view1 renders before view2
 returns +1 if view1 renders after view2
 */
int Surface::renderOrder(View* view1, View* view2) {
    if (view1==view2) return 0;
    for (View* prev=view2->_previousView ; prev ; prev=prev->_previousView) { // surely there's a better/faster way than this?
        if (view1==prev) return -1;
    }
    for (View* next=view2->_nextView ; next ; next=next->_nextView) {
        if (view1==next) return 1;
    }
    return NAN;
}

void Surface::addRenderOp(RenderOp* op) {
    op->_mustRedraw = true;
    assert(!op->_batch);
    _opsNeedingValidation.push_back(op);
}

void Surface::validateRenderOps() {
    if (!_opsNeedingValidation.size()) {
        return;
    }
    
    // First validate any ops that need it
    list<RenderOp*> opsValid;
    auto i = _opsNeedingValidation.begin();
    while (i != _opsNeedingValidation.end()) {
        RenderOp* op = (*i);
        op->validateShader();
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
    for (auto it=_listBatches.begin() ; it!=_listBatches.end() ; it++) {
        RenderOp* existingBatchedOp = *((*it)->_ops.begin());
        if (existingBatchedOp->canMergeWith(op)) {
            batch = *it;
            break;
        }
    }
    
    // No compatible batch exists, so create it
    if (!batch) {
        batch = new RenderBatch();
        batch->_surface = this;
        batch->_renderBatchListIterator = _listBatches.insert(_listBatches.end(), batch);
    }
    
    // Find the insertion point in the batch so we respect view order
    assert(!op->_batch);
    op->_batch = batch;
    batch->_dirty = true;
    for (auto it=batch->_ops.begin() ; it!=batch->_ops.end() ;it++) {
        if (renderOrder(op->_view, (*it)->_view) <0) {
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



void Surface::renderPhase1(View* view, Window* window, POINT origin) {

    if (view->_visibility != Visible) {
        return;
    }
    Surface* surface = this;

    // If view has a private surface, ensure surface has same size as view
    bool usesPrivateSurface = (view->_surface != surface);
    if (usesPrivateSurface) {
        bool sizeChanged = false;
        if (view->_surface->_size.width != view->_rect.size.width || view->_surface->_size.height != view->_rect.size.height) {
            view->_surface->setSize(view->_rect.size);
            sizeChanged = true;
        }
        
        // Create the private surface op, allocating vertex space from the host surface's vbo
        if (!view->_surface->_op) {
            RECT rect = view->getOwnRect();
            view->_surface->_op = new PrivateSurfaceRenderOp(view, rect);
            view->_surface->_op->_alloc = window->_quadBuffer->alloc(1, NULL);
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
    for (auto r:view->_renderList) {
        r->_mvpNum = _mvpNum;
    }
    
    // Give the view a chance to update its renderOps now that layout is complete
    if (view->_updateRenderOpsNeeded) {
        view->updateRenderOps();
        view->_updateRenderOpsNeeded = false;
    }
        
    // Recurse
    for (auto it = view->_subviews.begin(); it!=view->_subviews.end() ; it++) {
        sp<View>& subview = *it;
        surface->renderPhase1(subview, window, origin);
    }
 
    if (usesPrivateSurface) {
        surface->validateRenderOps();
    }

    if (changesMvp) {
        _mvpNum = mvpNumToRestore;
    }

}


PrivateSurfaceRenderOp::PrivateSurfaceRenderOp(View* view, const RECT& rect)  : TextureRenderOp(rect, NULL, NULL, 0) {
    _view = view;
    _dirty = true;
    validateShader();
}
PrivateSurfaceRenderOp::~PrivateSurfaceRenderOp() {
    if (_alloc) {
        _alloc->pool->free(_alloc);
        _alloc = NULL;
    }
}
void PrivateSurfaceRenderOp::validateShader() {
    TextureRenderOp::validateShader();
    _shaderValid = true;
}
void PrivateSurfaceRenderOp::rectToSurfaceQuad(RECT rect, QUAD* quad) {
    rect.origin += _view->_surfaceOrigin;
    *quad = QUAD(rect, 0);
}
void PrivateSurfaceRenderOp::render(Window* window, Surface* surface) {
    RenderOp::render(window, surface);
    
    // Bind to the private surface texture
    window->_currentTexture = NULL;
    check_gl(glBindTexture, GL_TEXTURE_2D, _view->_surface->_tex);
    
    if (_dirty) {
        _dirty = false;
        QUAD* quad = (QUAD*)_alloc->addr();
        asQuads(quad);
        check_gl(glBufferSubData, GL_ARRAY_BUFFER, _alloc->offset*sizeof(QUAD), _alloc->count*sizeof(QUAD), _alloc->addr());
    }
    check_gl(glDrawElements, GL_TRIANGLES, 6 * 1, GL_UNSIGNED_SHORT, (void*)((_alloc->offset)*6*sizeof(GLshort)));
}


void Surface::renderPhase2(Surface* prevsurf, View* view, Window* window) {
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
        window->pushClip(clip);
    }
    
    // Walk the ops for the current view
    for (auto it=view->_renderList.begin() ; it!=view->_renderList.end() ; it++) {
        RenderOp* op = *it;
        if (!op->_shaderValid) {
            continue;
        }
        
        // If private surface then we only draw those ops that need it
        if (surface->_supportsPartialRedraw && !op->_mustRedraw) {
            continue;
        }
        
        // If op not drawn yet, draw it (and as many others in the batch as can be done now)
        if (op->_renderCounter != window->_renderCounter) {
            window->setCurrentSurface(surface);
            RenderBatch* batch = op->_batch;
            batch->render(window, surface, op);
        }
    }
    
    // Recurse subviews
    for (auto it=view->_subviews.begin() ; it != view->_subviews.end() ; it++) {
        surface->renderPhase2(surface, *it, window);
    }
    
    // Pop draw state
    if (view->_clipsContent) {
        window->popClip();
    }
    if (changesMvp) {
        surface->_mvp = savedMatrix;
    }
    
    // If rendered a child surface then we must now render the child surface onto its parent
    if (!surfaceIsCurrent) {
        if (prevsurf) {
            window->setCurrentSurface(prevsurf);
            surface->_op->render(window, prevsurf);
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


void Surface::render(View* view, Window* window) {

    _renderInProgress = true;
    
    _mvpNum = _mvpNumPeak = 0;
    
    /** PHASE 1: ENSURE ALL RENDER LISTS ARE VALID **/
    renderPhase1(view, window, {0,0});
    
    validateRenderOps();
    
    /** PHASE 2: THE ACTUAL OPENGL DRAW COMMANDS **/
    renderPhase2(NULL, view, window);
    
    //app.log("batch count:%d", _listBatches.size());

    _renderInProgress = false;
}



Surface::~Surface() {
    cleanup();
}

void Surface::cleanup() {
    if (_tex) {
        check_gl(glDeleteTextures, 1, &_tex);
        _tex = 0;
        check_gl(glDeleteFramebuffers, 1, &_fb);
        _fb = 0;
    }
}





