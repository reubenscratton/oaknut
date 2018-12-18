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


