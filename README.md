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


Immediate Todos
---------------
- Web: text rendering broken after text change
- emBeeb: whats happened to 'Best' list?
- Linux: fix keyboard input processing (backspace doesnt work)
- Linux: fix text measurement. Somethings way off somewhere.
- Linux: fix VSCode generator
- Linux: implement settings
- Linux: urlrequest POST support
- Linux: fix textedit    
- Windows bindings & DX renderer
- Add CodeBlocks project generator
- Clion 'Clean' command doesn't work. CANT FIX, MUST DOCUMENT
- Clion: can we remove the fake non-buildable target?
- emBeeb: search box should capitalise first letter of words
- Android catchup - test face detection worker etc all function in android native and mobile chrome.
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

Level 1
-------
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


