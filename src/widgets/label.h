//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup widgets
 * @brief Displays a piece of text, potentially with attributes (bold, color, etc).
 */
class Label : public View {
public:
    
    /**  @cond INTERNAL */
    Label();
    ~Label();
    /**  @endcond */
    
    /** @name Properties
     * @{
     */
    virtual const AttributedString& getText() { return _textLayout.getText(); }
    virtual void setText(const AttributedString& text);
    virtual void setTextColor(COLOR color);
    virtual void setFont(Font* font);
    virtual void setFontName(const string& fontName);
    virtual void setFontSize(float fontSize);
    virtual void setFontWeight(float fontWeight);
    virtual void setMaxLines(int maxLines);
    virtual const Attribute* getAttribute(int32_t pos, Attribute::Type type);
    /** @} */

    /** @name Overrides
     * @{
     */
    bool applySingleStyle(const string& name, const style& value) override;
    //void invalidateContentSize() override;
    void layout(RECT constraint) override;
    void onEffectiveTintColorChanged() override;
    void setContentOffset(POINT contentOffset, bool animated=false) override;
    void setGravity(GRAVITY gravity) override;
    void updateContentSize(SIZE constrainingSize) override;
    void updateRenderOps() override;
    /** @} */

    
    
#ifdef DEBUG
    string debugViewType() override;
#endif
    
protected:
    TextLayout _textLayout;
    COLOR _defaultColor;
    float _prevParentWidth;
    
    struct {
        bool pending;
        string name;
        float size;
        float weight;
    } _pendingFontChange;
    
    void layoutText();
};


