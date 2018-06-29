# emBeeb
BBC Micro emulator for iOS, MacOS, Android, Windows, Linux, and Web. Powered by Oaknut.


Building is done via make, just specify the platform name as the target, i.e. to build for MacOS:

`make macos`

As with any Oaknut project the following environment variables must be set appropriately:

**OAKNUT_DIR**      Mandatory, must point to your Oaknut repo
**ANDROID_SDK_DIR** - Location of Android SDK, only needed for Android builds
**XCODE_DIR** - Location of XCode package, only needed for MacOS and iOS builds
**EMSCRIPTEN_DIR** - Location of Emscripten SDK, needed for Web builds
