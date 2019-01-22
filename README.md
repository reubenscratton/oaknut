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
- Linux: fix keyboard input processing (backspace doesnt work)
- Linux: fix text measurement. Somethings way off somewhere.
- Linux: fix VSCode generator
- Windows bindings & DX renderer
- Add CodeBlocks project generator
- Get reference docs to acceptable state
- CLion: what can we do about the broken 'clean' command? No way to clean custom targets
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
