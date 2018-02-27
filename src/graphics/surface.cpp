//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"



Matrix4 setOrthoFrustum(float l, float r, float b, float t, float n, float f) {
    Matrix4 mat;
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

#if TARGET_OS_OSX
    _pixelFormat = GL_RGBA;
    _pixelType = GL_UNSIGNED_BYTE;
#else
    check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_TYPE, &_pixelType);
    check_gl(glGetIntegerv, GL_IMPLEMENTATION_COLOR_READ_FORMAT, &_pixelFormat);
#endif
    check_gl(glGenTextures, 1, &_tex);
    check_gl(glGenFramebuffers, 1, &_fb);
}

void Surface::setupPrivateFbo() {
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
    assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
}

void Surface::setSize(SIZE size) {
    if (_size.width != size.width  ||  _size.height!=size.height) {
        _size = size;
        _mvp = setOrthoFrustum(0, size.width, size.height, 0, -1, 1);
        if (_supportsPartialRedraw) {
            _invalidRegion.rects.clear();
            _invalidRegion.addRect(RECT_Make(0,0,size.width,size.height));
        }
        if (_tex) {
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
int renderOrder(View* view1, View* view2) {
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
    
    // Find the compatible batch
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
        RenderOp* op2 = *it;
        if (renderOrder(op->_view, op2->_view) <0) {
            op->_batchIterator = batch->_ops.insert(it, op);
            return;
        }
        
    }
    op->_batchIterator = batch->_ops.insert(batch->_ops.end(), op);
}

void Surface::removeRenderOp(RenderOp* op) {
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



static void renderPhase1(Surface* surface, View* view, Canvas* canvas, POINT origin) {

    if (view->_visibility != VISIBILITY_VISIBLE) {
        return;
    }
    
    // If view has a private surface, ensure surface has same size as view
    if (view->_surface != surface) {
        bool sizeChanged = false;
        if (view->_surface->_size.width != view->_frame.size.width || view->_surface->_size.height != view->_frame.size.height) {
            view->_surface->setSize(view->_frame.size);
            sizeChanged = true;
        }
        
        // Create the private surface op, allocating vertex space from the host surface's vbo
        if (!view->_surface->_op) {
            RECT rect = view->getBounds();
            view->_surface->_op = new PrivateSurfaceRenderOp(view, rect);
            view->_surface->_op->_alloc = canvas->_quadBuffer->alloc(1, NULL);
        } else {
            if (sizeChanged) {
                view->_surface->_op->setRect(view->getBounds());
            }
        }

        // Reset origin
        origin = POINT_Make(0,0);
        surface = view->_surface;

    } else {

        // Adjust origin
        origin.x += view->_frame.origin.x;
        origin.y += view->_frame.origin.y;
    }
    
    // If view's content size is invalid, now's a great time to update it
    if (!view->_contentSizeValid) {
        assert(view->_widthMeasureSpec.refType!=REFTYPE_CONTENT); // shoulda been done in measure()!
        assert(view->_heightMeasureSpec.refType!=REFTYPE_CONTENT);
        float parentWidth = view->_parent?view->_parent->_frame.size.width : view->_window->_surfaceRect.size.width;
        float parentHeight = view->_parent?view->_parent->_frame.size.height : view->_window->_surfaceRect.size.height;
        view->updateContentSize(parentWidth, parentHeight);
        view->_contentSizeValid = true;
    }
    
    bool changesMvp = view->_matrix || view->_contentOffset.y != 0.0f;
    if (changesMvp) {
        surface->_mvpNum++;
    }
    for (auto r:view->_renderList) {
        r->_mvpNum = surface->_mvpNum;
    }
    
    // Give the view a chance to update its renderOps now that layout is complete
    if (view->_updateRenderOpsNeeded) {
        view->updateRenderOps();
        view->_updateRenderOpsNeeded = false;
    }
        
    // Recurse
    for (auto it = view->_subviews.begin(); it!=view->_subviews.end() ; it++) {
        ObjPtr<View>& subview = *it;
        renderPhase1(surface, subview, canvas, origin);
    }
 
    if (changesMvp) {
        surface->_mvpNum--;
    }

}


PrivateSurfaceRenderOp::PrivateSurfaceRenderOp(View* view, const RECT& rect)  : TextureRenderOp(view, rect, NULL, NULL, 0) {
    _dirty = true;
}
PrivateSurfaceRenderOp::~PrivateSurfaceRenderOp() {
    if (_alloc) {
        _alloc->pool->free(_alloc);
        _alloc = NULL;
    }
}
void PrivateSurfaceRenderOp::rectToSurfaceQuad(RECT rect, QUAD* quad) {
    rect.origin += _view->_surfaceOrigin;
    *quad = QUADFromRECT(rect, 0);
}
void PrivateSurfaceRenderOp::render(Canvas* canvas, Surface* surface) {
    RenderOp::render(canvas, surface);
   // _prog->setTintColour(_tintColour);
    
    // Bind to the private surface texture
    canvas->_currentTexture = NULL;
    check_gl(glBindTexture, GL_TEXTURE_2D, _view->_surface->_tex);
    
    if (_dirty) {
        _dirty = false;
        QUAD* quad = (QUAD*)_alloc->addr();
        asQuads(quad);
        check_gl(glBufferSubData, GL_ARRAY_BUFFER, _alloc->offset*sizeof(QUAD), _alloc->count*sizeof(QUAD), _alloc->addr());
    }
    check_gl(glDrawElements, GL_TRIANGLES, 6 * 1, GL_UNSIGNED_SHORT, (void*)((_alloc->offset)*6*sizeof(GLshort)));
}


static void renderPhase2(Surface* surface, View* view, Canvas* canvas) {
    if (view->_visibility != VISIBILITY_VISIBLE) {
        return;
    }
    
    Surface* prevsurf = surface;
    bool surfaceIsCurrent = view->_surface == surface;
    if (!surfaceIsCurrent) {
        surface = view->_surface;
    }
    

    Matrix4 savedMatrix;
    bool changesMvp = view->_matrix || view->_contentOffset.y != 0.0f;
    if (changesMvp) {
        savedMatrix = surface->_mvp;
    }
    if (view->_matrix) {
        surface->_mvp *= *view->_matrix;
    }
    if (view->_contentOffset.y != 0.0f) {
        Matrix4 tm;
        tm.translate(0, -view->_contentOffset.y, 0);
        surface->_mvp *= tm;
    }

    // Walk the ops for the current view
    for (auto it=view->_renderList.begin() ; it!=view->_renderList.end() ; it++) {
        RenderOp* op = *it;
        
        // If private surface then we only draw those ops that need it
        if (surface->_supportsPartialRedraw && !op->_mustRedraw) {
            continue;
        }
        
        // If op not drawn yet, draw it (and as many others in the batch as can be done now)
        if (op->_renderCounter != canvas->_renderCounter) {
            canvas->setCurrentSurface(surface);
            RenderBatch* batch = op->_batch;
            batch->render(canvas, surface, op);
        }
    }
    
    // Recurse subviews
    for (auto it=view->_subviews.begin() ; it != view->_subviews.end() ; it++) {
        renderPhase2(surface, *it, canvas);
    }
        
    // Pop draw state
    if (changesMvp) {
        surface->_mvp = savedMatrix;
    }
    
    // If rendered a child surface then we must now render the child surface onto its parent
    if (!surfaceIsCurrent) {
        if (prevsurf) {
            canvas->setCurrentSurface(prevsurf);
            surface->_op->render(canvas, prevsurf);
            if (surface->_supportsPartialRedraw) {
                surface->_invalidRegion.rects.clear();
            }
        }
    }
    
    if (view->_needsFullRedraw) {
        view->_needsFullRedraw = false;
    }
    
    // Update scroll offset if scroller is in use
    if (view->_scrollbarVert) {
        if (!view->_scrollbarVert->mFinished) {
            view->_scrollbarVert->computeScrollOffset();
            view->setContentOffset(POINT_Make(0, view->_scrollbarVert->mCurr));
        }
    }
    
}


void Surface::render(View* view, Canvas* canvas) {

    _mvpNum = 0;
    
    /** PHASE 1: ENSURE ALL RENDER LISTS ARE VALID **/
    renderPhase1(this, view, canvas, POINT_Make(0,0));
    
    /** PHASE 2: THE ACTUAL OPENGL DRAW COMMANDS **/
    renderPhase2(NULL, view, canvas);
    
    //oakLog("batch count:%d", _listBatches.size());
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





