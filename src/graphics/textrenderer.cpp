//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


TextRenderer::TextRenderer() {
    _fontWeight = FONT_WEIGHT_REGULAR;
}

void TextRenderer::setText(const AttributedString& text) {
    _text = text;
    _measuredSizeValid = false;
}
void TextRenderer::setColor(COLOR color) {
    _color = color;
    _measuredSizeValid = false; // lazy. color doesn't affect size!
}

void TextRenderer::setFont(Font* font) {
    _font = font;
    _fontName = _font->_name;
    _fontSize = _font->_size;
    _measuredSizeValid = false;
    _fontValid = true;
}

void TextRenderer::setFontName(const string &fontName) {
    if (_fontName != fontName) {
        _fontName = fontName;
        _measuredSizeValid = false;
        _fontValid = false;
    }
}
void TextRenderer::setFontSize(float fontSize) {
    if (_fontSize != fontSize) {
        _fontSize = fontSize;
        _measuredSizeValid = false;
        _fontValid = false;
    }
}
void TextRenderer::setFontWeight(float fontWeight) {
    if (_fontWeight != fontWeight) {
        _fontWeight = fontWeight;
        _fontValid = false;
    }
}


void TextRenderer::setMaxLines(int maxLines) {
    _maxLines = maxLines;
    _measuredSizeValid = false;
}

void TextRenderer::setGravity(GRAVITY gravity) {
    _gravity = gravity;
    _renderOpsValid = false;
}

void TextRenderer::measure() {
    if (_measuredSizeValid) {
        return;
    }
    measure({0,0});
}

bool TextRenderer::applyStyleValue(const string& name, const StyleValue* value) {
    if (name == "forecolor") {
        setColor(value->colorVal());
        return true;
    }
    if (name == "font-size") {
        setFontSize(value->floatVal());
        return true;
    }
    return false;
}

#define isWhitespace(c) (c==' ')

