//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup widgets
 * @brief A button with a text label. Or, more accurately, a `Label` with a button-style background.
 */
class Button : public Label {
public:
    
    Button();
    
    void setImage(const string& assetName);
    void setImage(Bitmap* bitmap);
    
    bool applySingleStyle(const string &name, const style &value) override;
    void setTextColor(COLOR color) override;
    void updateIntrinsicSize(SIZE constrainingSize) override;
    void layout(RECT constraint) override;
    RECT getTextRectForLayout() override;
    
protected:
    sp<Task> _imageTask;
    TextureRenderOp* _imageOp;
    bool _iconTint;
    float _imageSpacing;
    
#ifdef DEBUG
    string debugViewType() override;
#endif

};

/**
 * @ingroup widgets
 * @brief A button with an image.
 */
class ToolbarButton : public ImageView {
public:
    
    ToolbarButton();
    
    bool handleInputEvent(INPUTEVENT* event) override;
    
};


