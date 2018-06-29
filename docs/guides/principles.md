---
layout: default
---
# Principles

Oaknut was originally conceived in 2012 as a mobile app framework that would
bridge the gap between iOS and Android without any runtime performance cost.
The goal was for an Oaknut app to be absolutely indistinguishable from
one built against a native SDK, every native widget from both platforms
would be faithfully recreated down to the last pixel.

We're still some way from reaching that ambitious goal, mainly because
the original scope became even wider and even more crazily ambitious with the
discovery of [Mozilla's Emscripten project](http://kripken.github.io/emscripten-site/). Suddenly it seemed possible that one could write a C++ app that would run not just on
mobile but on the web as well. All that was missing was the framework ...


## Language vs APIs

In my opinion the choice of programming language matters less than having
good and powerful APIs. Oaknut is implemented in C++ because
that was the only language I knew that was common to all platforms. However,
it may be that Rust works equally well or better.  
