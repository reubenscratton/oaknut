//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Label : public View {
public:
    TextRenderer _textRenderer;
    COLOR _defaultColor;
    float _prevParentWidth;
        
    // API
    Label();
    ~Label();
    virtual const AttributedString& getText() { return _textRenderer.getText(); }
    virtual void setText(const AttributedString& text);
    virtual void setTextColor(COLOR color);
    virtual void setFont(Font* font);
    virtual void setFontName(const string& fontName);
    virtual void setMaxLines(int maxLines);
    virtual const Attribute* getAttribute(int32_t pos, Attribute::Type type);

    // Overrides
    bool applyStyleValue(const string& name, const StyleValue* value) override;
    void invalidateContentSize() override;
    void layout(RECT constraint) override;
    void onEffectiveTintColorChanged() override;
    void setContentOffset(POINT contentOffset) override;
    void setGravity(GRAVITY gravity) override;
    void updateContentSize(SIZE constrainingSize) override;
    void updateRenderOps() override;

    
    
#ifdef DEBUG
    string debugViewType() override;
#endif
    
protected:
    bool _textRendererMustRelayout;
    
};


