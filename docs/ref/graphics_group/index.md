---
layout: default
---

# Graphics

All rendering is done by RenderOps to a Surface.

By default Oaknut graphics coordinates are pixel-based with the top-left corner being the origin.The Surface keeps a list of all the RenderOps it has to draw and groups related ops in RenderBatches. A RenderBatch containing many RenderOps can be drawn in a single draw call and is much faster than if each op were drawn separately.At present the graphics module is tightly bound to OpenGL. In time it is hoped that the low-level graphics API will be abstracted away and will use Metal and DirectX on iOS/macOS and Windows respectively. This is now slightly more urgent now that Apple have officially deprecated OpenGL in June 2018...

## Classes

`class ` [Bitmap](todo) | 
The standard 2D array of color values.     
`class ` [Canvas](todo) | 
Basic 2D Canvas API.     
`class ` [COLOR](todo) | 
Basic encapsulation of a 32-bit color RGBA value.     
`class ` [Surface](todo) | 
Rendering target, currently backed by a GL texture.     
