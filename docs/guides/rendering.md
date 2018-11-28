---
layout: default
---
# Rendering

The basic unit of rendering is the `RenderOp`, a rectangle that is
drawn with a shader of some kind. Each `View` draws itself
using a list of RenderOps maintained via `View::addRenderOp()` and
`View::removeRenderOp()`.

At present all shaders are OpenGL but plans are afoot to implement
a Metal renderer - necessary since Apple recently deprecated OpenGL -
and DirectX.

RenderOps are rendered to a `Surface`. The application `Window`
owns the principal Surface but any View can also have a private
surface for rendering offscreen, useful for Views where rendering
might be particularly expensive.

When a View is attached to a surface (i.e. it is part of the visible
hierarchy) it's RenderOps are sorted into a collection of `RenderBatch`es
that group related RenderOps together. Every op in the same
batch can be rendered with a single GPU command, a significant optimisation
over drawing each op with a different command.


## When rendering occurs

Rendering is always done on the main thread. Application code signals
the need for a redraw by View::setNeedsFullRedraw() or View::invalidateRect().
This in turn asks the `Window` to which the View is attached to schedule a redraw
at the next opportunity, the details of which are OS-specific. The OS calls
Window::draw() which in turn invokes Surface::render() to draw itself - the Window
itself being the root of the view hierarchy - to the window's principal surface.
Surface::render() then recurses through the view tree to draw each view's
render list at the right position.

In addition to drawing the whole app UI, Window::draw() performs two other
important tasks: it updates all running Animations, and reclaims memory
used by any deleted Objects.


## Two stages of rendering

Surface::render() performs rendering in two stages. The first stage is to ensure
that all the RenderOps are up to date, calling View::updateRenderOps() for those
that need it. This is the last chance for Views to ensure their render ops
have the correct rects and rendering parameters. Once all the ops are known to
be up to date they are 'validated', meaning they select the shader program
they need to satisfy their render parameters. At the same time each render list
is assigned a 'renderOrder' number that is used in the second stage to ensure
that overlapping renders run in the right order.

The second stage is the render itself, i.e. issuing the GPU draw commands. This
involves a second traversal of the view tree where, for each RenderOp, the
RenderBatch it belongs to is examined and as much of that batch as possible is
drawn at once, i.e. without intersecting any ops in other batches that may need
to render first.


## Some OpenGL notes

The Window object owns the vertex and index buffers (see the `QuadBuffer` type).
Almost all shaders use a generic VERTEX structure containing 8 floats:

    struct VERTEX {
        GLfloat x,y;
        GLfloat s,t;
        uint32_t color;
        GLfloat unused[3];
    }
