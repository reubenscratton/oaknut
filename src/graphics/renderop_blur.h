//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class GLProgramBlur : public GLProgram {
public:
    GLuint _posTexOffset;
    int _blurRadius;
    float _sigma;
    
    GLProgramBlur(int blurRadius, int sigma);

    virtual void load();
    virtual void setTexOffset(POINT texOffset);
};


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
    void setRect(const RECT& rect) override;
    void asQuads(QUAD *quad) override;
    void render(Window* window, Surface* surface) override;
    void validateShader() override;

};
