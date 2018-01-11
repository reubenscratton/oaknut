//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

#define GLYPH_LINEBREAK ((Glyph*)13)

TextRenderer::TextRenderer() {
}

void TextRenderer::setText(const string& text) {
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
    auto spanStartIterator = _spans.begin();
    auto spanEndIterator = _spans.begin();
    
    // Defaults
    Font* currentFont = _defaultFont;
    
    // Start with an empty line
    _lines.clear();
    _lines.push_back(TEXTLINE());
    TEXTLINE* currentLine = &_lines.back();

    // Iterate over all characters in the text
    Utf8Iterator it(_text);
    int i=0;
    int whitespaceStartIndex = -1;
    while (char32_t ch = it.next()) {
        
        // Apply any spans that start at this point
        while (spanStartIterator!=_spans.end() && i>=((*spanStartIterator))->_start) {
            Span* startingSpan = (*spanStartIterator);
            if (startingSpan->_font) {
                fontStack.push(currentFont);
                currentFont = startingSpan->_font;
            }
            if (startingSpan->_forecolour) {
                if (startingSpan->_forecolour != textRenderParams.forecolour) {
                    forecolourStack.push(textRenderParams.forecolour);
                    textRenderParams.forecolour = startingSpan->_forecolour;
                    paramsChanged = true;
                }
            }
            spanStartIterator++;
        }
        
        // Unapply spans that end at this point
        while (spanEndIterator!=_spans.end() && i>=((*spanEndIterator))->_end) {
            Span* endingSpan = (*spanEndIterator);
            if (endingSpan->_font) {
                currentFont = fontStack.top();
                fontStack.pop();
            }
            if (endingSpan->_forecolour) {
                if (textRenderParams.forecolour != forecolourStack.top()) {
                    textRenderParams.forecolour = forecolourStack.top();
                    forecolourStack.pop();
                    paramsChanged = true;
                }
            }
            spanEndIterator++;
        }
        
        // If we're starting a new line
        if (ch == '\n') {
            //GLYPHINFO glyphinfo = {GLYPH_LINEBREAK, RECT_Zero, currentParams};
            //currentLine->glyphinfos.push_back(glyphinfo);
            _lines.push_back(TEXTLINE());
            currentLine = &_lines.back();
            continue;
        }
        
        // Whitespace.
        if (ch=='\r') {
            continue;
        }
        if (ch=='\t') {
            oakLog("todo: tabstops!");
            ch = ' ';
        }
        if (ch == ' ') { // TODO: there are many more space chars in unicode
            if (whitespaceStartIndex < 0) { // first char of whitespace
                whitespaceStartIndex = i;
            }
        }
        
        // Fetch the next glyph
        Glyph* glyph = currentFont->getGlyph(ch);
        if (!glyph) {
            glyph = currentFont->getGlyph('?');
        }
        assert(glyph);
        

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

        GLYPHINFO glyphinfo = {glyph, {0,0,static_cast<float>(glyph->bitmapWidth),static_cast<float>(glyph->bitmapHeight)}, currentParams};
        currentLine->glyphinfos.push_back(glyphinfo);
        float glyphAdvance = glyph->advance.width;
        currentLine->bounds.size.width += glyphAdvance;

        // Determine if this new glyph can fit on current line or if a new line is needed
        if (maxSize.width<=0 || currentLine->bounds.size.width < maxSize.width) {
            if (whitespaceStartIndex >=0) {
                whitespaceStartIndex = -1;
            }
            continue;
        }
        
        // Glyph cannot fit on current line, start a new one
        filledHorizontally = true;
        _lines.push_back(TEXTLINE());
        currentLine = &_lines.back();

        // Soft line break:
        if (whitespaceStartIndex >=0) {
            oakLog("TODO: soft line breaks!");
            whitespaceStartIndex = -1;
        }
        
        // If we've reached maxLines, ellipsize the text and break out of all 3 loops.
        if (_maxLines != 0) {
            /*if (paragraph->lines.size() == maxLines) {
             line->glyphCount-=2; // this is an approximation. TODO: make this perfect.
             line->ellipsis = true;
             i = (int)_paragraphs.size();
             runIndex = (int)paragraph->runs.size();
             k = (int)run->glyphs.size();
             continue;
             }*/
        }
        
        i++;
    }
    
    
    // Iterate lines, calculate each line height and do naive typesetting
    int numLines = (int)_lines.size();
    float y = 0;
    for (int i=0 ; i<numLines ; i++) {
        TEXTLINE& line = _lines.at(i);
        float lineHeight = 0;
        float baseline = 0;
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
        line.bounds.origin = POINT_Make(0, y);
        line.bounds.size.height = lineHeight;
        line.baseline = baseline;
        _measuredSize.width = max(_measuredSize.width, line.bounds.size.width);
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
            j->rect = RECT_Make(x + glyph->bitmapLeft,
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

    RECT visibleRect = view->getBounds();
    visibleRect.origin.y = view->_contentOffset.y;

    
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

TextRenderer::TEXTLINE* TextRenderer::getLineForGlyphIndex(int glyphIndex) {
    for (auto it = _lines.begin() ; it != _lines.end() ; it++) {
        TEXTLINE& line = *it;
        if (glyphIndex < line.glyphinfos.size()) {
            return &line;
        }
        glyphIndex -= line.glyphinfos.size();
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
            GLYPHINFO& glyphInfo = line->glyphinfos[glyphIndex];
            *origin = glyphInfo.rect.origin;
            return;
        } else {
            if (line->glyphinfos.size() > 0) {
                auto glyphInfo = line->glyphinfos.rbegin();
                *origin = POINT_Make(glyphInfo->rect.right(), glyphInfo->rect.bottom());
                return;
            }
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
    

