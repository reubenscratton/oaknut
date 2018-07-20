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


Font::Font(const string& fontAssetPath, float size) : FontBase(fontAssetPath, size),
    _fontHelper(val::null()) {
    string fontFamily = "sans-serif";
    if (fontAssetPath.length()) {
        auto it = s_customFonts.find(fontAssetPath);
        if (it != s_customFonts.end()) {
            fontFamily = it->second;
        } else {
            ByteBuffer* fontData = app.loadAsset(fontAssetPath.data());
            fontFamily = fontAssetPath;
            while (fontFamily.extractUpTo("/", true).length() > 0) {}
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
    
    _fontHelper = val::global("FontHelper").new_(val(_name), val(_size), val(fontFamily));
}
    
Glyph* Font::createGlyph(char32_t ch, Atlas* atlas) {
    
    // Convert the character code to a Javascript string
    val str = val::global("String").call<val>("fromCharCode", val((int)ch));
    
    // Measure the glyph in Javascript
    val metrics = _fontHelper.call<val>("measure", str);
    
    // Create a glyph object
    Glyph* glyph = new Glyph(this, ch, 0);
    glyph->bitmapWidth = metrics["w"].as<int>();
    glyph->bitmapHeight = metrics["h"].as<int>();
    glyph->advance.width = glyph->bitmapWidth+1; // todo: fix this terrible hack
    
    // Reserve a space in the atlas
    glyph->atlasNode = atlas->reserve(glyph->bitmapWidth, glyph->bitmapHeight, 1);
    
    // Copy the pixels from the helper into the atlas
    Bitmap* bitmap = (Bitmap*)glyph->atlasNode->page->_bitmap._obj;
    val targetBuff = val(typed_memory_view((size_t)bitmap->_pixelData.cb, (unsigned char*)bitmap->_pixelData.data));
    _fontHelper.call<void>("copyPixels",
                           val(glyph->bitmapWidth),
                           val(glyph->bitmapHeight),
                           val(glyph->atlasNode->rect.origin.x),
                           val(glyph->atlasNode->rect.origin.y),
                           targetBuff,
                           val(bitmap->_pixelData.stride));

    return glyph;
}

#endif
