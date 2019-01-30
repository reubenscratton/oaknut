# Oaknut
Oaknut is an experimental C++ GUI app platform. Write once and
deploy to any major OS, or to web.

## Getting Started
Once you've cloned the Oaknut repo, set the environment
variable **OAKNUT_DIR** to point to it.

Then open one of the samples in the IDE of your choice.
Build and run should Just Work but command-line building is
also supported, just run `make <platform>` in the project
directory where `<platform>` is one of the supported platforms,
i.e. `macos`, `web`, `ios`, `android`, `linux`, or `windows`.

Bitmaps and Textures
--------------------
A Bitmap is the main image class and it has two main properties:

- A wrapped platform-specific bitmap object. You do not use this directly
  but use the generic APIs to manipulate it.
- An associated Texture, for when the bitmap is used for rendering.

At least one of these two properties must be valid: a bitmap can
be "texture-only" in which case there is no wrapped platform object,
or it can be "texture-less" in which case there is no texture needed,
or it can have both platform and texture. In the latter case
Bitmap::texInvalidate() can be used to tell the renderer to upload
the platform-side pixels to graphics memory.

For application code to access a Bitmap's pixels it must use the lock()
and unlock() APIs. If the Bitmap wraps a platform bitmap then locking
and unlocking should have near zero runtime cost, but if it's texture-only
this will trigger a framebuffer read, followed by a framebuffer write if the app
modified the pixel data. Use with caution.

NB: iOS devices have a unified memory model where CPU and GPU
share the same RAM and no texture upload is ever necessary. The
iOS SDK exposes this via CoreVideo APIs and the iOS
implementation of Bitmap handles this completely transparently.


Immediate Todos
---------------
- Android: emBeeb status bar is wrong colour.
- Cameras and facedetectors on all platforms need a catchup
- emBeeb Web: disks VC is very scrolly?
- Mac/Metal: blur effect is upside down
- Linux: fix VSCode generator
- Linux: fix keyboard input processing (backspace doesnt work)
- Linux: fix text measurement. Somethings way off somewhere.
- Linux: implement settings
- Linux: urlrequest POST support
- Linux: fix textedit    
- Windows bindings & DX renderer
- Add CodeBlocks project generator
- Clion 'Clean' command doesn't work. CANT FIX, MUST DOCUMENT
- Clion: can we remove the fake non-buildable target?
- Pressing back when modal actionsheet visible doesn't cancel the actionsheet
- Bug: if drag starts in a text field, no drag happens...
- Get reference docs to acceptable state
- ImageView: Maybe move or remove BitmapProvider, am not sure renderops should be concerned with being notified. It's a view-level responsibility to obtain the Bitmap to be shown, *then* create the renderop for it.

Medium-term Roadmap
-------------------
- Sample : Todo list
- New Project template, i.e. you can run 'oaknut/build/createproj ~/helloworld'
- Build a proper UI widget library
- Extend Canvas as far as possible

Long-term Roadmap
-----------------
- SVG support
- Rust port!
- Web IDE


CC++ (Concise C++)
==================
A sort of a simplified C++ 'dialect' for Oaknut.  

Level 1 : Die, header files, die
--------------------------------
- Primary goal is to process a CC++ source file (.ccpp) into a .cpp file and a .h file.
- Configure IDEs for correct syntax colouring.
- Make/build integration
- Can we post-process symbol files to point debuggers into the .ccpp? To do so we'd need a line-number map between .cpp and .ccpp.

Level 2 : Module support
------------------------
- Add a 'module' keyword to parser. Only the first statement in a .ccpp can be the module declaration.
- Add a 'import' keyword to reference other modules. "import foo;" becomes "#include <foo.h>"
- Top-level declarations marked with 'public' keyword end up in module public header.

Level 3 : Syntax sugar
----------------------
- 'property' keyword with optional 'readonly', plus 'get' and 'set' blocks a la Swift.
- 'interface' keyword for declaring pure abstract classes.
- Only public inheritance allowed and then from a single non-pure base type.

Level 4 : Clang support
-----------------------
- Hack Clang to directly compile CC++, no .cpp generation.
