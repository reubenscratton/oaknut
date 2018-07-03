//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Font : public FontBase {
public:
    val _fontHelper;
    
    Font(const string& fontAssetPath, float size);
    
    Glyph* createGlyph(char32_t ch, Atlas* atlas);
};
