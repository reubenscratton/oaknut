//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup widgets
 * @brief A checkbox is a View that toggles it's `STATE_CHECKED` bit when pressed. Some special shaders
 * provide a close simulacrum to platform equivalents. 
 */
class Checkbox : public View {
public:
    Checkbox();
    
    std::function<void(View*,bool)> onIsCheckedChanged;
    
protected:
    bool applySingleStyle(const string &name, const style &value) override;
    bool handleInputEvent(INPUTEVENT* event) override;
    void onStateChanged(VIEWSTATE changes) override;
    void layout(RECT constraint) override;
    void updateRenderOps() override;

    RectRenderOp* _renderOpBox;
    sp<RenderOp> _renderOpTick;
    COLOR _boxColor;
};
