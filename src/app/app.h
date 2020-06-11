//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//



/**
 * @ingroup app_group
 * @class App
 * @brief Global object accessible through the global `app` variable that provides essential
 * top-level services such as loading resources, persisting configuration settings and many more.
 * \n
 * * [Entry point](#entry-point)\n
 * * [Assets](#assets)\n
 * * [Styles](#styles)\n
 * * [Layout](#layout)\n
 * * [Scheduling](#scheduling)\n
 * * [Time](#time)\n
 * * [Locale](#time)\n
 * * [Settings](#settings)\n
 * * [Files](#files)\n
 * \n
 */
class App : public Object {
public:

    /**  @cond INTERNAL */
    App();
    /**  @endcond */

    /** @name Entry point
     * @{
     */
    
    /** Application entry point. Your implementation of this must instantiate a ViewController
        and set it as _window.rootViewController before returning */
    virtual void main() = 0;

    /**@}*/

    
    /** @name Assets
     * @{
     */
    
    /** Load a file from the assets directory, synchronously. Since this does IO it's best to
     limit use to app startup and background threads */
    variant loadAssetSync(const string& assetPath);
    
    /** Load a file from the assets directory, asynchronously. */
    Task* loadAsset(const string& assetPath, std::function<void(variant&)> callback);

    /** Load and decompress a bitmap from the assets directory, asynchronously. */
    Task* loadBitmapAsset(const string& assetPath, std::function<void(Bitmap*)> callback);

    /**@}*/
    
    
    
    /** @name Styles
     * @{
     */
    
    /** Load a style asset from the assets directory. Generally used in App::main() */
    void loadStyleAssetSync(const string& assetPath);
    
    /** Get a named style value */
    const style* getStyle(const string& keypath);
    
    /** Get a named style value */
    string getStyleString(const string& key, const string& defaultString = string::empty);
    int getStyleInt(const string& key);
    float getStyleFloat(const string& key);
    COLOR getStyleColor(const string& key);
    
    Font* defaultFont();
    
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


    /** @name Time
     * @{
     */
    
    /** Gets the current system time, in milliseconds */
    TIMESTAMP currentMillis(); // millis

    /** Returns a 'friendly' text representation of a timestamp */
    string friendlyTimeString(TIMESTAMP timestamp);

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
    int getIntSetting(const string& key, int defaultValue=0);

    /** Sets a named integer setting. */
    void setIntSetting(const string& key, int value);
    
    /** Gets a named string setting */
    string getStringSetting(const string& key, const string& defaultValue = string::empty);

    /** Sets a named string setting. */
    void setStringSetting(const string& key, const string& value);
    
    /** Gets a named bool setting */
    bool getBoolSetting(const string& key, bool defaultValue=false);
    
    /** Sets a named bool setting. */
    void setBoolSetting(const string& key, bool value);

    /**@}*/
    
    
    
    /** The default display. Usually there is only one. */
    sp<Display> _defaultDisplay;
    
    /** The application window, which is your app's connection to the display
     and input subsystems of the host OS */
    sp<class Window> _window;
    

    /** Notifications */
    void subscribe(const char* notificationName, Object* observer, std::function<void(const char*, void*, variant&)> callback, void* source=NULL);
    void notify(const char* notificationName, void* source=NULL, variant data=variant());
    void unsubscribe(const char* notificationName, Object* observer);

protected:
    style _styles;


};

extern App* app;

