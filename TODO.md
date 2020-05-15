Oaknut TODOs
============

Feature: URL cache should not issue any remote request if cache entry still valid. (By default, obviously... lets allow a flag to override it).

Feature: Tasks need priorities.

Feature: URL RAM cache has very wrong byte count for images. Need an API for measuring RAM cost of decoded URLs.

Feature: Logging categories? Must be compiled in or out, no runtime switching.

Feature: URL disk cache max size enforcement. Low-priority background IO task

Feature: Asset loads should share URLCache.

Feature: oak::bytearray needs the capacity feature that bytestream has. oak::string has shared buffers and copy-on-write which would be VERY USEFUL for bytearray too. Why not let bytearray share the same shared buffer mechanism that string uses??

Image cache: The naive approach to image caching is to have a fixed-size cache of fully decompressed images with each image keyed by its URL. A better approach would be to give the app control of the size that an image is rasterized, since it is often the case that large bitmaps are rendered into smaller rectangles. BBC News is a case in point: news images are delivered in any of a fixed set of widths, client code is expected to round up to the nearest width it needs. But Im talking bollocks... the image width is part of the URL, so BBC News does *not* have one-URL-multiple-images and so we don't need to consider it.

However, it's still the case that images are often rendered at a different size to its intrinsic size, which means the rasterizer has to interpolate, which is not free. Oaknut should support an option to resize an image at decode time, it could be an important optimization. If the option is used we should disallow ImageView from initiating downloads before it has a non-zero size. Note that contentMode affects render size too.


ViewPager: page-aligned scrolling

Indexes: the cells should be 4 text lines tall. Currently timestamp text occludes 3rd line of headline.

iOS: Metal: Uploads are now on a dispatch queue, however they need to sync with frame commit/present. We want to be able to put something in the GPU command buffer that will cause the GPU to pause until some CPU-controlled state is set. MtlEvent might be what we need. Specifically when we dispatch an upload to the uploader queue (AND the upload is needed for the current frame!) we should use [MTLCommandBuffer encodeWaitForEvent:value:] to 

More generally, the approach to vertex and index buffers MAY need a total change. We currently maintain single static buffers, however there is a strong argument for constructing these buffers anew for each frame. 

Bylines look crap.
iOS: edge-swipe support
Indexes: 2-column area right margin too big
Image fade-in
Xcode project generator needs to support appicons (and start screens!)
Articles: Video support
ImageViews are being culled too soon when scrolling
Android: why no timestamps? why are font weights wrong?
Mouseover handling on links
Web browser articles: can we render them without a browser?
Scroll indicators are sometimes buggy (they stop fading)
Articles: Halfwidth picture support
Articles: <italic> and <bold> tags
Articles: Links
Indexes: Photo gallery cells
Indexes: New video carousel for front page
Shadows and gradients
Title modules need correct colour, weight, and the red chevron



Clipping & Visibility
=====================

A Window is a special View that sits at the root of the view tree and handles all display via its owned Surface. Subviews can be added and removed from the view tree, however they only get connected to the Surface and allowed to draw if:

    The view's visibility is set to 'Visible'.
    The view is at least partially within it's parents visible rect (bounds) OR the parent's clipControl is OFF.

Autoclipping means subviews are not attached to the window if theyre not in the viewable area. 

This changes a fundamental property of the view tree... Views can be in a Window's view tree but not attached to the window!


So, a view must updateWindowAttachment() when:
- it is removed/added to a parent View
- its visibility changes to/from Visible
- PARENT's clipControl changes AND it is outside parent's viewable rect
- PARENT viewable rect changes


View.visibility = visible/hidden/gone
View.window = is in a window hierarchy or not
View.visible_in_window = the view's window/surface rect intersects the window. (i.e. not scrolled out of view somewhere).



TextLayout
==========
Inputs: 
 - String (+attributes) 
 - Bounding size
 - Font
 - Font size
 - Line spacing
 - Gravity
 - Text direction

Outputs:
 - Overall bounding box
 - Array of lines
 - Array of glyphs,
 - Array of LOGICAL characters (glyph clusters)



First pass creates an array of glyphs from the input text and font and font size. Nothing is positioned or laid out and the glyphs themselves aren't necessarily rasterized. All we need from the font is glyph size and offset data. This pass must run every time the text or font or font size changes.

