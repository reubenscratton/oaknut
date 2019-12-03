//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

#define INVALID_GLYPHS    1
#define INVALID_LINES     2
#define INVALID_POSITION  4
//#define INVALID_RUNS      8
#define INVALID_OPS       16


TextLayout::TextLayout() {
    _defaultParams.font = app->defaultFont();
    _defaultParams.forecolor = 0xFF000000; // todo: style
    _defaultParams.lineHeightMul = 1.f;
    _defaultParams.lineHeightAbs = 0.f;
    _ellipsize = false;
}

void TextLayout::setText(const attributed_string& text) {
    _text = text;
    _invalid |= INVALID_GLYPHS;
}

void TextLayout::setFont(Font* font) {
    if (font != _defaultParams.font) {
        _defaultParams.font = font;
        _invalid |= INVALID_GLYPHS;
    }
}


void TextLayout::setColor(COLOR color) {
    if (_defaultParams.forecolor != color) {
        _defaultParams.forecolor = color;
        _invalid |= INVALID_OPS;
    }
}


void TextLayout::setMaxLines(int maxLines) {
    _maxLines = maxLines;
    _invalid |= INVALID_GLYPHS;
}

void TextLayout::setEllipsize(bool ellipsize) {
    if (ellipsize != _ellipsize) {
        _ellipsize = ellipsize;
        _invalid |= INVALID_GLYPHS;
    }
}

void TextLayout::setGravity(GRAVITY gravity) {
    _gravity = gravity;
    _invalid |= INVALID_POSITION;
}

void TextLayout::setLineHeight(float mul, float abs) {
    if (mul != _defaultParams.lineHeightMul) {
        _defaultParams.lineHeightMul = mul;
        _invalid |= INVALID_POSITION;
    }
    if (abs != _defaultParams.lineHeightAbs) {
        _defaultParams.lineHeightAbs = abs;
        _invalid |= INVALID_POSITION;
    }
}



#define isWhitespace(c) (c==' ')



