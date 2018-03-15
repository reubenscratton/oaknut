//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class App : public Object {
public:

    void main(); // implement this!

    ObjPtr<class Window> _window;

    float dp(float dp);
    float idp(float pix);
    class Data* loadAsset(const char* assetPath);
    long currentMillis(); // millis
    void log(char const* fmt, ...);
    void requestRedraw();
    string getAppHomeDir();
    void keyboardShow(bool show);
    void keyboardNotifyTextChanged();

};

extern App app;