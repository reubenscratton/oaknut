

Oaknut C++ uses a subset of C++. Without wishing to generate too much controversy
the goal is, as it always is, to avoid unnecessary complexity. Hence you won't find
much use of templates, multiple inheritance, operator overloading, and absolutely
no use of RTTI, 'friend', 'mutable', traits, metaprogramming, etc. I am one of those
who is profoundly skeptical as to the virtues of Boost.

Oaknut aims to leverage those parts of the underlying OS that are more or less
identical to corresponding parts of other OSes. Hence for glyph rasterization Oaknut
uses underlying OS services to do so, rather than importing a heavyweight lib
like Freetype.

Oaknut apps are based on a simple event model and the oakMain() function. oakMain()
only needs to set the root ViewController on the global Window object, everything
else happens in event handlers or on background threads. Oaknut offers no way
to directly create a background thread as such, the abstraction offered is a 'queue'
which may execute on one or many background threads, or none at all (i.e. on the main thread!)
Obviously that last thing is less than desirable

Oaknut objects are reference-counted via the APIs Object::retain() and
Object::release(). Released objects (i.e. those whose refcount has decremented to zero)
are free()d between frames.

Oaknut currently renders everything through OpenGL (WebGL on the web).