SIZE TextLayout::measure(SIZE& constrainingSize) {

    // Avoid unnecessary measure()s. Note that changing the constraint does not affect _renderGlyphs.
    if (constrainingSize != _constrainingSize) {
        _constrainingSize = constrainingSize;
        _invalid |= INVALID_LINES;
    }
    
    // Phase 1: Rebuild _renderGlyphs
    if (_invalid & INVALID_GLYPHS) {
        
        // Reset glyphs state
        _renderGlyphs.clear();

        // Enumerate font and forecolor attributes at this phase
        attributed_string::enumerator eFont(attributed_string::attribute::type::Font, _text, _defaultParams.font);
        attributed_string::enumerator eForecolor(attributed_string::attribute::type::Forecolor, _text, &_defaultParams.forecolor);
        

        // Iterate over text
        uint32_t offset = 0;
        char32_t codepoint='\0', prevCh;
        int32_t i=0;
        while (offset < _text.end()) {
            prevCh = codepoint;
            codepoint = _text.readChar(offset);
            if (!codepoint) {
                break;
            }

            
            i++;
            
            // Ignore '\r' chars, they have no value
            if (codepoint=='\r') {
                continue;
            }
            
            // Tab-handling. For now just convert to spaces.
            if (codepoint=='\t') {
                app->log("todo: tabstops!");
                codepoint = ' ';
            }

            
            // Fetch the glyph for this character
            eFont.advanceTo(i-1);
            Font* font = (Font*)eFont.current();
            Glyph* glyph = font->getGlyph(codepoint);
            if (!glyph) {
                glyph = font->getGlyph('?'); // todo: should ask the font for it's default character...
                assert(glyph);
            }
            
            eForecolor.advanceTo(i-1);
            
            // Add the glyph to the list
            _renderGlyphs.emplace_back(RENDER_GLYPH {glyph, *(COLOR*)eForecolor.current()});
        }
        
        _invalid &= ~ INVALID_GLYPHS;
        _invalid |= INVALID_LINES;
    }
    
    // Phase 2: Break into lines
    if (_invalid & INVALID_LINES) {
        
        bool filledHorizontally = false;
        _rect.size.width = 0;
        _rect.size.height = 0;
        int breakOpportunity = -1;
        char32_t codepoint='\0', prevCh;
        _renderLines.clear();
        bool startNewLine = true;
        bool isNewParagraph = true;
        int32_t lineNumWithinPara = 0;
        Font* currentFont = _defaultParams.font;
        vector<RENDER_LINE>::iterator currentLine;
        float constrainingWidth = constrainingSize.width;
        float leftIndent = 0.;
        float rightIndent = 0.;
        float hangingIndent = 0.;
        int i=0,prevI=0;
        int numPremeasuredChars=0;
        float premeasuredCharsWidth=0.;


        attributed_string::enumerator eParagraphMetrics(attributed_string::attribute::type::ParagraphMetrics, _text, &_defaultParams.paragraphMetrics);


        for (;;) {
    
            if (startNewLine) {
                if (_maxLines && _renderLines.size()>=_maxLines) {
                    break;
                }
                
                // Move the attributes enumerator
                eParagraphMetrics.advanceTo(i);
                
                auto paragraphMetrics = (PARAGRAPH_METRICS*)eParagraphMetrics.current();

                leftIndent = paragraphMetrics->insets.left;
                rightIndent = paragraphMetrics->insets.right;
                
                // Update which-line-is-this state
                startNewLine = false;
                if (isNewParagraph) {
                    lineNumWithinPara = 0;
                    isNewParagraph = false;
                } else {
                    lineNumWithinPara++;
                }
                
                // Calculate hanging indent and add it to the leftIndent
                if (lineNumWithinPara > 0) {
                    if (lineNumWithinPara==1) {
                        hangingIndent = 0;
                        for (int oi=0 ; oi<paragraphMetrics->hangingIndentChars ; oi++) {
                            hangingIndent += _renderGlyphs[currentLine->start+oi].glyph->_advance.width;
                        }
                    }
                    leftIndent += hangingIndent;
                }
                
                // Update width constraint and push a new RENDER_LINE struct on the list
                constrainingWidth = constrainingSize.width - (leftIndent + rightIndent);

                // Push a new line struct
                currentLine = _renderLines.emplace(_renderLines.end(),
                        RENDER_LINE { i-numPremeasuredChars, numPremeasuredChars,
                            {0,0,premeasuredCharsWidth,0},
                            0,currentFont, leftIndent});
                breakOpportunity = -1;
                numPremeasuredChars = 0;
                prevI = i;
                
            }

            // Get next glyph
            if (i>=_renderGlyphs.size()) {
                break;
            }
            auto& rg = _renderGlyphs[i];
            prevCh = codepoint;
            codepoint = rg.glyph->_codepoint;
            currentFont = rg.glyph->_font;

            
            // Get the glyph advance
            float advance;
            bool isHardLineBreak = (codepoint == '\n');
            if (isHardLineBreak) {
                advance = 0;
            }
            else {
                advance = static_cast<float>(rg.glyph->_advance.width);

                // If at the start of a word, track this latest opportunity for a soft
                // line-break if one is needed later
                if (isWhitespace(prevCh) && !isWhitespace(codepoint)) {
                    breakOpportunity = i;
                }
            }
                
            // Current line too full to take glyph?
            auto newFill = currentLine->rect.size.width + advance;
            bool exceedsBounds = newFill >= constrainingWidth;
            if (exceedsBounds && currentLine->count && i!=prevI && codepoint!=' ') { // NB: trailing spaces don't trigger soft breaks
                filledHorizontally = true;
                
                // Break the loop if we've reached the maximum number of lines
                if (_maxLines && _renderLines.size()>=_maxLines) {
                    
                    if (_ellipsize) {
                        
                        // 'Rewind' the line to fit the ellipsis
                        auto forecolor = rg.forecolor;
                        Glyph* dotGlyph = currentFont->getGlyph('.');
                        float ellipsisWidth = dotGlyph->_advance.width * 2 + dotGlyph->_size.width;
                        while (currentLine->count > 0
                               && (currentLine->rect.size.width + ellipsisWidth) > constrainingWidth) {
                            currentLine->count--;
                            auto& lastChar = _renderGlyphs[currentLine->start + currentLine->count];
                            currentLine->rect.size.width -= lastChar.glyph->_advance.width;
                        }
                        
                        // Append the ellipsis glyphs
                        _renderGlyphs.erase(_renderGlyphs.begin() + currentLine->start + currentLine->count, _renderGlyphs.end());
                        _renderGlyphs.emplace_back(RENDER_GLYPH {dotGlyph, forecolor});
                        currentLine->rect.size.width += dotGlyph->_advance.width;
                        currentLine->count++;
                        _renderGlyphs.emplace_back(RENDER_GLYPH {dotGlyph, forecolor});
                        currentLine->rect.size.width += dotGlyph->_advance.width;
                        currentLine->count++;
                        _renderGlyphs.emplace_back(RENDER_GLYPH {dotGlyph, forecolor});
                        currentLine->rect.size.width += dotGlyph->_size.width;
                        currentLine->count++;

                    }
                    break;
                }

                // If there were no spaces on this line, force break at preceding char
                if (breakOpportunity < 0) {
                    breakOpportunity = (int32_t)i-1;
                }
                    
                // 'Rewind' to break point
                premeasuredCharsWidth = 0;
                numPremeasuredChars = i - breakOpportunity;
                for (int32_t bi=0; bi < numPremeasuredChars; bi++) {
                    auto& character = _renderGlyphs[breakOpportunity + bi];
                    premeasuredCharsWidth += character.glyph->_advance.width;
                }
                currentLine->count -= numPremeasuredChars;
                currentLine->rect.size.width -= premeasuredCharsWidth;
                    
                // Remove any trailing whitespace from the measured line length, it shouldn't
                // count towards the measured line width.
                int32_t bi = breakOpportunity;
                while (--bi>=0) {
                    auto& character = _renderGlyphs[bi];
                    if (!isWhitespace(character.glyph->_codepoint)) {
                        break;
                    }
                    currentLine->rect.size.width -= character.glyph->_advance.width;
                }
                    
                // Restart the loop with a new line
                startNewLine = true;
                continue;
            }
            
            // Add the glyph to the current line
            currentLine->rect.size.width += advance;
            currentLine->count++;
            if (currentLine->tallestFont != currentFont) {
                if (currentFont->_height > currentLine->tallestFont->_height) {
                    currentLine->tallestFont = currentFont;
                }
            }
            
            // If char was a hard break, start a new line & paragraph
            if (isHardLineBreak) {
                startNewLine = true;
                isNewParagraph = true;
            }
            
            i++;
        }
        

        // Calculate line baselines and heights so we know total height
        int numLines = (int)_renderLines.size();
        for (int i=0 ; i<numLines ; i++) {
            RENDER_LINE& line = _renderLines.at(i);
            float lineHeight = line.tallestFont->_height;
            lineHeight *= _defaultParams.lineHeightMul;
            lineHeight += _defaultParams.lineHeightAbs;
            

            float baseline = line.tallestFont->_ascent;
            for (auto charIndex = line.start ; charIndex<line.start+line.count ; charIndex++) {
                auto& rg = _renderGlyphs[charIndex];
                if (rg.cluster) {
                    continue;
                }
                Font* font = rg.glyph->_font;
                lineHeight = fmaxf(lineHeight, font->_height);
                baseline = fmaxf(baseline, font->_ascent);
                if (rg.glyph->_size.height > lineHeight) { // i.e. outsized glyph exceeds normal font lineheight
                    lineHeight = rg.glyph->_size.height + (font->_height-(font->_ascent-font->_descent));
                    baseline = lineHeight + font->_descent;
                }
            }
            line.baseline = baseline;
            _rect.size.width = MAX(_rect.size.width, line.rect.size.width);

            line.rect.size.height = lineHeight;
            _rect.size.height += lineHeight;
        }
    
        // Round measured size up to whole pixels
        _rect.size.width = ceilf(_rect.size.width);
        _rect.size.height = ceilf(_rect.size.height);

        _invalid &= ~INVALID_LINES;
        _invalid |= INVALID_POSITION;
    }
    
    return _rect.size;
}

