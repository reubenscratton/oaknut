//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(View*)> OnClickDelegate;

class Button : public Label {
public:
    OnClickDelegate _onClickDelegate;
    
    Button();
    
    // Styleable colours
    COLOUR _backgroundColour;
    COLOUR _borderColour;
    COLOUR _pressedFillColour;

    // Overrides
    virtual bool onInputEvent(INPUTEVENT* event);
    virtual bool applyStyleValue(const string& name, StyleValue* value);
    virtual void applyStyleValues(const StyleValueList& values);

};


class ToolbarButton : public View {
public:

	ImageView* _imageView;
	OnClickDelegate _onClickDelegate;
	
	void setImageUrl(const string& url);
    void setImageBitmap(Bitmap* bitmap);
    virtual bool onInputEvent(INPUTEVENT* event);

protected:
    void lazyCreateImageView();
};


