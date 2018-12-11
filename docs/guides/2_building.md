---
layout: default
---
# Building
Oaknut projects have a `Makefile` in their root and are built with plain old
GNU make, or with an IDE. The project Makefile is expected to only contain settings
and reference the real Oaknut makefile (oaknut.make) via an include statement
at the end.

A very simple project makefile:
````
PROJECT_NAME:=HelloWorld

include $(OAKNUT_DIR)/build/oaknut.make
````


### Using `make`

By default `make` without arguments will build the app version native to your
operating system. To build an app for the web or a mobile OS you specify it
with the `PLATFORM` variable, e.g.:
````
    make PLATFORM=web
````
Supported platforms are `macos`, `linux`, `web`, `ios`, `android`,
with `windows` coming soon.

Another optional variable is `CONFIG` which may be either `debug` or `release`.
The default is `debug`.


### Variables

Oaknut's make system expects some platform-specific information to be
passed by variable, for example `make PLATFORM=android` will expect to find
the location of the Android SDK in the `ANDROID_SDK_DIR` variable. A variable
can be set either in the host OS environment or in the project Makefile or
passed on the `make` command line.

| PLATFORM   | Variable               | Example          |
|------------|------------------------|------------------|
| `android`  | `ANDROID_SDK_DIR`      |                  |
|            | `ANDROID_PACKAGE_NAME` |                  |
|            | `ANDROID_ABIS`         | x86_64 arm64-v8a |
|            | `ANDROID_VER`          | 26               |
|            | `ANDROID_BUILDTOOLS`   | 26.0.2           |
|            | `ANDROID_KEYSTORE`     | ~/android.keystore |
|            | `ANDROID_KEYSTORE_PASSWORD` |             |
|            | `ANDROID_KEY_ALIAS`    |                  |
|            | `ANDROID_KEY_PASSWORD` |                  |
| web        | `EMSCRIPTEN_ROOT`      | ~/emsdk-portable/emscripten/1.38.6 |


### Build output

The built app will be found along with all intermediate build files in the
projects `.build/<platform>/<config>` subdirectory.
