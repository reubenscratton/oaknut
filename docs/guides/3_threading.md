---
layout: default
---
# Threading

Oaknut apps have a very simple thread model, they are for the most part
single threaded. All rendering, event handling, app logic, etc, occurs
on the primary/main thread.


### main()

The application is expected to implement `App::main()` whose job is to set
a root `ViewController` on the application `Window` object. The runtime
implements an appropriate event and render loop under the hood.


### Workers

Owing to the temporary limitation of current web standards there is not yet a
general purpose threading API such as pthreads, but instead Oaknut borrows the
web's `Worker` concept for background processing. For devs who may be unfamiliar
with the concept, Workers are rather like out-of-process services: you send them
serialized input and at some future point you get back some serialized
output.

In other words Oaknut does support background threads, they just can't share
memory with the main thread.

The Oaknut source code includes a number of workers, usually for media related
tasks such as image compression, audio conversion, and face detection.

NB: If you use Workers and wish to target the web you must build the worker
separate from the main build. See the `build_worker` script for details.


### Background code without Workers

There are one or two places where, on non-web platforms, it is possible
to have code running on a system background thread. Download processing
is one such place: `URLRequest::onGotResponseInBackground` is a handler
that will run in the background on the platform thread performing the
download.


### Tasks

A `Task` represents a cancelable async operation being performed by the platform.
For example `Bitmap::createFromData()` returns a `Task` that represents a background
operation that decodes a bitmap from PNG or JPEG data.

A `Task` is also returned by `App::postToMainThread`, the API for running
callbacks on the main thread.

Note that you should never need (AFAICS) to instantiate a Task directly.
