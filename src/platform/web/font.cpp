//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

extern string base64_encode(const char* input, size_t len);

static map<string,string> s_customFonts;

static void setGlyphMetrics(Font* font, Glyph* glyph, int glyphWidth) {
    glyph->bitmapWidth = ceilf(glyphWidth);
    glyph->bitmapHeight = ceilf(font->_size);
    glyph->advance.width = glyph->bitmapWidth+1; // todo!!!
}


Font::Font(const string& fontAssetPath, float size) : FontBase(fontAssetPath, size), _canvas(val::null()), _ctxt(val::null()) {
    _canvas = val::global("document").call<val>("createElement", val("canvas"));
    _canvas.set("width", _size);
    _canvas.set("height", _size);
    string fontFamily = "sans-serif";
    if (fontAssetPath.length()) {
        auto it = s_customFonts.find(fontAssetPath);
        if (it != s_customFonts.end()) {
            fontFamily = it->second;
        } else {
            ByteBuffer* fontData = app.loadAsset(fontAssetPath.data());
            fontFamily = fontAssetPath;
            while (stringExtractUpTo(fontFamily, "/", true).length() > 0) {}
            stringEndsWith(fontFamily, ".ttf", true);
            stringEndsWith(fontFamily, ".otf", true);
            //app.log("Custom font %s loaded from %s", fontFamily.data(), fontAssetPath.data());
            string fontDataStr = "@font-face { font-family:\"";
            fontDataStr += fontFamily;
            fontDataStr += "\"; src: url(data:application/font-sfnt;base64,";
            fontDataStr += base64_encode((const char*)fontData->data, fontData->cb);
            fontDataStr += "); }";
            EM_ASM_({
                var style = document.createElement("style");
                style.type = "text/css";
                style.innerHTML = Pointer_stringify($0);
                document.getElementsByTagName('head')[0].appendChild(style);
            }, fontDataStr.data());
            s_customFonts.insert(std::make_pair(fontAssetPath, fontFamily));
        }
    }
    _ctxt = _canvas.call<val>("getContext", val("2d"));
    char ach[32];
    sprintf(ach, "%dpx %s", (int)_size, fontFamily.data());
    _ctxt.set("font", val(string(ach)));
    _ctxt.set("textBaseline", val(string("top")));
}
    
Glyph* Font::createGlyph(char32_t ch, Atlas* atlas) {
    string str;
    stringAppendCodepoint(str, ch);
    val::global("GlobalObjectTracker").set(7, _ctxt);
    Glyph* glyph = new Glyph(this, ch, 0);
    EM_ASM_({
        var ctxt = GlobalObjectTracker[7];
        var jstr = UTF8ToString($2);
        var textMetrics = ctxt.measureText(jstr);
        Runtime.dynCall('viii', $3, [$0,$1,textMetrics.width]);
    }, this, glyph, str.data(), setGlyphMetrics);
    
    glyph->atlasNode = atlas->reserve(glyph->bitmapWidth, glyph->bitmapHeight, 1);
    POINT pt = glyph->atlasNode->rect.origin;
    Bitmap* bitmap = (Bitmap*)glyph->atlasNode->page->_bitmap._obj;
    
    EM_ASM_({
        var ctxt = GlobalObjectTracker[7];
        ctxt.clearRect(0,0,$1, $2);
        ctxt.fillText(UTF8ToString($0), 0,0);
    }, str.data(), glyph->bitmapWidth, glyph->bitmapHeight);
    
    val targetBuff = val(typed_memory_view((size_t)bitmap->_pixelData.cb, (unsigned char*)bitmap->_pixelData.data));
    val::global("GlobalObjectTracker").set(8, targetBuff);
    EM_ASM_({
        var ctxt = GlobalObjectTracker[7];
        var targetBuff = GlobalObjectTracker[8];
        var imgdata = ctxt.getImageData(0, 0, $2, $3);
        var o = 0;
        for (y=0 ; y<$3 ; y++) {
            for (x=0 ; x<$2 ; x++) {
                targetBuff[(y+$1)* $4 + $0+x] = imgdata.data[o+3]; // i.e. only copy alpha channel bytes
                o+=4;
            }
        }
    }, glyph->atlasNode->rect.origin.x, glyph->atlasNode->rect.origin.y, glyph->bitmapWidth, glyph->bitmapHeight, bitmap->_pixelData.stride);
    
    return glyph;
}

#endif
