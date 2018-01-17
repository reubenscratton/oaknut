//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Label : public View {
public:
    TextRenderer _textRenderer;
    COLOUR _defaultColour;
    
    Label();
	~Label();
    
    // Overrides
    virtual bool applyStyleValue(const string& name, StyleValue* value);
    virtual void updateRenderOps();
    virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual void layout();
    virtual void onEffectiveTintColourChanged();
    virtual void setGravity(GRAVITY gravity);
    
    // API
    virtual const string& getText() { return _textRenderer.getText(); }
	virtual void setText(const string& text);
	virtual void setText(const char* format, ...);
    virtual void setTextColour(COLOUR colour);
	virtual void setFont(Font* font);
    virtual void setMaxLines(int maxLines);
    virtual void setStyle(string styleName);
    
#ifdef DEBUG
    virtual string debugViewType();
#endif
    
protected:
    bool _textRendererMustRelayout;
    
};


