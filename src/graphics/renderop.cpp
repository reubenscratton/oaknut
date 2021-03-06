//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



RenderOp::RenderOp() : _view(NULL), _alpha(1.0f) {
}
RenderOp::~RenderOp() {
}

bool RenderOp::canMergeWith(const RenderOp* op) {
    size_t vtable_this = *(size_t*)this;
    size_t vtable_other = *(size_t*)op;
    return vtable_this == vtable_other
        && _shader==op->_shader
        && _blendMode==op->_blendMode
        && _alpha==op->_alpha;
}

void RenderOp::setRect(const RECT& rect) {
    if (!rect.equal(_rect)) {
        _rect = rect;
        if (_batch) {
            invalidateVertexes();
        }
    }
}

int RenderOp::numQuads() {
    return 1;
}

RECT RenderOp::surfaceRect() {
    RECT rect = _rect;
    rect.origin += _view->_surfaceOrigin;
    if (_view->_parent) {
        rect.origin -= _view->_parent->_contentOffsetAccum;
    }
    return rect;
}
bool RenderOp::intersects(RenderOp *op) {
    RECT rect = _rect;
    rect.origin += _view->_surfaceOrigin;
    RECT oprect  = op->_rect;
    oprect.origin += op->_view->_surfaceOrigin;
    return rect.intersects(oprect);
}
/*
bool RenderOpMultiRect::intersects(RenderOp* op) {
    bool trivialTest = RenderOp::intersects(op);
    if (_rects.size()==0 && op->_rects.size()==0) {
        return trivialTest;
    }
    if (trivialTest) {
        if (_rects && op->_rects) { // worst case, have to try every combo to see if it *really* intersects
            for (auto i=_rects->begin() ; i!=_rects->end() ; i++) {
                RECT recti = *i;
                recti.origin += _view->_surfaceOrigin;
                for (auto j=op->_rects->begin() ; j!=op->_rects->end() ; j++) {
                    RECT rectj = *j;
                    rectj.origin += op->_view->_surfaceOrigin;
                    if (recti.intersects(rectj)) {
                        return true;
                    }
                }
            }
        }
        else if (_rects) {
            for (auto i=_rects->begin() ; i!=_rects->end() ; i++) {
                RECT recti = *i;
                recti.origin += _view->_surfaceOrigin;
                if (recti.intersects(oprect)) {
                    return true;
                }
            }
        }
        else {
            for (auto i=op->_rects->begin() ; i!=op->_rects->end() ; i++) {
                RECT recti = *i;
                recti.origin += op->_view->_surfaceOrigin;
                if (recti.intersects(rect)) {
                    return true;
                }
            }
        }
    }
    return false;
}*/

void RenderOp::rectToSurfaceQuad(RECT rect, QUAD* quad) {
    if (!_view->_ownsSurface) {
        rect.origin += _view->_surfaceOrigin;
    }
    *quad = QUAD(rect, _color);
}

void RenderOp::asQuads(QUAD* quad) {
    rectToSurfaceQuad(_rect, quad);
}


#if DEBUG
string debugRect(const RECT& rect) {
    char ach[128];
    sprintf(ach, "%d,%d:%dx%d", (int)rect.origin.x, (int)rect.origin.y,
            (int)rect.size.width,(int)rect.size.height);
    return string(ach);
}

string RenderOp::debugDescription() {
    return debugRect(_rect);
}
#endif

void RenderOp::prepareToRender(RenderTask* r, Surface* surface) {
    assert(_shader);
    r->setCurrentShader(_shader);
    switch (_blendMode) {
        case BLENDMODE_NONE:
            r->setBlendNone();
            break;
        case BLENDMODE_NORMAL:
            r->setBlendNormal();
            break;
        case BLENDMODE_PREMULTIPLIED:
            r->setBlendPremultiplied();
            break;
        default:
            assert(0);
    }
    r->setUniform(_shader->_u_mvp, surface->_mvpR);
}

void RenderOp::invalidateVertexes() {
    if (!_batchGeometryValid) {
        return;
    }
    if (!_batch) {
        return;
    }
    _batchGeometryValid = false;
    _batch->invalidateGeometry(this);
}

void RenderOp::invalidateShader() {
    if (_shader) {
        if (_view->_surface) {
            invalidateBatch();
        }
        _shader = NULL;
    }
}

void RenderOp::invalidateBatch() {
    if (_batch) {
        _view->_surface->unbatchRenderOp(this);
        _view->_surface->_opsNeedingValidation.push_back(this);
    }
}

void RenderOp::setBlendMode(int blendMode) {
    if (_blendMode != blendMode) {
        _blendMode = blendMode;
        invalidateBatch();
    }
}

void RenderOp::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        invalidateBatch();
    }
}
void RenderOp::setColor(COLOR color) {
    if (color != _color) {
        _color = color;
        invalidateVertexes(); // color is a vertex attribute so need to update vertex data
    }
}
void RenderOp::setInset(EDGEINSETS inset) {
    _inset = inset;
    // no need to do anything here, the view code applies the inset when it sets the rect
}

int RenderOp::getRenderOrder() {
    return (_list->_renderListsIndex << 16) | _listIndex;
}

void RenderOp::rebatchIfNecessary() {
    // Rebatch necessary when render properties change
    if (_batch) {
        // This is an unsafe optimization, hence commented out. The op in a single-op batch might be
        // mergeable into other batches, after a property change. Hence we have to always add+remove
        // after a property change. Alternative is to end up with multiple compatible batches, not wanted.
        //if (_batch->_ops.size() > 1) {
            sp<Surface> surface = _batch->_surface;
            surface->detachRenderListOp(this);
            surface->addRenderOp(this);
        //}
    }
}

void RenderOp::reset() {
    // no-op
}
