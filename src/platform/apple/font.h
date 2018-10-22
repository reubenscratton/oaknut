//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class FontApple : public Font {
public:
    CTFontRef _ctfont;
    
    FontApple(const string& fontAssetPath, float size, float weight);
    
protected:
    CTFontRef createCTFont();
    Glyph* createGlyph(char32_t ch, Atlas* atlas);
};

