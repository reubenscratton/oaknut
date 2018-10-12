//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup app_group
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
    TIMESTAMP currentMillis(); // millis

    /** @name Logging
     * @{
     */

    /** Log an informational message */
    void log(char const* fmt, ...);

    /** Log a warning message prefixed with "Warning: " */
    void warn(char const* fmt, ...);

    /**@}*/

    /** @name Keyboard
     * @{
     */
    
    /**@}*/
    
    string getPathForGeneralFiles();
    string getPathForUserDocuments();
    string getPathForCacheFiles();
    string getPathForTemporaryFiles();
    
    /** @name Styles
     * @{
     */
    
    /** Load a style asset from the assets directory. Generally used in App::main() */
    void loadStyleAsset(const string& assetPath);

    /** Get a named style value */
    const StyleValue* getStyleValue(const string& keypath);
    
    /** Get a named style value and coerce to a string */
    string getStyleString(const string& key, const char* defaultString = NULL);

    /** Get a named style value and coerce to a float */
    float getStyleFloat(const string& key);

    /** Get a named style value and coerce to a COLOR */
    COLOR getStyleColor(const string& key);
    
    /**@}*/
    
    
    
    /** @name Layout
     * @{
     */
    
    /** Loads a view from an asset file. See the Layout Guide for details */
    class View* layoutInflate(const string& assetPath);
    void layoutInflateExistingView(View* view, const string& assetPath);
    
    /** Converts a 'dp' measurement into physical pixels. Dp is a device-independent pixel and is based on 320dpi */
    float dp(float dp);
    
    /** Converts a 'dp' measurement into physical pixels and floor()s the result to an integer */
    float idp(float pix);

    /**@}*/

    
    /** @name Locale
     * @{
     */

    /** Returns ISO-3166 two-letter country code the device is configured for, which is
        not necessarily the country the device is currently in.  */
    string currentCountryCode() const;
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
    
    /**@}*/

    
    string friendlyTimeString(TIMESTAMP timestamp);
    
protected:
    StyleValue _styles;


};

extern App app;
