//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Label : public View {
public:
    TextRenderer _textRenderer;
    COLOR _defaultColor;
    float _prevParentWidth;
    
    Label();
	~Label();
    
    // Overrides
    bool applyStyleValue(const string& name, StyleValue* value) override;
    void updateRenderOps() override;
    void measure(float parentWidth, float parentHeight) override;
    void updateContentSize(float parentWidth, float parentHeight) override;
    void setContentOffset(POINT contentOffset) override;
    void layout() override;
    void onEffectiveTintColorChanged() override;
    void setGravity(GRAVITY gravity) override;
    
    // API
    virtual const string& getText() { return _textRenderer.getText(); }
	virtual void setText(const string& text);
    virtual const AttributedString& getAttributedText() { return _textRenderer.getAttributedText(); }
    virtual void setAttributedText(const AttributedString& text);
    virtual void setTextColor(COLOR color);
	virtual void setFont(Font* font);
    virtual void setFontName(const string& fontName);
    virtual void setMaxLines(int maxLines);
    
#ifdef DEBUG
    virtual string debugViewType();
#endif
    
protected:
    bool _textRendererMustRelayout;
    
};


