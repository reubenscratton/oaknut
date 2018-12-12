//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup widgets
 * @brief A checkbox is an ImageView that toggles it's `STATE_CHECKED` bit when tapped.
 */
class Checkbox : public ImageView {
public:
    Checkbox();
    
    std::function<void(View*,bool)> onIsCheckedChanged;
    
protected:
    bool handleInputEvent(INPUTEVENT* event) override;
    void onStateChanged(STATESET changes) override;

};
