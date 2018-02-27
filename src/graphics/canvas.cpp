//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


Canvas::Canvas() {
    _quadBuffer = new QuadBuffer();
}


void Canvas::setBlendMode(int blendMode) {
    if (blendMode != _blendMode) {
        if (blendMode == BLENDMODE_NONE) {
            check_gl(glDisable, GL_BLEND);
        } else {
            if (blendMode == BLENDMODE_NORMAL) {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // normal alpha blend
            } else {
                glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
            }
            if (_blendMode == BLENDMODE_NONE) {
                check_gl(glEnable, GL_BLEND);
            }
        }
        _blendMode = blendMode;
    }
}
void Canvas::glEnableScissorTest(bool enabled) {
    if (enabled && !_enabledFlags.scissorTest) {
        _enabledFlags.scissorTest = 1;
        check_gl(glEnable, GL_SCISSOR_TEST);
    }
    else if (!enabled && _enabledFlags.scissorTest) {
        _enabledFlags.scissorTest = 0;
        check_gl(glDisable, GL_SCISSOR_TEST);
    }
}


void Canvas::prepareToDraw() {
	// GL context init
	if (!_doneGlInit) {
		_doneGlInit = 1;
        check_gl(glDepthMask, GL_TRUE);
        check_gl(glClear, GL_DEPTH_BUFFER_BIT);
        check_gl(glDepthMask, GL_FALSE);
        check_gl(glClearColor, 1,0,0,1);
        check_gl(glClear, GL_COLOR_BUFFER_BIT);
        check_gl(glDisable, GL_DEPTH_TEST);
        check_gl(glActiveTexture, GL_TEXTURE0);
        _blendMode = BLENDMODE_NONE;
        check_gl(glDisable, GL_BLEND);
        _enabledFlags.scissorTest = 0;
        check_gl(glDisable, GL_SCISSOR_TEST);

        // As long as we only have one quadbuffer we only need to bind the once
        _quadBuffer->bind();
        _currentVertexConfig = 0;
	}

    _renderCounter++;
    _currentSurface = NULL;
    _currentProg = 0;
}

void Canvas::setCurrentSurface(Surface* surface) {
    if (surface != _currentSurface) {
        _currentSurface = surface;
        _currentSurface->use();
    }
}

void Canvas::setVertexConfig(int vertexConfig) {
    _currentVertexConfig = vertexConfig;
    check_gl(glVertexAttribPointer, VERTEXATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
    check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)8);
//    check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VERTEX), (void*)8);
    check_gl(glVertexAttribPointer, VERTEXATTRIB_COLOUR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), (void*)16);
    
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_POSITION);
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_TEXCOORD);
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_COLOUR);

}

void Canvas::bindTexture(Bitmap* texture) {
    if (texture&& _currentTexture != texture) {
        _currentTexture = texture;
        texture->bind();
    }

}

