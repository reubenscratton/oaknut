//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * \class Application
 * \brief The single instance of this class is accessed through the global 'app' variable.
 */
class App : public Object {
public:

    App();
    
    /**
     * Application entry point. Your implementation of this must instantiate a ViewController and set it as
     * _window.rootViewController before returning.
     */
    void main();

    /**
     * The application window, which is your app's connection to the display and input subsystems of the host OS.
     */
    ObjPtr<class Window> _window;

    /** @name Device <-> Physical pixel conversions
     * @{
     */
    /**
     * Converts a 'dp' measurement into physical pixels. Dp is a device-independent pixel and is based on 320dpi.
     */
    float dp(float dp);

    /**
     * Converts a 'dp' measurement into physical pixels and floor()s the result to an integer.
     */
    float idp(float pix);

    /**@}*/

    /**
     * Load a file from the assets directory, synchronously. Since this does IO it's best to limit use to app startup
     * and background threads.
     */
    class ByteBuffer* loadAsset(const char* assetPath);

    /**
     * Gets the current system time, in milliseconds
     */
    long currentMillis(); // millis


    /** @name Logging
     * @{
     */

    /**
     * Log a message to stdout
     */
    void log(char const* fmt, ...);

    /**
     * Log a warning message prefixed with the string "Warning: " to stderr
     */
    void warn(char const* fmt, ...);

    /**@}*/

    /**
     * Ask the underlying OS to schedule a redraw
     */
    void requestRedraw();

    /**
     * Show or hide the system soft keyboard, if there is one
     */
    void keyboardShow(bool show);
    void keyboardNotifyTextChanged();

    typedef enum {
        General,
        UserDocument,
        Cache
    } FileType;
    string getDirectoryForFileType(FileType fileType);
    
    
    void loadStyleAsset(const string& assetPath);
    string defaultFontName();
    float defaultFontSize();
    StyleValue* getStyleValue(const string& keypath);
    class Font* getFont(const string& key);
    string getString(const string& key);
    string getLocalizedString(const string& key);
    float getFloat(const string& key);
    COLOUR getColour(const string& key);
    class View* layoutInflate(const string& assetPath);
    
protected:
    StyleMap _styles;


};

extern App app;
