//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

#define GLYPH_LINEBREAK ((Glyph*)13)

TextRenderer::TextRenderer() {
}

void TextRenderer::setText(const string& text) {
    _text = text;
    _measuredSizeValid = false;
}
void TextRenderer::setAttributedText(const AttributedString& text) {
    _text = text;
    _measuredSizeValid = false;
}

void TextRenderer::setDefaultColour(COLOUR colour) {
    _defaultColour = colour;
    //_renderOpsValid = false;
    _measuredSizeValid = false; // lazy. colour doesn't affect size!
}

void TextRenderer::setDefaultFont(Font* font) {
    _defaultFont = font;
    _measuredSizeValid = false;
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
    measure(SIZE_Make(0,0));
}

#define isWhitespace(c) (c==' ')

void TextRenderer::measure(SIZE maxSize) {
    assert(!_measuredSizeValid); // measure() has been called pointlessly
    
    bool filledHorizontally = false;
    _measuredSize.width = 0;
    _measuredSize.height = 0;
    _renderParams.clear();
    
    // Initial text render params
    TEXTRENDERPARAMS textRenderParams;
    textRenderParams.atlasPage = NULL;
    textRenderParams.forecolour = _defaultColour;
    textRenderParams.renderOp = NULL;
    TEXTRENDERPARAMS* currentParams = &textRenderParams;
    bool paramsChanged = false;
    
    // Prepare to walk the ordered spans collection
    stack<Font*> fontStack;
    stack<COLOUR> forecolourStack;
    auto spanStartIterator = _text._attributes.begin();
    auto spanEndIterator = _text._attributes.begin();
    
    // Defaults
    Font* currentFont = _defaultFont;
    
    // Start with an empty line
    _lines.clear();
    auto currentLine = _lines.emplace(_lines.end(), TEXTLINE());
    currentLine->font = currentFont;

    // Iterate over all characters in the text
    StringProcessor it(_text);
    int i=0;
    char32_t ch='\0', prevCh;
    int breakOpportunity = -1;
    while (true) {
        prevCh = ch;
        ch = it.next();
        if (!ch) {
            break;
        }
        
        
        // Unapply spans that end at this point
        while (spanEndIterator!=_text._attributes.end() && i>=spanEndIterator->end) {
            auto endingSpan = (*spanEndIterator);
            if (endingSpan.attribute._type == Attribute::Type::Font) {
                currentFont = fontStack.top();
                fontStack.pop();
            }
            if (endingSpan.attribute._type == Attribute::Type::Forecolour) {
                textRenderParams.forecolour = forecolourStack.top();
                forecolourStack.pop();
                paramsChanged = true;
            }
            spanEndIterator++;
        }

        // Apply any spans that start at this point
        while (spanStartIterator!=_text._attributes.end() && i>=spanStartIterator->start) {
            auto startingSpan = (*spanStartIterator);
            if (startingSpan.attribute._type == Attribute::Type::Font) {
                fontStack.push(currentFont);
                currentFont = startingSpan.attribute._font;
            }
            if (startingSpan.attribute._type == Attribute::Type::Forecolour) {
                forecolourStack.push(textRenderParams.forecolour);
                textRenderParams.forecolour = startingSpan.attribute._colour;
                paramsChanged = true;
            }
            spanStartIterator++;
        }
        
        
        
        // Hard line-break
        if (ch == '\n') {
            currentLine = _lines.emplace(_lines.end(), TEXTLINE());
            currentLine->font = currentFont;
            breakOpportunity = -1;
            continue;
        }
        
        // Whitespace.
        if (ch=='\r') {
            continue;
        }
        else if (ch=='\t') {
            app.log("todo: tabstops!");
            ch = ' ';
        }
        if (isWhitespace(prevCh) && !isWhitespace(ch)) {
            breakOpportunity = (int32_t)currentLine->glyphinfos.size(); // track first non-whitespace char after run of whitespace chars
        }
        
        // Fetch the next glyph
        Glyph* glyph = currentFont->getGlyph(ch);
        if (!glyph) {
            glyph = currentFont->getGlyph('?');
            assert(glyph);
        }

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

        // Will adding this glyph to the current line cause the line to exceed bounds?
        auto glyphAdvance = glyph->advance.width;
        auto newBoundsWidth = currentLine->bounds.size.width + glyphAdvance;
        bool exceedsBounds = newBoundsWidth>=maxSize.width && currentLine->glyphinfos.size() > 0;
        
        // Soft line break
        if (exceedsBounds && !isWhitespace(ch)) {
            filledHorizontally = true;
            if (breakOpportunity < 0) { // if there were no spaces on this line, force a break at the preceding char
                breakOpportunity = (int32_t)currentLine->glyphinfos.size()-1;
            }
            vector<GLYPHINFO> extract;
            float xx = 0;
            while (breakOpportunity < currentLine->glyphinfos.size()) {
                auto gi = currentLine->glyphinfos.begin() + breakOpportunity;
                xx += gi->glyph->advance.width;
                extract.push_back(*gi);
                currentLine->glyphinfos.erase(gi);
            }
            currentLine->bounds.size.width -= xx;
            currentLine = _lines.emplace(_lines.end(), TEXTLINE());
            currentLine->font = currentFont;
            currentLine->bounds.size.width = xx;
            currentLine->glyphinfos = extract;
            breakOpportunity = -1;
        }
        
        // Add the glyph to the current line
        currentLine->glyphinfos.emplace_back(GLYPHINFO {glyph, {0,0,static_cast<float>(glyph->advance.width),static_cast<float>(glyph->bitmapHeight)}, currentParams});
        currentLine->bounds.size.width += glyphAdvance;

        // If we've reached maxLines, ellipsize the text and break out of all 3 loops.
        /*if (_maxLines != 0) {
            if (paragraph->lines.size() == maxLines) {
             line->glyphCount-=2; // this is an approximation. TODO: make this perfect.
             line->ellipsis = true;
             i = (int)_paragraphs.size();
             runIndex = (int)paragraph->runs.size();
             k = (int)run->glyphs.size();
             continue;
             }
        }*/
        
        i++;
    }
    
    
    // Iterate lines, calculate each line height and do naive typesetting
    int numLines = (int)_lines.size();
    float y = 0;
    int32_t startingIndex = 0;
    for (int i=0 ; i<numLines ; i++) {
        TEXTLINE& line = _lines.at(i);
        float lineHeight = line.font->_height;
        float baseline = line.font->_ascent;
        for (auto jt = line.glyphinfos.begin() ; jt!=line.glyphinfos.end() ; jt++) {
            GLYPHINFO& glyphInfo = *jt;
            Font* font = glyphInfo.glyph->_font;
            lineHeight = fmaxf(lineHeight, font->_height);
            baseline = fmaxf(baseline, font->_ascent);
            if (glyphInfo.glyph->bitmapHeight > lineHeight) { // i.e. outsized glyph exceeds normal font lineheight
                lineHeight = glyphInfo.glyph->bitmapHeight + (font->_height-(font->_ascent-font->_descent));
                baseline = lineHeight + font->_descent;
            }
        }
        line.bounds.size.height = lineHeight;
        line.bounds.origin = POINT_Make(0, y);
        line.baseline = baseline;
        line.startingIndex = startingIndex;
        startingIndex += line.glyphinfos.size();
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
        for (auto j=line.glyphinfos.begin() ; j!=line.glyphinfos.end() ; j++) {
            Glyph* glyph = j->glyph;
            j->rect = RECT(x + glyph->bitmapLeft,
                            y + line.baseline - (glyph->bitmapHeight+ glyph->bitmapTop),
                            glyph->atlasNode->rect.size.width,
                            glyph->atlasNode->rect.size.height);
            x += glyph->advance.width;
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
            for (auto j=line.glyphinfos.begin() ; j!=line.glyphinfos.end() ; j++) {
                if (currentParams != j->renderParams) {
                    currentParams = j->renderParams;
                    if (!currentParams->renderOp) {
                        currentParams->renderOp = new TextRenderOp(view, currentParams);
                    }
                    auto t = uniqueParams.insert(currentParams);
                    if (t.second) {
                        _textOps.push_back(currentParams->renderOp);
                    }
                }
                if (j->glyph->charCode != ' ') {
                    currentParams->renderOp->addGlyph(j->glyph, j->rect);
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

int32_t TextRenderer::characterIndexFromPoint(const POINT& pt) const {
    assert(_measuredSizeValid);
    const TEXTLINE* line = NULL;
    for (auto& it: _lines) {
        if (pt.y>=it.bounds.origin.y && pt.y<it.bounds.bottom()) {
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
            line = &_lines.at(_lines.size()-1);
        }
    }
    return line->startingIndex + characterIndexFromX(line, pt.x);
}

int32_t TextRenderer::characterIndexFromX(const TEXTLINE* line, const float x) const {
    float minErr = MAXFLOAT;
    int charIndex=0, bestIndex=0;
    for (auto it : line->glyphinfos) {
        float dx = fabs(x - it.rect.origin.x);
        if (dx < minErr) {
            bestIndex = charIndex;
            minErr = dx;
        }
        charIndex++;
    }
    return bestIndex;
}

const TextRenderer::TEXTLINE* TextRenderer::getLineForGlyphIndex(int glyphIndex, int dLine) const {
    for (int i=0 ; i<_lines.size() ; i++) {
        const TEXTLINE& line = _lines.at(i);
        if (glyphIndex < line.glyphinfos.size()) {
            i += dLine;
            if (i<0 || i>=_lines.size()) {
                return NULL;
            }
            return &_lines.at(i);
        }
        glyphIndex -= line.glyphinfos.size();
    }
    return NULL;
}

TextRenderer::TEXTLINE* TextRenderer::getLineForGlyphIndex(int& glyphIndex) {
    
    for (auto it = _lines.begin() ; it != _lines.end() ; it++) {
        TEXTLINE& line = *it;
        if (glyphIndex < line.glyphinfos.size()) {
            return &line;
        }
        glyphIndex -= line.glyphinfos.size();
    }
    if (_lines.size()) {
        glyphIndex += _lines.rbegin()->glyphinfos.size();
    }
    return NULL;
}

void TextRenderer::getGlyphOrigin(int glyphIndex, POINT* origin, float* ascent, float* descent) {
    assert(_measuredSizeValid);
    TEXTLINE* line = getLineForGlyphIndex(glyphIndex);
    if (!line && _lines.size() > 0) {
        line = &(*_lines.rbegin());
    }
    if (line) {
        *ascent = line->baseline;
        *descent = -(line->bounds.size.height - line->baseline);
        if (glyphIndex < line->glyphinfos.size()) {
            const GLYPHINFO& glyphInfo = line->glyphinfos[glyphIndex];
            *origin = POINT_Make(glyphInfo.rect.left(), glyphInfo.rect.bottom());
            return;
        } else {
            if (line->glyphinfos.size() > 0) {
                auto glyphInfo = line->glyphinfos.rbegin();
                *origin = POINT_Make(glyphInfo->rect.left() + glyphInfo->glyph->advance.width, glyphInfo->rect.bottom());
            } else {
                *origin = POINT_Make(line->bounds.origin.x, line->bounds.origin.y + line->baseline);
            }
            return;
        }
    }
    // TextRenderer is completely empty so return where first glyph would go
    float lineHeight = _defaultFont->_height;
    *ascent = _defaultFont->_ascent;
    *descent = _defaultFont->_descent;
//    float space = lineHeight - (*ascent - *descent);
    origin->x = _layoutRect.left();
    origin->y = _layoutRect.top() + lineHeight + *descent;
    if (_gravity.vert == GRAVITY_CENTER) {
        origin->y += (_layoutRect.size.height - lineHeight) / 2;
    } else if (_gravity.vert == GRAVITY_BOTTOM) {
        origin->y += (_layoutRect.size.height - lineHeight);
    }
}
    /*
     // Ellipsis?
     if (line->ellipsis) {
     //Glyph* dotGlyph = currentRun->font->getGlyph('.');
     //float dx = dotGlyph->advance.width;
     for (int k=0 ; k<3 ; k++) {
     //context->drawModel(&dotGlyph->quad);
     //context->translate(dx,0,0);
     }
     }*/
    

