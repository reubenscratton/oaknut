/** @defgroup graphics_group Graphics
 *
 * All rendering is done by RenderOps to a Surface. By default Oaknut
 * graphics coordinates are pixel-based with the top-left corner being
 * the origin.
 *
 * The Surface keeps a list of all the RenderOps it has to draw and
 * groups related ops in RenderBatches. A RenderBatch containing
 * many RenderOps can be drawn in a single draw call and is much faster
 * than if each op were drawn separately.
 *
 * At present the graphics module is tightly bound to OpenGL. In time
 * it is hoped that the low-level graphics API will be abstracted away
 * and will use Metal and DirectX on iOS/macOS and Windows respectively.
 * This is now slightly more urgent now that Apple have officially
 * deprecated OpenGL in June 2018...
 *
 */
