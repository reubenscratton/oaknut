//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

DECLARE_DYNCREATE(Label);

/*
 Setting text, font, or anything else that affects text size invalidates the content
 size. To determine the content size the text is initially processed into lines and
 renderparams (unique text styles).
 
 Second stage processing occurs on the first render after layout & updateContentSize,
 where the Label must determine which lines are visible and build the appropriate ops.
 
 Second stage processing also occurs when contentOffset changes but effort is
 made to minimise the amount of work done, i.e. nothing happens unless lines are
 scrolled in or out of view.
 
 Line spacing
 ============
 Line bounds are determined by the largest ascent and descent of the glyphs in it, i.e.
 the bounds rect tightly fits the glyphs that make up the line.
 
 Line spacing (the distance from the baseline to the baseline above) is determined
 mainly by the height of the tallest font in the line. It is possible for freakishly
 tall glyphs to exceed the font height, eg Ǻ (improperly rendered in xcode, the acute
 accent above the ring is missing!) in which case the line height is raised to :
 
  tallestGlyph.ascent+tallestGlyph.descent + (font.height-(font.ascent+font.descent)).
 
 
 */




Label::Label() : View() {
    _textRenderer.setDefaultFont(oakFontGet(Styles::defaultFontName(), Styles::defaultFontSize()));
    _textRenderer.setDefaultColour(0xFF000000);
    _textRenderer.setGravity(_gravity);
}
Label::~Label() {	
}


bool Label::applyStyleValue(const string& name, StyleValue* value) {
    if (name=="font-name") {
        setFont(oakFontGet(value->str, _textRenderer.getDefaultFont()->_size));
        return true;
    }
    if (name=="font-size") {
        setFont(oakFontGet(_textRenderer.getDefaultFont()->_name, value->getAsFloat()));
        return true;
    }
    if (name=="forecolour") {
        setTextColour(value->i);
        return true;
    }
    if (name=="text") {
        setText(value->str);
        return true;
    }
    if (name=="gravityX") {
        if (value->str == "left") {
            _gravity.horz = GRAVITY_LEFT;
        } else if (value->str == "right") {
            _gravity.horz = GRAVITY_RIGHT;
        } else if (value->str == "center" || value->str == "centre") {
            _gravity.horz = GRAVITY_CENTER;
        } else {
            assert(0);
        }
        _textRenderer.setGravity(_gravity);
        return true;
    }
    if (name=="gravityY") {
        if (value->str == "top") {
            _gravity.vert = GRAVITY_TOP;
        } else if (value->str == "bottom") {
            _gravity.vert = GRAVITY_BOTTOM;
        } else if (value->str == "center" || value->str == "centre") {
            _gravity.vert = GRAVITY_CENTER;
        } else {
            assert(0);
        }
        _textRenderer.setGravity(_gravity);
        return true;
    }
    return View::applyStyleValue(name, value);
}

void Label::setText(string text) {
    _textRenderer.setText(text);
    invalidateContentSize();
}

void Label::setFont(Font* font) {
    _textRenderer.setDefaultFont(font);
    invalidateContentSize();
}

void Label::setMaxLines(int maxLines) {
    _textRenderer.setMaxLines(maxLines);
    invalidateContentSize();
}


void Label::setTextColour(COLOUR colour) {
    _defaultColour = colour;
    onEffectiveTintColourChanged();
}

void Label::onEffectiveTintColourChanged() {
    _textRenderer.setDefaultColour(_effectiveTintColour ? _effectiveTintColour : _defaultColour);
    setNeedsFullRedraw();
}



void Label::updateContentSize(float parentWidth, float parentHeight) {

    _contentSize.width = 0;
    _contentSize.height = 0;

    parentWidth -= (_padding.left + _padding.right);
    parentHeight -= (_padding.top + _padding.bottom);
    
    _textRenderer.measure(SIZE_Make(parentWidth, parentHeight));
    
    _contentSize = _textRenderer.measuredSize();
    // If we had to use a soft linebreak then we know we filled the available width
    //if (_textRenderer.hasSoftLineBreaks) {
    //    _contentSize.width = parentWidth;
    //}

    // Flag that renderOps will need updating after layout
    _textRendererMustRelayout = true;
    _updateRenderOpsNeeded = true;
}


void Label::updateRenderOps() {
    if (_textRendererMustRelayout) {
        _textRenderer.layout(getBoundsWithPadding());
        _textRendererMustRelayout = false;
    }
    _textRenderer.updateRenderOps(this);
}

void Label::layout() {
	View::layout();

    _textRenderer.layout(getBoundsWithPadding());
    _textRendererMustRelayout = false;
    _updateRenderOpsNeeded = true;
}

void Label::setGravity(GRAVITY gravity) {
    View::setGravity(gravity);
    _textRenderer.setGravity(gravity);
}

void Label::setStyle(string styleName) {
    // TODO: this method should just fetch the named map from Styles:: and call View::applyStyleValues()
    string fontName = Styles::getString(styleName + ".font-name");
    float fontSize = Styles::getFloat(styleName + ".font-size");
    if (fontName.size() && fontSize) {
        setFont(oakFontGet(fontName, fontSize));
    }
    COLOUR textColour = Styles::getColour(styleName + ".forecolour");
    if (textColour) {
        setTextColour(textColour);
    }
}

#ifdef DEBUG

string Label::debugViewType() {
    char ach[256];
    snprintf(ach, 256, "Label:%s", _textRenderer.getText().data());
    return string(ach);
}

#endif
