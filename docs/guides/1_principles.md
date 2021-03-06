---
layout: default
---
# Principles

Oaknut was conceived in 2012 as a mobile app framework that would
bridge the gap between iOS and Android without any runtime performance cost
and without any Javascript. The goal was for an Oaknut app to be
indistinguishable from one built against a platform SDK, every native widget
from both platforms would be faithfully recreated down to the last pixel. Write
once, run not quite everywhere but at least on every smartphone.

It's still some way from reaching that lofty goal, mainly because
the original scope became considerably more ambitious with the
discovery of [Mozilla's Emscripten project](http://kripken.github.io/emscripten-site/). Suddenly it seemed possible that one could write a C++ app that would run not just on
mobile but on the web as well. All that was missing was the framework ...


#### Debug natively

One of the more attractive features of Oaknut is the ability
to debug code on the native platform (Mac, Linux, Windows) and then
later deploy to another (Android, iOS, Web). The fast build system
and not having to deploy to another machine or virtual machine make
incremental rebuild times a fraction of what is normal for mobile development.

You also get the benefit of native platform analysis tools, such as XCode
Instruments.


#### Extremely lightweight

At present the whole Oaknut source tree is compiled
into each app. This may switch to a precompiled library form at
a later date but at this stage it's more convenient to work with
in source form.

This approach also produces the smallest and fastest binaries, thanks
to dead code elimination.


#### Use of underlying OS

Oaknut aims to minimise wheel reinvention by leveraging those parts of the underlying
OS that are more or less identical to corresponding parts of other OSes. For example,
most of the 2D graphics APIs are a thin wrapper around OS APIs. Drawing rectangles,
lines, circles, decompressing JPEGs and PNGs, these things all done by the OS
since there's so little variance in how they are done.

Glyph rasterization is another job given to the OS, however glyph and text layout
is done by Oaknut (currently with custom logic but soon to be replaced by Pango).



## Application architecture

Oaknut aims to be unopinionated about app architecture but broadly follows the MVC pattern used in iOS: everything visual is a `View`, Views are grouped together and coordinated by `ViewController`s. There is no formal definition of a Model type but there is support for serialization and persistance.

An Oaknut project may either be a standalone application, in which case you must implement `App::main()`, or it may be a component of a larger application in which case you will need to invoke your Oaknut code by instantiating a `Window` via `Window::create()`, setting it's `rootViewController` property and then calling `window->show()`.

The top-level directory structure of a project is important because the build system has some assumptions about it:

	Makefile - contains the project definitions
	/app - the source code
	/assets - images, layout and style data, and anything else you want in the application bundle/package.
	/platform - platform specific files

Project files live in the project root, run "make xcode" or "make androidstudio" to generate these. Project files are optional, you can build with plain old `make` if you prefer.




## Why C++?

In my occasionally humble opinion the choice of programming language matters
less - much less - than having good and powerful APIs available. Oaknut is
implemented in C++ because that was the only language I knew that was common to
all the platforms I care about. It may be that Rust works equally well or better
and future research effort will explore implementing in other languages (there
is a strong case for a new language that translates to C++...).  



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
compiles to.


## Conventions and departures from 'standard' C++

NB: These notes are purely explanatory. Oaknut does not seek to mandate
any particular style of programming on others, these are just the decisions
that went into Oaknut itself. Feel free to write code the way YOU think best!

### Reference vs values

Oaknut makes a distinction between *reference types* - i.e. types that extend
the root `Object` class, and which are usually heap-allocated and passed by
pointer or reference - and *value types* which have no special lifetime
management and are usually passed by value.  A simple naming convention exists:
value type names are either all lowercase (`string`, `vector`, etc) or all
uppercase (e.g. `POINT`, `RECT`), whereas reference type names are always
camel-case.


### Object lifetimes

The Object root class implements a standard retain/release reference counting
scheme and has a companion smart pointer value type named `sp` to hold
strong references. Oaknut does not use the standard C++ smart pointer classes at all,
but that doesn't mean you can't use them if you want to.

Released objects (i.e. those whose refcount has decremented to zero)
are `free()`d between frames.

### Strings and the STL

Oaknut has it's own 'string' value type which is likely a better fit for
programmers coming from conventional application dev environments. It differs from C++'s
`std::string` in that it is a string of *characters*, not bytes, which may be
internally encoded as UTF-8 (the default) or UTF-32. It also has many useful
methods familiar to iOS and Java developers.

Oaknut does however use std::vector and std::map and a few other STL offerings.
It also adds a `bytearray` type.


### Multiple inheritance

Multiple inheritance is avoided except when secondary base classes are pure
interfaces, i.e. we copy Java, Obj-C, Swift, etc. It is occasionally helpful to
know that a pointer to a Foo is *always* a pointer to a Foo, or whatever Foo
extends, and doesn't need to be static_cast or otherwise adjusted to point to
the correct bit of memory.
