//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Canvas : public Object {
public:
    int _doneGlInit;
    QuadBuffer* _quadBuffer;

    // GL state
    struct {
        //GLint blend:1;
        GLint scissorTest:1;
    } _enabledFlags;
    //GLenum _blendFuncSFactor, _blendFuncDFactor;
    void setBlendMode(int blendMode);
    void glEnableScissorTest(bool enabled);
    //void glBlendFunc(GLenum sfactor, GLenum dfactor);
    void bindTexture(Bitmap* texture);
    
    GLuint _currentProg;
    Surface* _currentSurface;
    Bitmap* _currentTexture;
    int _renderCounter;
    int _currentVertexConfig;
    int _blendMode;
    
	Canvas();

	void prepareToDraw();
    void setCurrentSurface(Surface* surface);
    void setVertexConfig(int vertexConfig);
    
};


