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

    // Overrides
    void setRect(const RECT& rect) override;
    void asQuads(QUAD *quad) override;
    void validateShader(Renderer* renderer) override;
    void prepareToRender(Renderer* renderer, Surface* surface) override;
    
    // These are also in PrivateSurfaceRenderOp...
    bool _dirty;
    ItemPool::Alloc* _alloc;
    
    SIZEI _fullSizePow2;
    SIZEI _downsampledSize;
    MATRIX4 _mvp;    
    sp<Texture> _tex1;
    sp<Surface> _surface1;
    sp<Surface> _surface2;
    sp<Shader> _blurShader;
    MATRIX4* _pmvp;
    

};


