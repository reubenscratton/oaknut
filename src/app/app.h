//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * \class App
 * \brief The single instance of this class is accessed through the global 'app' variable.
 */
class App : public Object {
public:

    App();
    
    /** Application entry point. Your implementation of this must instantiate a ViewController
        and set it as _window.rootViewController before returning */
    void main();

    /** The application window, which is your app's connection to the display
        and input subsystems of the host OS */
    ObjPtr<class Window> _window;

    /** Load a file from the assets directory, synchronously. Since this does IO it's best to
        limit use to app startup and background threads */
    class ByteBuffer* loadAsset(const char* assetPath);

    /** Gets the current system time, in milliseconds */
    long currentMillis(); // millis


    /** @name Logging
     * @{
     */

    /** Log an informational message */
    void log(char const* fmt, ...);

    /** Log a warning message prefixed with "Warning: " */
    void warn(char const* fmt, ...);

    /**@}*/

    /** Ask the underlying OS to schedule a redraw */
    void requestRedraw();

    /** @name Keyboard
     * @{
     */
    
    /** Show or hide the system soft keyboard, if there is one */
    void keyboardShow(bool show);
    
    /**@}*/
    
    void keyboardNotifyTextChanged();

    typedef enum {
        General,
        UserDocument,
        Cache
    } FileType;
    string getDirectoryForFileType(FileType fileType);
    
    
    /** @name Styles
     * @{
     */
    
    /** Load a style asset from the assets directory. Generally used in App::main() */
    void loadStyleAsset(const string& assetPath);

    /** Get a named style value */
    StyleValue* getStyleValue(const string& keypath);
    
    /** Get a named style value and coerce to a string */
    string getStyleString(const string& key);

    /** Get a named style value and coerce to a float */
    float getStyleFloat(const string& key);

    /** Get a named style value and coerce to a COLOUR */
    COLOUR getStyleColour(const string& key);

    /** Get the Font for a named font style */
    class Font* getStyleFont(const string& key);
    
    /**@}*/
    
    
    
    /** @name Layout
     * @{
     */
    
    /** Loads a view from an asset file. See the Layout Guide for details */
    class View* layoutInflate(const string& assetPath);
    
    /** Converts a 'dp' measurement into physical pixels. Dp is a device-independent pixel and is based on 320dpi */
    float dp(float dp);
    
    /** Converts a 'dp' measurement into physical pixels and floor()s the result to an integer */
    float idp(float pix);

    /**@}*/

    
    /** @name Settings
     * @{
     */
    
    /** Gets a named integer setting */
    int getIntSetting(const char* key, const int defaultValue);

    /** Sets a named integer setting. NB: call saveSettings() when finished updating settings */
    void setIntSetting(const char* key, int value);
    
    /** Gets a named string setting */
    string getStringSetting(const char* key, const char* defaultValue);

    /** Sets a named string setting. NB: call saveSettings() when finished updating settings. */
    void setStringSetting(const char* key, const char* value);
    
    /** Commits any changes to settings to permanent storage */
    void saveSettings();

    /**@}*/

    
protected:
    StyleMap _styles;


};

extern App app;