bool TextLayout::glyphsNeedsPositioning() {
    return _invalid & INVALID_POSITION;
}
bool TextLayout::opsValid() {
    return !(_invalid & INVALID_OPS);
}

void TextLayout::layout(RECT& containingRect) {
    
    assert(!(_invalid & INVALID_GLYPHS)); // oops, needs a measure()!
    assert(!(_invalid & INVALID_LINES)); // oops, needs a measure()!

    // Avoid unnecessary layout()s
    if (containingRect != _containingRect) {
        _containingRect = containingRect;
        _invalid |= INVALID_POSITION;
    }

    // Phase 3: Position the glyphs and update line rects wrt each other and gravity
    if (_invalid & INVALID_POSITION) {
        
        float dY = 0;
        if (_gravity.vert != GRAVITY_TOP) {
            dY = containingRect.size.height - _rect.size.height;
            if (_gravity.vert == GRAVITY_CENTER) {
                dY /= 2;
            }
        }

        float currentBaselineOffset = 0;
        attributed_string::enumerator eBaselineOffsets(attributed_string::attribute::type::BaselineOffset, _text, &currentBaselineOffset);

        float y =  containingRect.origin.y + dY;
        _rect.origin.y = y;

        for (int i=0 ; i<_renderLines.size() ; i++) {
            RENDER_LINE& line = _renderLines.at(i);
            float x = containingRect.origin.x;
            if (_gravity.horz != GRAVITY_LEFT) {
                float dX = containingRect.size.width - line.rect.size.width;
                if (_gravity.horz == GRAVITY_CENTER) {
                    dX /= 2;
                }
                x += dX;
            }

            // Position all glyphs on the displayed line
            x += line.leftIndent;
            line.rect.origin.x = x;
            line.rect.origin.y = y;
            for (int32_t charIndex=line.start; charIndex<line.start+line.count ; charIndex++) {
                auto& rg = _renderGlyphs[charIndex];
                Glyph* glyph = rg.glyph;
                int ix = (int)x;
                if (!glyph) {
                    rg.topLeft.x = ix;
                    rg.topLeft.y = y;
                } else {
                    rg.topLeft.x = x + glyph->_origin.x;
                    rg.topLeft.y = y + line.baseline - (glyph->_size.height + glyph->_origin.y);
                    x += glyph->_advance.width;
                }
            }
            
            // Apply baseline offsets
            eBaselineOffsets.enumerate(line.start, line.start+line.count, [&](const attributed_string::attribute_usage& a) {
                for (int32_t i=a.start ; i<a.end ; i++) {
                    auto& rg = _renderGlyphs[i];
                    rg.topLeft.y -= a._floatVal * line.tallestFont->_height;
                }
            });

            y += line.rect.size.height;
        }
        
        _invalid &= ~ INVALID_POSITION;
        _invalid |= INVALID_OPS;
    }
}