Label measurement. Measuring and laying out has to be split, it cannot be done in a single function. Measurement is done 
with respect to a constraining size that will often have nothing to do with positioning. It is likely to be the Label's
parent view width most of the time. If the label is wrap_content then the layout rect is its own content width, not
whatever the measurement constraint was.



Modules & Cells.
Cells have been promoted to Views, since Oaknut Views are very lightweight. This greatly simplifies the app design. 
But what about modules?


Text attributes
---------------
For ease of iteration in TextLayout we need two stores of attribute references, one for starts and one for ends. Since attributes can nest, the order attributes start is not the same as the order attributes end (duh!).

8. Candidate function not viable: no known conversion from 
'const std::__1::__list_iterator<oak::attributed_string::attribute_usage, void *>' 
to 
'const oak::attributed_string::attribute_usage' for 1st argument






RoundRect ShaderToy
===================

const float halfStroke = 5.0 / 2.0;
const vec4 YELLOW = vec4(1.0, 1.0, 0.0, 1.0);
const vec4 RED = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 BLUE = vec4(0.0, 0.0, 1.0, 1.0);
const float r = 10.0 - halfStroke;



void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // B is the inner bounds, i.e. excluding border
    vec2 b = vec2(20,20);
    
    // P is the XY distance from fragCoord to the center of the rect
	vec2 p = (fragCoord - vec2(512,288) / 2.0);

    // RoundRect SDF: inside the shape is -ve, 0 is the edge
    vec2 d = abs(p) - b + vec2(r);
	float fDist = min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
    
    // NB: the SDF defines the inner area, i.e. fDist=0 corresponds
    // to the outer edge of the RED shape.
    
    
    // There are two possible interpolations: from border 
    // to background or from border to fill. 
    vec4 v4FromColor = YELLOW; 
    vec4 v4ToColor = (fDist < 0.0) ? RED : BLUE;
    
    // I don't understand this!
    fDist = abs(fDist) - halfStroke;
    
    float fBlendAmount = smoothstep(-1.0, 1.0, fDist);    
    fragColor = mix(v4FromColor, v4ToColor, fBlendAmount);

}






Interesting GUI kits
====================
https://github.com/hecrj/iced
https://github.com/xi-editor/druid
https://api.flutter.dev/flutter/rendering/BoxConstraints-class.html
http://madebyevan.com/shaders/fast-rounded-rectangle-shadows/
https://raphlinus.github.io/graphics/2020/04/21/blurred-rounded-rects.html

Interesting articles about platform diffs
=========================================
https://medium.com/@Alconost/ios-vs-android-design-630340a73ee6


Problem:
BNURLRequest lifetime is fucked. If one is rapidly cancelled, it is destroyed while the contained URLRequest's callback is extant... and when it runs it refers to a dead object.
Ideally we want the URLRequest to keep BNURLRequest alive

URLRequest
==========
Currently modelled so platform layer is a single synchronous method that gets the data from the remote endpoint. BUT, when this method is blocked and waiting for data, it needs to also check the cancel semaphore! At present no platform layer does this bit - bad!


Oaknut Task doc notes
=====================
No direct support for threads, what we have instead are Tasks. Tasks contain a single callable function which takes a `variant&` as input and returns a `variant` as output. A Task executes in one of three contexts:

 - Main thread
 - Background
 - IO

The main API is `Task::enqueue()` which allows you to submit a chain of one or more tasks that will complete serially, each task passing it's output to the next task as input. For example a single call to Task::enqueue() could run (1) an IO Task that loads a file from disk, which is followed by (2) a Background task that processes the contents, and finally (3) a Main thread task updates UI based on the results.