void TextRenderer::measure(SIZE maxSize) {
    assert(!_measuredSizeValid); // measure() has been called pointlessly
    
    // Get font if not got it yet
    if (!_fontValid) {
        _font = Font::get(_fontName, _fontSize, _fontWeight);
        _fontValid = true;
    }
    
    bool filledHorizontally = false;
    _measuredSize.width = 0;
    _measuredSize.height = 0;
    _renderParams.clear();
    
    
    // TODO! This function should be split into a character generation part and a measure part
    // The character generation only needs to run after text or attributes changes, it doesn't
    // need to run on each and every measure.
    
    // Initial text render params
    TEXTRENDERPARAMS textRenderParams;
    textRenderParams.atlasPage = NULL;
    textRenderParams.forecolor = _color;
    textRenderParams.renderOp = NULL;
    TEXTRENDERPARAMS* currentParams = &textRenderParams;
    bool paramsChanged = false;
    
    // Prepare to walk the ordered spans collection
    stack<Font*> fontStack;
    stack<COLOR> forecolorStack;
    stack<float> fontWeightStack;
    auto spanStartIterator = _text._attributes.begin();
    auto spanEndIterator = _text._attributes.begin();
    
    // Defaults
    Font* currentFont = _font;
    float leadingSpace = 0;
    float currentFontWeight = FONT_WEIGHT_REGULAR;
    
    // Start with an empty line and no characters
    _characters.clear(); // todo: reserve some empirical number of chars
    _lines.clear();
    auto currentLine = _lines.emplace(_lines.end(), TEXTLINE {0});
    currentLine->font = currentFont;

    // Iterate over all codepoints in the text and process it into the list of displayable characters
    StringProcessor it(_text);
    int32_t codepointIndex=0;
    char32_t codepoint='\0', prevCh;
    int breakOpportunity = -1;
    while (true) {
        prevCh = codepoint;
        codepoint = it.next();
        if (!codepoint) {
            break;
        }
        
        // TODO: accumulate (via 'continue') consecutive codepoints into a single character here
        // Since these are early days we TEMPORARILY treat one codepoint == one character
        
        
        // Unapply spans that end at this point
        while (spanEndIterator!=_text._attributes.end() && codepointIndex>=spanEndIterator->end) {
            auto endingSpan = (*spanEndIterator);
            if (endingSpan.attribute._type == Attribute::Type::Font) {
                currentFont = fontStack.top();
                fontStack.pop();
            }
            else if (endingSpan.attribute._type == Attribute::Type::Forecolor) {
                textRenderParams.forecolor = forecolorStack.top();
                forecolorStack.pop();
                paramsChanged = true;
            }
            else if (endingSpan.attribute._type == Attribute::Type::LeadingSpace) {
                leadingSpace = 0;
            }
            else if (endingSpan.attribute._type == Attribute::Type::FontWeight) {
                currentFont = fontStack.top();
                fontStack.pop();
                currentFontWeight = fontWeightStack.top();
                fontWeightStack.pop();
            }
            spanEndIterator++;
        }

        // Apply any spans that start at this point
        while (spanStartIterator!=_text._attributes.end() && codepointIndex>=spanStartIterator->start) {
            auto startingSpan = (*spanStartIterator);
            if (startingSpan.attribute._type == Attribute::Type::Font) {
                fontStack.push(currentFont);
                currentFont = startingSpan.attribute._font;
            }
            else if (startingSpan.attribute._type == Attribute::Type::Forecolor) {
                forecolorStack.push(textRenderParams.forecolor);
                textRenderParams.forecolor = startingSpan.attribute._color;
                paramsChanged = true;
            }
            else if (startingSpan.attribute._type == Attribute::Type::LeadingSpace) {
                leadingSpace = startingSpan.attribute._f;
            }
            else if (startingSpan.attribute._type == Attribute::Type::FontWeight) {
                fontWeightStack.push(currentFontWeight);
                currentFontWeight = startingSpan.attribute._f;
                fontStack.push(currentFont);
                currentFont = Font::get(currentFont->_name, currentFont->_size, currentFontWeight);
            } else {
                app.warn("unrecognized attribute type");
            }
            
            spanStartIterator++;
        }
        
        // Ignore the sequence of \r\n... we don't need to do anything cos the linebreak was a
        // already done when the \r was encountered on the previous iteration
        if (!(codepoint == '\n' && prevCh == '\r')) {

            Glyph* glyph;
            float glyphAdvance, glyphHeight;
            
            // Hard line-break
            bool isHardLineBreak = (codepoint == '\n' || codepoint == '\r');
            if (isHardLineBreak) {
                glyph = NULL;
                glyphAdvance = glyphHeight = 0;
            }
            
            // Normal character handling
            else {
        
                // Tab-handling. For now just convert to spaces.
                if (codepoint=='\t') {
                    app.log("todo: tabstops!");
                    codepoint = ' ';
                }
                
                // If at the start of a word, track this latest opportunity for a soft line-break if one is needed later
                if (isWhitespace(prevCh) && !isWhitespace(codepoint)) {
                    breakOpportunity = currentLine->numCharacters; // track first non-whitespace char after run of whitespace chars
                }
                
                // Fetch the glyph for this character
                glyph = currentFont->getGlyph(codepoint);
                if (!glyph) {
                    glyph = currentFont->getGlyph('?'); // todo: should ask the font for it's default character...
                    assert(glyph);
                }
                glyphAdvance = static_cast<float>(glyph->advance.width);
                glyphHeight = static_cast<float>(glyph->bitmapHeight);

                // Track atlas page changes
                if (glyph->atlasNode->page != textRenderParams.atlasPage) {
                    textRenderParams.atlasPage = glyph->atlasNode->page;
                    paramsChanged = true;
                }
                
                // Ensure our renderParams set includes current params
                if (paramsChanged) {
                    auto p = _renderParams.insert(textRenderParams);
                    auto p2 = p.first;
                    currentParams = const_cast<TEXTRENDERPARAMS*>(&*p2);
                    paramsChanged = false;
                }
            }
            

            // Will adding this glyph to the current line cause the line to exceed bounds?
            //auto glyphAdvance = glyph->advance.width;
            auto newBoundsWidth = currentLine->bounds.size.width + leadingSpace + glyphAdvance;
            bool exceedsBounds = newBoundsWidth>=maxSize.width && currentLine->numCharacters > 0
                    && maxSize.height>0;
            
            // If we've exceeded bounds we'll have to apply a soft linebreak...
            if (exceedsBounds && !isWhitespace(codepoint)) { // NB: trailing spaces are allowed to overflow bounds!
                filledHorizontally = true;
                if (breakOpportunity < 0) { // if there were no spaces on this line, force a break at the preceding char
                    breakOpportunity = (int32_t)currentLine->numCharacters-1;
                }
                float xx = 0;
                int32_t charsOnNextLine = currentLine->numCharacters - breakOpportunity;
                for (int32_t bi=0; bi < charsOnNextLine; bi++) {
                    auto& character = _characters[currentLine->startCharacterIndex + breakOpportunity + bi];
                    xx += character.leadingSpace + character.glyph->advance.width;
                }
                currentLine->numCharacters -= charsOnNextLine;
                currentLine->bounds.size.width -= xx;
                // Remove any trailing whitespace from the measured line length, it shouldn't
                // count towards the measured line width.
                int32_t bi = breakOpportunity;
                while (--bi>=0) {
                    auto& character = _characters[currentLine->startCharacterIndex + bi];
                    if (!isWhitespace(character.glyph->charCode)) {
                        break;
                    }
                    currentLine->bounds.size.width -= character.leadingSpace + character.glyph->advance.width;
                }
                currentLine = _lines.emplace(_lines.end(), TEXTLINE {currentLine->startCharacterIndex + breakOpportunity});
                currentLine->font = currentFont;
                currentLine->bounds.size.width = xx;
                currentLine->numCharacters = charsOnNextLine;
                breakOpportunity = -1;
            }
            
            // Add the character to our vector
            _characters.emplace_back(DISPLAYED_CHAR {codepointIndex, leadingSpace, {0,0,glyphAdvance,glyphHeight}, currentParams, 0, glyph});

            // Add the glyph to the current line
            currentLine->bounds.size.width += leadingSpace+glyphAdvance;
            currentLine->numCharacters++;
            
            // If char was a hard break, start a new line
            if (isHardLineBreak) {
                currentLine = _lines.emplace(_lines.end(), TEXTLINE {codepointIndex+1});
                currentLine->font = currentFont;
                breakOpportunity = -1;
            }
        }
        codepointIndex++;
    }
    
    
    // Iterate lines, calculate each line height and do naive typesetting
    int numLines = (int)_lines.size();
    float y = 0;
    for (int i=0 ; i<numLines ; i++) {
        TEXTLINE& line = _lines.at(i);
        float lineHeight = line.font->_height;
        float baseline = line.font->_ascent;
        for (auto charIndex = line.startCharacterIndex ; charIndex<line.startCharacterIndex+line.numCharacters ; charIndex++) {
            auto& character = _characters[charIndex];
            if (!character.glyph) {
                continue;
            }
            Font* font = character.glyph->_font;
            lineHeight = fmaxf(lineHeight, font->_height);
            baseline = fmaxf(baseline, font->_ascent);
            if (character.glyph->bitmapHeight > lineHeight) { // i.e. outsized glyph exceeds normal font lineheight
                lineHeight = character.glyph->bitmapHeight + (font->_height-(font->_ascent-font->_descent));
                baseline = lineHeight + font->_descent;
            }
        }
        line.bounds.size.height = lineHeight;
        line.bounds.origin = {0, y};
        line.baseline = baseline;
        _measuredSize.width = MAX(_measuredSize.width, line.bounds.size.width);
        _measuredSize.height += lineHeight;
        y += lineHeight;
    }
    
    _measuredSize.width = ceilf(_measuredSize.width);
    _measuredSize.height = ceilf(_measuredSize.height);

    _measuredSizeValid = true;
    _renderOpsValid = false;
}

