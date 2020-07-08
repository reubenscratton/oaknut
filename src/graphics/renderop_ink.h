//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class InkRenderOp : public RectRenderOp {
public:
    
    InkRenderOp();

    void setOrigin(POINT origin);
    void setRadius(float radius);

    void validateShader(RenderTask* r) override;
    void prepareToRender(RenderTask* r, class Surface* surface) override;

    POINT _origin;
    float _radius;
    sp<class Animation> _rippleAnim;
    sp<Animation> _fadeAnim;
};

