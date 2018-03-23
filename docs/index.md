---
layout: default
---

<iframe src="minesweeper/xx.html" width="320" height="480" align="right">
</iframe>
Oaknut is an experimental GUI framework for truly cross-platform C++ apps. An Oaknut
app can be built to run natively on any major OS, or it can run in a web browser
via WebAssembly and WebGL. The sample project 'Minesweeper' should be
running in an iframe to the right of this text.

Oaknut is currently at the "proof of concept" stage, almost no part of it is
anywhere near fully-featured. However all the main problems are solved - as far as I
can see - and so almost all the work from the current point lies in
building out the remaining APIs and UI widgetry that modern app developers
expect to be available.

Oaknut is extremely lightweight. The whole source code is compiled
into each app. It may switch to a precompiled and/or dynamic library form at
a later date but at this early stage it's convenient to work with this
way. Some lesser-used parts are opt-in via preprocessor definitions,
e.g. `OAKNUT_WANT_CAMERA`.

#### Threading
Oaknut apps are based on a simple event model. The application implements `App::main()`
whose job is to set a root ViewController on the global Window object. After that
everything happens in event handlers or on background threads. All drawing is done
via OpenGL on the primary/main thread.

Oaknut offers no way to directly create background threads. Downloading is performed
by background system threads (see `URLRequest`) but you may add code to process data
on the background thread as it is downloaded.

Instead of threads Oaknut offers 'queues', one of which may execute on one or many
background threads, or none at all (i.e. on the main thread!) Obviously that last
thing is less than desirable and is only likely to happen on the web if
web workers are disabled.


#### Use of underlying OS
Oaknut aims to minimise wheel reinvention by leveraging those parts of the underlying
OS that are more or less identical to corresponding parts of other OSes. For example,
most of the 2D graphics APIs are a thin wrapper around OS APIs. Drawing rectangles,
lines, circles, decompressing JPEGs and PNGs, is all done by the OS since there's so
little variance in how these things are done.

Glyph rasterization is another job given to the OS, however glyph and text layout
is done by Oaknut, mainly for performance reasons (I may revisit this decision soon).


#### Object lifetimes
Oaknut objects are reference-counted via the APIs `Object::retain()` and
`Object::release()`. Released objects (i.e. those whose refcount has decremented to zero)
are `free()`d between frames.


#### C++
Oaknut's use of C++ aims to avoid unnecessary complexity. It has very little
use of templates beyond a few STL containers, it avoids multiple inheritance,
operator overloading, RTTI, 'friend', 'mutable', traits, metaprogramming, etc.
As a rule I distrust source code that is harder to read than the machine
code it compiles to, hence no Boost.



#### Build system
Oaknut apps are built through plain old Make.
```
make PLATFORM=macos CONFIG=debug
```
A subdirectory named `build` is created under the application source tree and
all object and other intermediate build files are placed there.

CMake is also supported, but mainly only so the excellent CLion
IDE could be used. All that the `CMakeLists.txt` files do is define
custom targets that all delegate to Make.

The platform makefiles expect information to be passed by variable,
for example the Android makefile needs to know where the Android SDK
is installed and you specify this via the `ANDROID_SDK_DIR` variable.


#### Layout
Oaknut supports declarative layout that is broadly similar to Android's
except that it is sorta-JSON instead of XML and has far less
redundant declarations. Unlike Android, resource qualifiers are
allowed on individual attributes as well as files, e.g.

```
Label: {
id: hello
text@en_GB: "Hello!"
text@en_US: "Hi ya!"
text@fr_FR: "Bonjour!"
}
```

Oaknut has also borrowed from Android's view layout system in that
the layout process is split into a measuring pass and a positioning
pass. Each view is asked (in `View::measure()`) to update it's own
intrinsic content size (if necessary), and then to set the size it
would like to be given that content size, the parent size,
and the view's sizing rules in the `_widthMeasureSpec` and
`_heightMeasureSpec` members. After all views have decided what size
they are, they are all positioned via calls to their `layout()` methods,
the default implementation of which uses the `_alignspecX` and
`_alignspecY` members to determine position.

#### Styles
As well as declarative layout Oaknut also supports declarative styling
in the same sorta-JSON text files. Style

#### Debugging

One of the more attractive features of Oaknut is the ability
to debug on the native platform (Mac, Linux, Windows) and then
later deploy to another (Android, iOS, Web). The fast build system
and not having to deploy to another machine make incremental
rebuild times a fraction of what is normal for mobile development.


