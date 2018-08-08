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
    virtual bool applyStyleValue(const string& name, StyleValue* value);
    virtual void updateRenderOps();
    virtual void measure(float parentWidth, float parentHeight);
    virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual void setContentOffset(POINT contentOffset);
    virtual void layout();
    virtual void onEffectiveTintColorChanged();
    virtual void setGravity(GRAVITY gravity);
    
    // API
    virtual const string& getText() { return _textRenderer.getText(); }
	virtual void setText(const string& text);
    virtual const AttributedString& getAttributedText() { return _textRenderer.getAttributedText(); }
    virtual void setAttributedText(const AttributedString& text);
    virtual void setTextColor(COLOR color);
	virtual void setFont(Font* font);
    virtual void setMaxLines(int maxLines);
    virtual void setStyle(string styleName);
    
#ifdef DEBUG
    virtual string debugViewType();
#endif
    
protected:
    bool _textRendererMustRelayout;
    
};


