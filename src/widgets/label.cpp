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


bool Label::applyStyleValue(const string& name, const StyleValue* value) {
    if (name=="font-name") {
        _textRenderer.setFontName(value->stringVal());
        return true;
    }
    if (name=="font-size") {
        _textRenderer.setFontSize(value->floatVal());
        return true;
    }
    if (name=="font-weight") {
        _textRenderer.setFontWeight(value->fontWeightVal());
        return true;
    }
    if (name=="forecolor") {
        setTextColor(value->colorVal());
        return true;
    }
    if (name=="text") {
        setText(value->stringVal());
        return true;
    }
    if (name=="maxLines") {
        setMaxLines(value->intVal());
        return true;
    }
    return View::applyStyleValue(name, value);
}

void Label::setText(const AttributedString& text) {
    _textRenderer.setText(text);
    invalidateContentSize();
}

void Label::setFont(Font* font) {
    _textRenderer.setFont(font);
    invalidateContentSize();
}
void Label::setFontName(const string& fontName) {
    _textRenderer.setFontName(fontName);
    invalidateContentSize();
}

void Label::setMaxLines(int maxLines) {
    _textRenderer.setMaxLines(maxLines);
    invalidateContentSize();
}


void Label::setTextColor(COLOR color) {
    _defaultColor = color;
    onEffectiveTintColorChanged();
}

void Label::onEffectiveTintColorChanged() {
    _textRenderer.setColor(_effectiveTintColor ? _effectiveTintColor : _defaultColor);
    setNeedsFullRedraw();
}

void Label::measure(float parentWidth, float parentHeight) {
    
    // If the width changed and the label isn't absolutely sized, force a re-evaluation of content size
    if (parentWidth != _prevParentWidth && _widthMeasureSpec.refType!=MEASURESPEC::RefTypeAbs) {
        _textRenderer._measuredSizeValid = false;
        _contentSizeValid = false;
        _prevParentWidth = parentWidth;
    }
    View::measure(parentWidth, parentHeight);
}

void Label::setContentOffset(POINT contentOffset) {
    View::setContentOffset(contentOffset);
    _textRenderer.updateRenderOps(this);

}

void Label::setRectSize(const SIZE& asize) {
    if (asize.width == _rect.size.width && asize.height == _rect.size.height) {
        return;
    }
    // This is called when our parent view (typically a LinearLayout) is overriding the size we wanted to be
    // and this means we have to remeasure the text. Which means the height may grow (or shrink).
    _textRenderer._measuredSizeValid = false;
    SIZE size = asize;
    updateContentSize(size.width, MAXFLOAT);
    if (_heightMeasureSpec.refType == MEASURESPEC::RefTypeContent) {
        size.height = _contentSize.height + _padding.top+_padding.bottom;
    }
    View::setRectSize(size);
}


void Label::updateContentSize(float parentWidth, float parentHeight) {

    _contentSize.width = 0;
    _contentSize.height = 0;

    parentWidth -= (_padding.left + _padding.right);
    parentHeight -= (_padding.top + _padding.bottom);
    
    _textRenderer.measure({parentWidth, parentHeight});
    
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
        _textRenderer.layout(getOwnRectPadded());
        _textRendererMustRelayout = false;
    }
    _textRenderer.updateRenderOps(this);
}

void Label::layout() {
	View::layout();
    
    // Automatically set clipsContent based on whether text overflows bounds
    _clipsContent = (_contentSize.height > _rect.size.height)
                 || (_contentSize.width > _rect.size.width);

    _textRenderer.layout(getOwnRectPadded());
    _textRendererMustRelayout = false;
    _updateRenderOpsNeeded = true;
}

void Label::setGravity(GRAVITY gravity) {
    View::setGravity(gravity);
    _textRenderer.setGravity(gravity);
}


#ifdef DEBUG

string Label::debugViewType() {
    char ach[256];
    snprintf(ach, 256, "Label:%s", _textRenderer.getText().data());
    return string(ach);
}

#endif
