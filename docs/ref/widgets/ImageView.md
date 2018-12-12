---
layout: default
---

# ImageView

```
class ImageView
    : public View
```


Displays a bitmap.     

    
` ImageView()`<br>

`void setImageUrl(const `[`string`](/ref/base_group/string)` & url)`<br>

`void setImageAsset(const `[`string`](/ref/base_group/string)` & assetPath)`<br>

`void setBitmap(`[`Bitmap`](/ref/graphics_group/Bitmap)`* bitmap)`<br>

`void setImageNode(AtlasNode* node)`<br>

`ContentMode getContentMode()`<br>

`void setContentMode(ContentMode contentMode)`<br>

`RECT getImageRect()`<br>

`void attachToWindow(`[`Window`](/ref/views/Window)`* window)`<br>Called when the view is attached to the application Window and will be drawn and able to receive input events, etc.

`void detachFromWindow()`<br>Called when the view is detached from the application Window.

`void onEffectiveTintColorChanged()`<br>

`void layout(RECT constraint)`<br>Recalculate and apply this view's size and position (as well as all subviews).
Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour.

`bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)`* value)`<br>Applies a single style value for the given attribute name.
Custom views should override this method to add support for custom attributes.

`void updateContentSize(SIZE constrainingSize)`<br>Called during measure() views should set the _contentSize property here.
The defaut implementation does nothing.

`void updateRenderOps()`<br>


`void loadImage()`<br>

`void cancelLoad()`<br>