Behind the scenes Oaknut maintains two thread pools: one for Background tasks, and one for IO tasks. The Background pool is fixed to max(1, numCores-1) threads and the Tasks on these threads *should not block* (this will hopefully some day be enforced in a similar fashion to Android's Strict mode). The intention is to keep all the non-primary cores performing useful background work rather than blocking. The IO pool has no fixed size - it grows and shrinks as required - and it is expected that IO threads will spend almost all of their time in a suspended state, waiting for the OS to complete IO, rather than competing with Background threads for CPU time.

Tasks are cancellable (see `Task::cancel()`) so long-running Background tasks should periodically check `isCancelled()` and early exit if appropriate.






Things like LocalStorage need atomic write, or as close as can be got to. LocalStorage consists of two files: a small index file that's loaded into RAM as long as the index is needed, and a larger file which is random access. Record changes must be transactional:
1. Write the change to an append-only temporary file
2. Make the change to the database file
3. Make the change to the index
4. Delete the temp file.

The POSIX rename syscall performs an atomic replace if a file with the target name already exists 




Smart pointers
==============
- I hate std::shared_ptr cos the refs are stored IN THE POINTER, not the object, so it is trivially easy to create leaks by inadvertently sharing a raw pointer.
- Presume objects are most often not shared between threads. Assert thread ownership in every retain/release in debug builds.

The Object base class implements elementary reference counting via retain()/release() methods. However, in the interests of performance and on the assumption that the vast majority of objects will not be accessed by multiple threads, these methods are not atomic and can only be called by one thread, hereafter termed the 'owning thread'. An Object's owning thread is the first thread to call retain() and in DEBUG builds subsequent calls to retain() and release() will assert that it is the owning thread calling them. How then do we safely share Objects between two or more threads if the refcount isn't atomic? The answer is that non-owning threads must use a SharedObject, which is a sort of thread-local proxy refcounted object. When the SharedObject's refcount drops to zero on the worker thread, the owning thread is dispatched with a call that release()s the real refcount on the owner thread.



  1. The pointer to the (retained) object.
  2. A pointer to the owning PoolThread, or nullptr if the object is owned by the main thread.

Call Mrs Patel, 01582 547786





Safe areas
==========
The OS indicates that window edges should not be used for touch via "safe insets". Views that need to respect these insets can use the special constant


Touch
=====
Mouse and touch events from the OS are received by the Window. The Window dispatches the events to Views in the following way:
1. Hit-test subviews to find the first visible, enabled view that contains the point. When the remotest subview has been found, the event is sent to handleInputEvent() which returns a boolean indicating whether the view handled that event or not. If the view did not handle the event it is passed to the parent view, and so on up the tree.
2. Whichever view handles an initial event (DOWN or DRAG_START), that view becomes the first receiver of handleInputEvent() for subsequent MOVE/DRAG/UP events for the same gesture.

Nice alignment declarations:

align: left,top
left: 10dp
right: 10dp

tabBar: {
  top: parent.bottom - 60dp
  bottom: parent.bottomUnsafe
}


"alignX" really means "left". There is no "right" but maybe there should be? And "left", "right", "top", "bottom" as values only work in "align" decl.

4x LAYOUTSPECS! To replace 2x MEASURESPEC and 2x ALIGNSPEC.

left: default is parent.left + 0dp.
right: default is self.left + self.contentSize. If specified in a style, default ref is parent.right.

A layoutspec describes a position on one axis. It is always relative to a reference position:

- a view's edge (left or right, or top or bottom) OR this view's own opposite layoutspec (which cannot be opposite).
- a multiplier for the reference view edge, if present
- a multiplier for

enum RefType {
  View,
  Opposite,
  Aspect,
}
struct LAYOUTSPEC {
  View* view;
  float multRef;
  float multContentSize;
  float constant;
};


 

Layout issue
------------
What should setNeedsLayout() actually do? It currently sets the view's _layoutValid false, and applies that up the view tree to the root, and then requests a redraw from the Window. 
LinearLayout shows a deficiency in this API. By necessity it lays out it's child views *twice*: once to find the minimum width (or height) of them all stacked together, and then it calls subview.layout() on those that must increase size to fill up available space.

Ideally there would be no such API as setNeedsLayout(). Layout would occur as a natural result of using setLayoutSize(), setLayoutPos(), and invalidateContentSize(). But it should also be possible to override the default layout algorithm and call setRect() directly

What if setNeedsLayout() added the view to a linked list instead? A linked list that is kept sorted by display order. Then window.layout() wouldn't have to visit every view in the tree, only those explicitly marked as needing layout.
And what about layout dependencies? When View A's size is derived from View B's it would be super-nice if re-laying out View B would *automatically* trigger a re-layout of View A without invalidating the entire view tree.
What if Window kept a list of dependencies? So when a view's layout property changes, it looks in view->window->layoutDeps to see which views refer to it and calls layout() on those. Using parent's existing rect as the c




iOS ViewPager notes
-------------------
When scrolling is possible on both axes, the first axis that breaks the "slop" distance wins, and scrolling is then *only* done on that axis for the rest of the touch. This has to be directionalLockEnabled.

In a viewpager, the child page views are usually not scrollable horizontally. We want horizontal scrolling to be handled by the view pager, and vertical scrolling by the page views. If a child page was scrollable horizontally then obviously it gets first choice at the scroll events, and once it's reached the end it can let the parent handle it. Once a scrollable view has decided it is *not* handling scrolling on a particular axis, it should reject those scroll events that it doesn't handle to its parent.

Reminder that Window receives InputEvents from the OS, they arrivate in Window::dispatchInputEvent() - remembering that a Window is a kind of View that sits at the root of the view tree - and dispatchInputEvent() hit-tests its children to find the most distant leaf view that contains the touch location.
It then calls View::handleInputEvent() on that leaf view, the return value of which is either the View that handled the event, or NULL. If that view returns NULL then the event is offered to the parent View, and so on up the tree.

Note that NO DRAGGING OCCURS UNTIL SLOP DISTANCE IS EXCEEDED. V important detail.

Note that we want horz scrolling to be handled by the ViewPager and vertical scrolling by the ViewPager children. The viewpager has directionalLock enabled, so what this flag needs to do is split the move event into two move events and dispatch them separately. If one event results in dragging, events to the other axis are discounted.


Size syntax
===========
A view's width or height is a reference size multiplied by a multiplier, plus another constant. The reference size may be one of:

 - Another views' size (most often the parent view).
 - The view's own content size
 - The view's own size along the other axis (ie aspect ratio)

In text form:

[[reference ID.]multiplier][[+-]constant]

A few ID values are reserved: 

 - "wrap" means view's own content size should be the reference size
 - "aspect" means view's other dimension is the reference size.
 - "fill"  means parent size is the reference size and multiplier is 100% (unless also specified).
 
Position Syntax
================
A view's top or left position is calculated from three components that are added together:

 1. Reference view size multiplier
 2. View's own content size multiplier
 3. A constant.

In text form any combination of these 3 components are allowed but they must follow the above order. Some examples:

4dp                Parent left edge plus 4dp
right              Parent right edge
right + 8dp	   Right edge will be 8dp left of the parent right edge
100%-100% 	   Another way to say "right", ie 100% of parent size minus 100% of own size
label1.right	   Right edge will align to label1's right edge
label2.left + 8dp  

If the own content size multiplier is omitted it will default to zero EXCEPT if one of the position words are used, 


The reference view is, by default, the parent view. 
 
label.bottom+4dp
ref, refmul, ownmul, constant
"below" = ref*1.0
"above" = ref*0.0 -self*1.0
"top" = 0
"bottom" = ref*1.0 -self*1.0
"center" = ref*0.5 - self*0.5
[ref.]top|bottom|center|above|below|<float|percent> [[+|-]float|percent] [[+|-]float|percent]
[ref.]left|right|center|toLeftOf|toRightOf|<float|percent>]
ref.above

