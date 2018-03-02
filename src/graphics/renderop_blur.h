//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class BlurRenderOp : public RenderOp {
public:
	BlurRenderOp(View* view);
    ~BlurRenderOp();
    
    SIZEI _fullSizePow2;
	SIZEI _downsampledSize;
    bool _vertexesValid;
    Matrix4 _mvp;
	
    GLuint _textureIds[3];
    GLuint _fb[2];
    ItemPool::Alloc* _alloc;
    
    // Overrides
    virtual void setRect(const RECT& rect);
    virtual void asQuads(QUAD *quad);
    virtual void render(Window* window, Surface* surface);
    
};
