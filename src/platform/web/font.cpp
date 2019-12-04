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
            bytearray fontData;
            app->loadAsset(fontAssetPath, fontData);
            fontFamily = fontAssetPath;
            auto lastSlashPos = fontAssetPath.findLast("/");
            fontFamily = fontAssetPath.substr(lastSlashPos+1);
            if (!fontFamily.lengthInBytes()) {
                fontFamily = fontAssetPath;
            }
            fontFamily.hadSuffix(".ttf");
            fontFamily.hadSuffix(".otf");
            //app->log("Custom font %s loaded from %s", fontFamily.c_str(), fontAssetPath.c_str());
            string fontDataStr = "@font-face { font-family:\"";
            fontDataStr += fontFamily;
            fontDataStr += "\"; src: url(data:application/font-sfnt;base64,";
            fontDataStr += base64_encode((const uint8_t*)fontData.data(), fontData.size());
            fontDataStr += "); }";
            EM_ASM_({
                var style = document.createElement("style");
                style.type = "text/css";
                style.innerHTML = Pointer_stringify($0);
                document.getElementsByTagName('head')[0].appendChild(style);
            }, fontDataStr.c_str());
            s_customFonts.insert(std::make_pair(fontAssetPath, fontFamily));
        }
    }
    
    _fontHelper = val::global("FontHelper").new_(val(_name.c_str()), val(_size), val(_weight), val(fontFamily.c_str()));
    val metrics = _fontHelper.call<val>("measure", val("Mg"));
    _ascent = metrics["a"].as<float>();
    _descent = -metrics["d"].as<float>();
    _height = metrics["h"].as<float>();
}
    
Glyph* FontWeb::createGlyph(char32_t ch) {
    
    // Convert the character code to a Javascript string
    val str = val::global("String").call<val>("fromCharCode", val((int)ch));
    
    // Measure the glyph in Javascript
    val metrics = _fontHelper.call<val>("measure", str);
    
    // Create a glyph object
    Glyph* glyph = new Glyph(this, ch);
    glyph->_size.width = metrics["w"].as<int>();
    glyph->_size.height = metrics["h"].as<int>();
    glyph->_origin.y = -metrics["d"].as<int>();
    glyph->_advance.width = glyph->_size.width+1; // todo: fix this terrible hack
    

    return glyph;
}
void FontWeb::rasterizeGlyph(Glyph* glyph, Atlas* atlas) {
    // Reserve a space in the atlas
    glyph->_atlasNode = atlas->reserve(glyph->_size.width, glyph->_size.height, 1);
    
    // Copy the pixels from the helper into the atlas
    auto bitmap = glyph->_atlasNode->page->_bitmap.as<BitmapWeb>();
    val targetBuff = val(typed_memory_view((size_t)bitmap->_pixelData.cb, (unsigned char*)bitmap->_pixelData.data));
    _fontHelper.call<void>("copyPixels",
                           val(glyph->_size.width),
                           val(glyph->_size.height),
                           val(glyph->_atlasNode->rect.origin.x),
                           val(glyph->_atlasNode->rect.origin.y),
                           targetBuff,
                           val(bitmap->_pixelData.stride));
    bitmap->texInvalidate();
}

Font* Font::create(const oak::string &fontAssetPath, float size, float weight) {
    return new FontWeb(fontAssetPath, size, weight);
}

#endif