ref.50%-50%-20dp

100%+50%+30dp
below(ref)+20dp
ref.50%-50%+20dp

Image resizing
==============
Images tend to be displayed at a different size to their resolution. Keeping large bitmaps around unnecessarily is a waste of memory and hurts performance as drawing reads more memory and has to bilinear filter. It'd be desirable if we could automate some of the pain away. Maybe ImageView could have a
'resizeImage' property whose values are:
    'off' : no resizing
    'on': image is automatically resized, original image is discarded.
NB: We also need a Bitmap resizing API!
...

Small image texture cache
=========================
It'd be good if small images could self-organize into larger textures. If an image is, say, less than half the screen width, then 

How to organize texture cache? 
Screen size is relevant

Android: "Glyphs are packed in the textures in columns. Each texture has list of currently allocated columns plus all the available empty space. If a glyph fits in an existing column, it is added at the bottom of the occupied space in said column. If all columns are occupied, a new column is carved out of the left side of the remaining space. The width of each glyph is rounded to a multiple of 4 pixels. This is a good compromise between columns reuse and texture packing. The packing is not optimum, but it offers as fast implementation." All glyphs are stored in the textures with an empty border of 1 pixel around them. This is necessary to avoid artifacts when the font textures are sampled with bilinear filtering.

1024x512 alpha cache	= 512KB
2048x256 alpha cache	= 512KB
2048x256 alpha cache	= 512KB
2048x512 alpha cache	= 1MB
1024x512 RGBA cache	= 2MB
2048x256 RGBA cache	= 8MB


