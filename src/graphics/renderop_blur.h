//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class BlurRenderOp : public RenderOp {
public:
	BlurRenderOp();
    ~BlurRenderOp();
    /*
    SIZEI _fullSizePow2;
	SIZEI _downsampledSize;
    bool _vertexesValid;
    MATRIX4 _mvp;
	
    GLuint _textureIds[3];
    GLuint _fb[2];
    ItemPool::Alloc* _alloc;
    
    // Overrides
    void setRect(const RECT& rect) override;
    void asQuads(QUAD *quad) override;
    void render(Renderer* renderer, Surface* surface) override;
*/
    void validateShader(Renderer* renderer) override;
};