/**
 layout the lines of text w.r.t. their size and gravity within the
 frame into which they are rendered
 */
//void TextRenderer::layout(const RECT& arect) {
//    RECT rect = arect;
void TextRenderer::layout(RECT rect) {
    
    _layoutRect = rect;
    float dY = 0;
    if (_gravity.vert != GRAVITY_TOP) {
        dY = rect.size.height - _measuredSize.height;
        if (_gravity.vert == GRAVITY_CENTER) {
            dY /= 2;
        }
    }

    for (int i=0 ; i<_lines.size() ; i++) {
        TEXTLINE& line = _lines.at(i);

        line.bounds.origin =  rect.origin;
        line.bounds.origin.y += dY;
        float dX = rect.size.width - line.bounds.size.width;
        if (dX > 0) {
            if (_gravity.horz != GRAVITY_LEFT) {
                if (_gravity.horz == GRAVITY_CENTER) {
                    dX /= 2;
                }
                line.bounds.origin.x += dX;
            }
        }

        float x=line.bounds.origin.x;
        float y=line.bounds.origin.y;
        for (int32_t charIndex=line.startCharacterIndex ; charIndex<line.startCharacterIndex+line.numCharacters ; charIndex++) {
            auto& character = _characters[charIndex];
            Glyph* glyph = character.glyph;
            int ix = (int)x;
            if (!glyph) {
                character.rect = RECT(ix, y + line.baseline, 0, 0);
            } else {
                ix += character.leadingSpace;
                character.rect = RECT(ix + glyph->bitmapLeft,
                                y + line.baseline - (glyph->bitmapHeight+ glyph->bitmapTop),
                                glyph->atlasNode->rect.size.width,
                                glyph->atlasNode->rect.size.height);
                x += character.leadingSpace + glyph->advance.width;
            }
        }
        
        rect.origin.y += line.bounds.size.height;
    }
    _renderOpsValid = false;
}

