//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

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
    applyStyle("Label");
    
    // Ensure overridden property setters are actually called.
    setGravity(_gravity);
}
Label::~Label() {
}


bool Label::applySingleStyle(const string& name, const style& value) {
    if (name=="font-name") {
        setFontName(value.stringVal());
        return true;
    }
    if (name=="font-size") {
        setFontSize(value.floatVal());
        return true;
    }
    if (name=="font-weight") {
        setFontWeight(value.fontWeightVal());
        return true;
    }
    if (name=="forecolor") {
        setTextColor(value.colorVal());
        return true;
    }
    if (name=="text") {
        setText(value.stringVal());
        return true;
    }
    if (name=="lines") {
        int numLines = value.intVal();
        setMinLines(numLines);
        setMaxLines(numLines);
        return true;
    }
    if (name=="min-lines") {
        setMinLines(value.intVal());
        return true;
    }
    if (name=="max-lines") {
        setMaxLines(value.intVal());
        return true;
    }
    if (name=="ellipsize") {
        setEllipsize(value.boolVal());
        return true;
    }
    if (name=="line-height") {
        auto m = value.measurementVal();
        if (m._unit == measurement::PC) {
            setLineHeight(m.val(), 0);
        } else {
            setLineHeight(0, m.val());
        }
        return true;
    }
    return View::applySingleStyle(name, value);
}

void Label::setText(const attributed_string& text) {
    _textLayout.setText(text);
    invalidateIntrinsicSize();
}

void Label::setFont(Font* font) {
    _textLayout.setFont(font);
    _pendingFontChange.pending = false;
    invalidateIntrinsicSize();
}
void Label::setFontName(const string& fontName) {
    _pendingFontChange.name = fontName;
    _pendingFontChange.pending = true;
    invalidateIntrinsicSize();
}
void Label::setFontSize(float size) {
    auto font = _textLayout.getFont();
    if (!font || size != font->_size) {
        _pendingFontChange.size = size;
        _pendingFontChange.pending = true;
        invalidateIntrinsicSize();
    }
}
void Label::setFontWeight(float weight) {
    _pendingFontChange.weight = weight;
    _pendingFontChange.pending = true;
    invalidateIntrinsicSize();
}

void Label::setLineHeight(float mul, float abs) {
    _textLayout.setLineHeight(mul, abs);
    invalidateIntrinsicSize();
}

void Label::setMinLines(int minLines) {
    _textLayout.setMinLines(minLines);
    invalidateIntrinsicSize();
}

void Label::setMaxLines(int maxLines) {
    _textLayout.setMaxLines(maxLines);
    invalidateIntrinsicSize();
}

bool Label::getEllipsize() const {
    return _textLayout.ellipsize();
}
void Label::setEllipsize(bool ellipsize) {
    _textLayout.setEllipsize(ellipsize);
    invalidateIntrinsicSize();
}

void Label::setTextColor(COLOR color) {
    if (_defaultColor != color) {
        _defaultColor = color;
        onEffectiveTintColorChanged();
    }
}

void Label::onEffectiveTintColorChanged() {
    _textLayout.setColor(_effectiveTintColor ? _effectiveTintColor : _defaultColor);
    _updateRenderOpsNeeded = true;
    setNeedsFullRedraw();
}

void Label::layout(RECT constraint) {
    
    View::layout(constraint);
    
    layoutText();
}

RECT Label::getTextRectForLayout() {
    return getOwnRectPadded();
}
void Label::layoutText() {
    RECT rect = getTextRectForLayout();
    _textLayout.layout(rect);
    if (!_textLayout.opsValid()) {
        _updateRenderOpsNeeded = true;
    }
}

void Label::setContentOffset(POINT contentOffset, bool animated) {
    View::setContentOffset(contentOffset, animated);
    _textLayout.invalidateOps();
    _updateRenderOpsNeeded = true;
}


void Label::updateIntrinsicSize(SIZE constrainingSize) {

    // Update font
    if (_pendingFontChange.pending) {
        const Font* currentFont = _textLayout.getFont();
        string name = _pendingFontChange.name.length() ? _pendingFontChange.name : currentFont->_name;
        float size = _pendingFontChange.size ? _pendingFontChange.size : currentFont->_size;
        float weight = _pendingFontChange.weight ? _pendingFontChange.weight : currentFont->_weight;
        _textLayout.setFont(Font::get(name, size, weight));
        _pendingFontChange.pending = false;
    }
    
    // Measure text
    constrainingSize.width -= (_padding.left + _padding.right);
    constrainingSize.height -= (_padding.top + _padding.bottom);
    _intrinsicSize = _textLayout.measure(constrainingSize);
    
    // Flag that renderOps will need updating after layout
    _updateRenderOpsNeeded = true;
    
    // If no layout() call is scheduled but text layout needs repositioning (cos the text
    // changed) then run text layout using our existing rect frame.
    if (_layoutValid && _textLayout.glyphsNeedsPositioning()) {
        layoutText();
    }
}


void Label::updateRenderOps() {
    //COLOR textColor = _effectiveTintColor ? _effectiveTintColor : _defaultColor;


    _textLayout.updateRenderOpsForView(this);
    //updateEffectiveAlpha();
    //log_info("Eff. alpha %f for label '%s'", _effectiveAlpha, _textRenderer._text.c_str());
}


void Label::setGravity(GRAVITY gravity) {
    View::setGravity(gravity);
    _textLayout.setGravity(gravity);
}

const attributed_string::attribute* Label::getAttribute(int32_t pos, attributed_string::attribute::type type) {
    return _textLayout._text.getAttribute(pos, type);
}




#ifdef DEBUG

string Label::debugViewType() {
    char ach[256];
    snprintf(ach, 256, "Label:%s", _textLayout.getText().c_str());
    return string(ach, -1);
}

#endif
