//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

//extern string base64_encode(const char* input, size_t len);

static map<string,string> s_customFonts;


FontWeb::FontWeb(const string& fontAssetPath, float size, float weight) : Font(fontAssetPath, size, weight),
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
            fontFamily.hadSuffix(".ttf");
            fontFamily.hadSuffix(".otf");
            //app.log("Custom font %s loaded from %s", fontFamily.data(), fontAssetPath.data());
            string fontDataStr = "@font-face { font-family:\"";
            fontDataStr += fontFamily;
            fontDataStr += "\"; src: url(data:application/font-sfnt;base64,";
            fontDataStr += base64_encode((const uint8_t*)fontData->data, fontData->cb);
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
    
    _fontHelper = val::global("FontHelper").new_(val(_name.data()), val(_size), val(_weight), val(fontFamily.data()));
}
    
Glyph* FontWeb::createGlyph(char32_t ch, Atlas* atlas) {
    
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
    BitmapWeb* bitmap = (BitmapWeb*)glyph->atlasNode->page->_bitmap._obj;
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

Font* Font::create(const oak::string &fontAssetPath, float size, float weight) {
    return new FontWeb(fontAssetPath, size, weight);
}

#endif
