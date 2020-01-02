//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup widgets
 * @brief
 */
class TabBar : public LinearLayout {
public:
    
    /**  @cond INTERNAL */
    TabBar();
    /**  @endcond */
    
    /** @name Properties
     * @{
     */
    /** @} */
    
    /** @name Overrides
     * @{
     */
    /** @} */
    bool applySingleStyle(const string& name, const style& value) override;

    virtual void setSelectedIndex(int buttonIndex);
    std::function<void(int)> onSelectedIndexSet;
    std::function<void(int,View*)> onButtonClick;

#ifdef DEBUG
    string debugViewType() override;
#endif
    
protected:
    virtual void handleButtonClick(int buttonIndex);
    
    string _buttonClassName;
    COLOR _selectedTint;
    int _selectedIndex;
};


