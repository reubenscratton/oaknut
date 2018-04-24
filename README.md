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
- Linux urlrequest
- Initial non-ref documentation
- Clean target
- Sample : Hello world
- Sample : Todo list
- Project template
- Fix imageview bug I just added. Maybe move or remove the 
BitmapProvider thing, I don't think renderops should support 
being notified like that. It's a view-level responsibility
to obtain the Bitmap to be shown, then create the renderop
for it.
- Standardize on one word for image/bitmap/texture. "Image" is
currently my preferred option.

Medium-term Roadmap
-------------------
- Initial 0.1 release to interested devs
- Windows bindings
- Build a proper UI widget library
- Extend Canvas as far as possible

Long-term Roadmap
-----------------
- SVG support
- Rust port!
- Web IDE