void TextRenderer::updateRenderOps(View* view) {

    // TODO: optimize for multiline text

    RECT visibleRect = view->getOwnRect();
    visibleRect.origin.y = view->getContentOffset().y;

    
    // Clear old ops
    for (auto it : _textOps) {
        view->removeRenderOp(it);
        it->reset();
    }
    _textOps.clear();

    // Iterate over the lines to find the visible ones
    set<TEXTRENDERPARAMS*> uniqueParams;
    TEXTRENDERPARAMS* currentParams = NULL;
    for (int i=0 ; i<_lines.size() ; i++) {
        TEXTLINE& line = _lines.at(i);
        if (visibleRect.intersects(line.bounds)) {
            for (auto charIndex=line.startCharacterIndex ; charIndex<line.startCharacterIndex+line.numCharacters ; charIndex++) {
                auto& character = _characters[charIndex];
                if (!character.glyph) {
                    continue;
                }
                if (currentParams != character.renderParams) {
                    currentParams = character.renderParams;
                    if (!currentParams->renderOp) {
                        currentParams->renderOp = new TextRenderOp(currentParams);
                    }
                    auto t = uniqueParams.insert(currentParams);
                    if (t.second) {
                        _textOps.push_back(currentParams->renderOp);
                    }
                }
                if (character.glyph->charCode != ' ') {
                    currentParams->renderOp->addGlyph(character.glyph, character.rect);
                }
            }
        }
    }
    
    // Add new ops to view
    for (auto it=_textOps.begin() ; it!=_textOps.end() ; it++) {
        view->addRenderOp(*it);
    }

    _renderOpsValid = true;
}

int32_t TextRenderer::moveCharacterIndex(int32_t charIndex, int dx, int dy) const {
    if (dy == 0) {
        charIndex += dx;
        if (charIndex <0) charIndex = 0;
        if (charIndex > _characters.size()) charIndex = (int32_t)_characters.size(); // yes, one past last char
    } else {
        POINT pt;
        getCharacterOrigin(charIndex, &pt, NULL, NULL);
        auto line = getLineForCharacterIndex(charIndex, dy);
        float minDiffX = FLT_MAX;
        int nearestCharIndex = 0;
        for (int i=0 ; i<line->numCharacters ; i++) {
            auto& character = _characters[line->startCharacterIndex+i];
            float diffX = fabs(character.rect.origin.x - pt.x);
            if (diffX < minDiffX) {
                minDiffX = diffX;
                nearestCharIndex = i;
            }
        }
        charIndex = line->startCharacterIndex + nearestCharIndex;
    }
    return charIndex;
}

int32_t TextRenderer::characterIndexFromPoint(const POINT& pt) const {
    assert(_measuredSizeValid);
    const TEXTLINE *line = NULL;
    for (auto &it: _lines) {
        if (pt.y >= it.bounds.origin.y && pt.y < it.bounds.bottom()) {
            line = &it;
            break;
        }
    }
    if (!line) {
        if (_lines.size() == 0) {
            return 0;
        }
        if (pt.y < _lines.begin()->bounds.origin.y) {
            line = &_lines.at(0);
        } else {
            line = &_lines.at(_lines.size() - 1);
        }
    }

    int32_t nearestCharIndex = 0;
    if (pt.x >= line->bounds.right()) {
        nearestCharIndex = line->numCharacters;
    } else {
        float minErr = FLT_MAX;
        for (int32_t charIndex = 0; charIndex < line->numCharacters; charIndex++) {
            auto character = _characters[line->startCharacterIndex + charIndex];
            float dx = fabs(pt.x - character.rect.midX());
            if (dx < minErr) {
                nearestCharIndex = charIndex;
                minErr = dx;
            }
        }
    }
    return line->startCharacterIndex + nearestCharIndex;
}


const TextRenderer::TEXTLINE* TextRenderer::getLineForCharacterIndex(int32_t charIndex, int dLine) const {
    for (int i=0 ; i<_lines.size() ; i++) {
        const TEXTLINE& line = _lines.at(i);
        int32_t numCharacters = line.numCharacters;
        if (charIndex >= line.startCharacterIndex && charIndex < line.startCharacterIndex+numCharacters) {
            i += dLine;
            if (i<0) i=0;
            if (i>=_lines.size()) i=(int)_lines.size()-1;
            return &_lines.at(i);
        }
    }
    assert(_lines.size());// there should always be at least one line
    return &_lines.at(_lines.size()-1);
}

void TextRenderer::getCharacterOrigin(int32_t charIndex, POINT* origin, float* ascent, float* descent) const {
    assert(_measuredSizeValid);
    const TEXTLINE* line = getLineForCharacterIndex(charIndex, 0);

    if (ascent) {
        *ascent = line->baseline;
        *descent = -(line->bounds.size.height - line->baseline);
    }
    
    if (charIndex < line->startCharacterIndex + line->numCharacters) {
        const DISPLAYED_CHAR& glyphInfo = _characters[charIndex];
        *origin = {glyphInfo.rect.left(), glyphInfo.rect.bottom()};
    } else {
        *origin = {line->bounds.right(), line->bounds.origin.y + line->baseline};
    }
}

    

