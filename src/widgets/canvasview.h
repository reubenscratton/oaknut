//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup widgets
 * @brief A view that draws a `Canvas`. Implement the canvas drawing code in the `redraw` method.
 */
class CanvasView : public View {
public:

    /**  @cond INTERNAL */
    CanvasView();
    /**  @endcond */
    
    // Overrides
    void layout(RECT constraint) override;
    
protected:
    virtual void redraw();
    
    sp<Canvas> _canvas;
    sp<TextureRenderOp> _textureRenderOp;
};