RECT TextLayout::rect() {
    assert(!(_invalid & INVALID_GLYPHS));   // needs measure()
    assert(!(_invalid & INVALID_LINES));    // needs measure()!
    assert(!(_invalid & INVALID_POSITION)); // needs layout()!
    return _rect;
}

void TextLayout::invalidateOps() {
    _invalid |= INVALID_OPS;
}

void TextLayout::updateRenderOpsForView(View* view) {
    
    
    if (_invalid & INVALID_OPS) {

        // Get the view's visible rect (TODO: should be a View API)
        RECT visibleRect = view->getOwnRect();
        visibleRect.origin = view->getContentOffset();
        
        // Clear old ops
        resetRenderOps();
        
        // Set up an underlines enumerator
        attributed_string::enumerator eUnderlines(attributed_string::attribute::type::Underline, _text, &_defaultParams.underline);
        
        // Iterate over the lines to find the visible ones
        map<AtlasPage*,TextRenderOp*> uniqueOps;
        AtlasPage* currentAtlas = NULL;
        TextRenderOp* currentOp = NULL;
        for (int i=0 ; i<_renderLines.size() ; i++) {
            auto& line = _renderLines.at(i);
            if (visibleRect.intersects(line.rect)) {
                for (auto g=line.start ; g<line.start+line.count ; g++) {
                    auto& rg = _renderGlyphs[g];
                    if (!rg.glyph) {
                        continue;
                    }
                    auto node = rg.glyph->atlasNode();
                    if (currentAtlas != node->page) {
                        currentAtlas = node->page;
                        auto it = uniqueOps.insert({currentAtlas, NULL});
                        if (it.second) {
                            it.first->second = new TextRenderOp(currentAtlas);
                            _renderOps.push_back(it.first->second);
                        }
                        currentOp = it.first->second;
                    }
                    if (rg.glyph->_codepoint != ' ') {
                        currentOp->addGlyph(rg.glyph, RECT {rg.topLeft.x,rg.topLeft.y, static_cast<float>(rg.glyph->_size.width), static_cast<float>(rg.glyph->_size.height)}, rg.forecolor);
                    }
                }
                
                // Experimental underline support
                eUnderlines.enumerate(line.start, line.start+line.count, [=](const attributed_string::attribute_usage& attrib) {
                    int32_t from = MAX(attrib.start, line.start);
                    int32_t to = MIN(attrib.end, line.start+line.count);
                    auto& rgFrom = _renderGlyphs[from];
                    auto& rgTo = _renderGlyphs[to-1];
                    float x = rgFrom.rect().origin.x;
                    RECT underlineRect = {
                        x,
                        line.rect.top() + line.baseline + app->dp(2),
                        rgTo.rect().origin.x+rgTo.glyph->_size.width - x,
                        app->dp(1)
                    };
                    auto underlineOp = new RectRenderOp();
                    underlineOp->setRect(underlineRect);
                    underlineOp->setColor(_defaultParams.forecolor);
                    _renderOps.push_back(underlineOp);
                });
            }
        }

        // Add new ops to view
        for (auto it=_renderOps.begin() ; it!=_renderOps.end() ; it++) {
            view->addRenderOp(*it);
        }
        
        _invalid &= ~ INVALID_OPS;
    }
}

