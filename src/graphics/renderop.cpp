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
    return _prog==op->_prog && _blendMode==op->_blendMode && _alpha==op->_alpha;
}

void RenderOp::setRect(const RECT& rect) {
    if (!rect.equal(_rect)) {
        _rect = rect;
        if (_batch) {
            _batch->invalidateGeometry(this);
        }
    }
}

int RenderOp::numQuads() {
    return 1;
}

RECT RenderOp::surfaceRect() {
    RECT rect = _rect;
    rect.origin += _view->_surfaceOrigin;
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
    if (!_view->_ownsPrivateSurface) {
        rect.origin += _view->_surfaceOrigin;
    }
    *quad = QUADFromRECT(rect, _color);
}

void RenderOp::asQuads(QUAD* quad) {
    rectToSurfaceQuad(_rect, quad);
}


string debugRect(const RECT& rect) {
    char ach[128];
    sprintf(ach, "%d,%d:%dx%d", (int)rect.origin.x, (int)rect.origin.y,
            (int)rect.size.width,(int)rect.size.height);
    return string(ach);
}


void RenderOp::render(Window* window, Surface* surface) {
    window->setBlendMode(_blendMode);
    _prog->use(window);
    _prog->setMvp(surface->_mvp);
    _prog->setAlpha(_alpha);
}

void RenderOp::invalidateBatchGeometry() {
    if (!_batchGeometryValid) {
        return;
    }
    if (!_batch) {
        return;
    }
    _batchGeometryValid = false;
    _batch->invalidateGeometry(this);
}

void RenderOp::invalidate() {
    if (_shaderValid) {
        _shaderValid = false;
        if (_view->_surface) {
            if (_batch) {
                _view->_surface->unbatchRenderOp(this);
            }
            _view->_surface->_opsNeedingValidation.push_back(this);
        }
    }
}
void RenderOp::setBlendMode(int blendMode) {
    if (_blendMode != blendMode) {
        _blendMode = blendMode;
        invalidate();
    }
}

void RenderOp::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        invalidate();
    }
}
void RenderOp::setColor(COLOR color) {
    if (color != _color) {
        _color = color;
        invalidate();  // hmmm... color is an attribute now, can we remove this?
    }
}
void RenderOp::setInset(EDGEINSETS inset) {
    _inset = inset;
    // no need to do anything here, the view code applies the inset when it sets the rect
}

void RenderOp::rebatchIfNecessary() {
    // Rebatch necessary when render properties change
    if (_batch) {
        // This is an unsafe optimization, hence commented out. The op in a single-op batch might be
        // mergeable into other batches, after a property change. Hence we have to always add+remove
        // after a property change. Alternative is to end up with multiple compatible batches, not wanted.
        //if (_batch->_ops.size() > 1) {
            ObjPtr<Surface> surface = _batch->_surface;
            surface->removeRenderOp(this);
            surface->addRenderOp(this);
        //}
    }
}

