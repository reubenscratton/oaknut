//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup app_group
 * @class Display
 * @brief A physical display/screen. Accessed through app->_defaultDisplay.
 * \n
 */
class Display : public Object {
public:
    int _pixelWidth;
    int _pixelHeight;
    float _dpiX;
    float _dpiY;
    float _scale; // multiplier for points -> pixels.
    
    Display(int pixelWidth, int pixelHeight, float dpiX, float dpiY, float scale);
    
    enum SizeClass {
        Phone,      //      ... 5.5"
        Phablet,    // 5.5" ...   7"
        Tablet,     //   7" ...  12"
        Computer,   //  12" ...  30"
        TV          //  30" ...
    } _sizeClass;
    float size() const; // inches
    enum SizeClass sizeClass();
    
};