void TextLayout::resetRenderOps() {
    for (auto it : _renderOps) {
        it->_view->removeRenderOp(it);
        it->reset();
    }
    _renderOps.clear();
}




int32_t TextLayout::charIndexMove(int32_t charIndex, int dx, int dy) const {
    if (dy == 0) {
        charIndex += dx;
        if (charIndex <0) charIndex = 0;
        if (charIndex > _renderGlyphs.size()) charIndex = (int32_t)_renderGlyphs.size(); // yes, one past last char
    } else {
        POINT pt;
        charOriginFromIndex(charIndex, &pt, NULL, NULL);
        auto lineIndex = lineIndexFromCharIndex(charIndex, dy);
        auto& line = _renderLines[lineIndex];
        float minDiffX = FLT_MAX;
        int nearestCharIndex = 0;
        for (int i=0 ; i<line.count ; i++) {
            auto& rg = _renderGlyphs[line.start+i];
            float diffX = fabs(rg.topLeft.x - pt.x);
            if (diffX < minDiffX) {
                minDiffX = diffX;
                nearestCharIndex = i;
            }
        }
        charIndex = line.start + nearestCharIndex;
    }
    return charIndex;
}

int32_t TextLayout::charIndexFromPoint(const POINT& pt) const {
    const RENDER_LINE *line = NULL;
    for (auto &it: _renderLines) {
        if (pt.y >= it.rect.origin.y && pt.y < it.rect.bottom()) {
            line = &it;
            break;
        }
    }
    if (!line) {
        if (_renderLines.size() == 0) {
            return 0;
        }
        if (pt.y < _renderLines.begin()->rect.origin.y) {
            line = &_renderLines.at(0);
        } else {
            line = &_renderLines.at(_renderLines.size() - 1);
        }
    }
    
    int32_t nearestCharIndex = 0;
    if (pt.x >= line->rect.right()) {
        nearestCharIndex = line->count;
    } else {
        float minErr = FLT_MAX;
        for (int32_t charIndex = 0; charIndex < line->count; charIndex++) {
            auto& rg = _renderGlyphs[line->start + charIndex];
            float dx = fabs(pt.x - (rg.topLeft.x + rg.glyph->_size.width/2));
            if (dx < minErr) {
                nearestCharIndex = charIndex;
                minErr = dx;
            }
        }
    }
    return line->start + nearestCharIndex;
}


int32_t TextLayout::lineIndexFromCharIndex(int32_t charIndex, int dLine) const {
    for (int i=0 ; i<_renderLines.size() ; i++) {
        const auto& line = _renderLines.at(i);
        int32_t numCharacters = line.count;
        if (charIndex >= line.start && charIndex < line.start+numCharacters) {
            i += dLine;
            if (i<0) i=0;
            if (i>=_renderLines.size()) i=(int)_renderLines.size()-1;
            return i;
        }
    }
    assert(_renderLines.size());// there should always be at least one line
    return (int)_renderLines.size()-1;
}

RECT TextLayout::lineRect(int32_t lineIndex) {
    const auto& line = _renderLines.at(lineIndex);
    return line.rect;
}

RECT TextLayout::RENDER_GLYPH::rect() const {
    return {topLeft.x, topLeft.y, static_cast<float>(glyph->_size.width), static_cast<float>(glyph->_size.height)};
}
void TextLayout::charOriginFromIndex(int32_t charIndex, POINT* origin, float* ascent, float* descent) const {
    int lineIndex = lineIndexFromCharIndex(charIndex, 0);
    auto& line = _renderLines[lineIndex];
    if (ascent) {
        *ascent = line.baseline;
        *descent = -(line.rect.size.height - line.baseline);
    }
    
    if (charIndex < line.start + line.count) {
        const auto& glyphInfo = _renderGlyphs[charIndex];
        auto glyphRect = glyphInfo.rect();
        *origin = {glyphRect.left(), glyphRect.bottom()};
    } else {
        *origin = {line.rect.right(), line.rect.origin.y + line.baseline};
    }
}



