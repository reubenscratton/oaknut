---
layout: default
---
<iframe src="minesweeper/xx.html" style="margin-top:24px; margin-left:16px;" width="320" height="400" align="right">
</iframe>
Oaknut is an experimental GUI framework for truly cross-platform C++ apps. An
Oaknut app can be built to run natively on any major OS, and can also run in
a web browser via WebAssembly and WebGL. The 'Minesweeper' sample app is
running in an iframe to the right of this text.

Oaknut is currently at the "proof of concept" stage, almost no part of it is
anywhere near fully-featured. However all the main problems are solved - as far as I
can see - and all work from the current point lies in building out the remaining
APIs and UI widgetry that modern app developers need.

## Getting started
1. Clone the Oaknut repository:

    `git clone https://github.com/reubenscratton/oaknut`

2. Set the `OAKNUT_DIR` environment variable to point to it:

    `export set OAKNUT_DIR=/path/to/oaknut`

3. Select and build one of the samples in `oaknut/samples`

	`cd oaknut/samples/xxx`

    `make`

## Compiling
By default `make` without arguments will build the app version native to your
operating system. To build an app for the web or a mobile OS you specify it
with the `PLATFORM` variable, e.g.:

    make PLATFORM=web

Supported platforms are `macos`, `linux`, `web`, `ios`, `android`,
with `windows` coming soon.

Note that Oaknut's build system expects platform-specific information to be
passed by variable, for example `make PLATFORM=android` will expect to find
the location of the Android SDK in the `ANDROID_SDK_DIR` variable.

Another optional variable is `CONFIG` which may be either `debug` or `release`. The
default is `debug`.

The built app will be found along with all intermediate build files in the
projects `.build/<platform>/<config>` subdirectory.


## IDE support
Oaknut is not tied to any particular IDE, instead there are special make commands
which generate project files for several major IDEs:

- XCode `make xcode`
- CLion `make cmake`
- (more coming soon)


## Design notes

Oaknut is extremely lightweight. The whole source code is compiled
into each app. It may switch to a precompiled library form at
a later date but at this early stage it's convenient to work with this
way. Some lesser-used parts are opt-in via preprocessor definitions,
e.g. `OAKNUT_WANT_CAMERA`.

#### Threading
Oaknut apps are based on a simple event model. The application implements `App::main()`
whose job is to set a root ViewController on the global Window object. After that
everything happens in event handlers or on background threads. All drawing is done
via OpenGL on the primary/main thread.

Oaknut offers no way to directly create background threads. Downloading is performed
by background system threads (see `URLRequest`) but you may add code to process
that data on the background thread as it is downloaded.

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
is done by Oaknut (currently with custom logic but soon to be replaced by Pango).


#### Object lifetimes
Oaknut objects are reference-counted via the APIs `Object::retain()` and
`Object::release()`. Released objects (i.e. those whose refcount has decremented to zero)
are `free()`d between frames.


#### C++ usage
Oaknut uses a subset of C++11 in order to minimise the learning curve for those coming
from other languages and platforms. If you don't know a vtable from a r-value reference,
`it doesn't matter`. The general aim is for application code to look reasonably
close to what the equivalent code would have been in Java or Obj-C or Swift, rather
than be impenetrable blobs of pure C++.

Therefore Oaknut has little use of templates beyond a few indispensable STL containers
(`map`, `set`, and so on), it avoids multiple inheritance, operator overloading, RTTI,
'friend', 'mutable', traits, metaprogramming, etc. Without wishing to generate controversy
I personally dislike source code that is harder to read than the machine code it
compiles to, hence no use of Boost.

(One newish C++ feature Oaknut enthusiastically embraces is lambdas,
an indispensible and long overdue language feature.)



#### Layout
Oaknut supports declarative layout that is broadly similar to Android's
except that the syntax is a "light" form of JSON instead of XML, with fewer
redundant declarations. Unlike Android resource qualifiers are
allowed on individual attributes as well as files, e.g.

```
Label: {
  id: hello
  text@en_GB: "Hello!"
  text@en_US: "Hi!"
  text@fr_FR: "Bonjour!"
}
```

In this "light" JSON quotes are unnecessary for field names and are
optional for string values - the only real use for quotes is for multiline
text.  There is also no need for commas to separate fields.

In a layout file each object declaration must be the name of a View-derived
class, and each non-object attribute is some property supported by that class.


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
and not having to deploy to another machine or virtual machine make
incremental rebuild times a fraction of what is normal for mobile development.
