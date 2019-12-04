//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Display::Display(int pixelWidth, int pixelHeight, float dpiX, float dpiY, float scale) {
    _pixelWidth = pixelWidth;
    _pixelHeight = pixelHeight;
    _dpiX = dpiX;
    _dpiY = dpiY;
    _scale = scale;
}

float Display::sizeDiagonalInches() const {
    float x = (_pixelWidth*_scale) / _dpiX;
    float y = (_pixelHeight*_scale) / _dpiY;
    return hypotf(x,y);
}

enum Display::SizeClass Display::sizeClass() {
    float s = sizeDiagonalInches();
    if (s<=5.5) return Phone;
    if (s<=7) return Phablet;
    if (s<=12) return Tablet;
    if (s<=30) return Computer;
    return TV;
}

