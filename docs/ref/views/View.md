---
layout: default
---

# View

```
class View
    : public Styleable
```


Fundamental UI building block.     

    
### Identity & Hierarchy

`void addSubview(`[`View`](/ref/views/View)`* subview)`<br>Adds a subview to the end of the subviews list, i.e.
last in the z-order

`void insertSubview(`[`View`](/ref/views/View)`* subview, int index)`<br>Inserts a subview at a particular index, z-order ascending.

`int getSubviewCount()`<br>Get the number of subviews.

[`View`](/ref/views/View)`* getSubview(int index)`<br>Get a subview at a particular index, z-order ascending.
Returns NULL if no such index.

`void removeSubview(`[`View`](/ref/views/View)`* subview)`<br>Removes a subview.

[`View`](/ref/views/View)`* getParent()`<br>Return the parent view of this view.

`void removeFromParent()`<br>Removes this view from its parent view.

`int indexOfSubview(`[`View`](/ref/views/View)`* subview)`<br>Find the index of the given subview, returns -1 if given view is not a subview.

[`View`](/ref/views/View)`* findViewById(const `[`string`](/ref/base_group/string)` & id)`<br>Find a view among the subviews (and all descendent views) given a string id.

[`View`](/ref/views/View)`* getRootView()`<br>Return the view at the root of the view tree.

`void removeSubviewsNotInVisibleArea()`<br>Helper to remove any subviews currently scrolled out of sight.


### Window & Surface

`void setUsePrivateSurface(bool usePrivateSurface)`<br>Controls whether the view should render to a private surface or not.
A private surface is useful when rendering is particularly expensive and likely to contribute to lag.

[`Window`](/ref/views/Window)`* getWindow()`<br>Return the Window that this View is attached to.
In practice this is the main application window or NULL.

`void attachToWindow(`[`Window`](/ref/views/Window)`* window)`<br>Called when the view is attached to the application Window and will be drawn and able to receive input events, etc.

`void detachFromWindow()`<br>Called when the view is detached from the application Window.


### Size & Position

`float getWidth()`<br>Get the width of the view.

`float getHeight()`<br>Get the height of the view.

`float getLeft()`<br>Get the X-coordinate of the left edge, in parent coords.

`float getRight()`<br>Get the X-coordinate of the right edge, in parent coords.

`float getTop()`<br>Get the Y-coordinate of the top edge, in parent coords.

`float getBottom()`<br>Get the Y-coordinate of the bottom edge, in parent coords.

`RECT getRect()`<br>Get the current view rect, in parent coords.

`RECT getOwnRect()`<br>Get the view rect in it's own coordinate system, i.e.
(0,0,_rect.size.width,_rect.size.height)

`RECT getOwnRectPadded()`<br>Same as getOwnRect() but with current padding insets applied.

`POINT mapPointToWindow(POINT pt)`<br>Convert a point from own-rect coordinates to window coordinates.

`void setPadding(EDGEINSETS padding)`<br>Set padding insets, i.e.
the space between the view rect and its content

`void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)`<br>Set the size this view would like to have, given various constraints (see MEASURESPEC and measure()).

`void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)`<br>Set the preferred alignment within the parent view.
See ALIGNSPEC and layout()

`void setNeedsLayout()`<br>Signal that the view needs to recalculate its size and position.

`void layout(RECT constraint)`<br>Recalculate and apply this view's size and position (as well as all subviews).
Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour.

`void setRectSize(const SIZE & size)`<br>Sets the size of the view rect.
CAUTION! This API is dumb and will probably be removed.

`void setRectOrigin(const POINT & origin)`<br>Sets the position of the view.
CAUTION! This API exists so Views may override the positioning of subviews during layout. Ideally this would not be a `public` API.

`void setVisibility(Visibility visibility)`<br>


### Content & Scrolling

`SIZE getContentSize()`<br>Returns the current content size, as calculated by updateContentSize() during layout.
The content size is not directly settable, each view should determine it's own.

`void invalidateContentSize()`<br>Mark the view's contentSize as invalid and call setNeedsLayout()

`void updateContentSize(SIZE constrainingSize)`<br>Called during measure() views should set the _contentSize property here.
The defaut implementation does nothing.

`void setGravity(GRAVITY gravity)`<br>Set the gravity flags.
Gravity affects how the content is aligned within the view rect when the view rect size exceeds the content size.

`POINT getContentOffset()`<br>

`void setContentOffset(POINT contentOffset)`<br>

`void setScrollInsets(EDGEINSETS scrollInsets)`<br>

`bool canScrollHorizontally()`<br>

`bool canScrollVertically()`<br>

`float getMaxScrollY()`<br>

`bool getClipsContent()`<br>

`void setClipsContent(bool clipsContent)`<br>

`void scrollBy(POINT scrollAmount)`<br>


### Drawing

`void setBackground(RenderOp* renderOp)`<br>

`RenderOp* getBackgroundOp()`<br>

`void setBackgroundColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)`<br>

`void addRenderOp(RenderOp* renderOp)`<br>

`void addRenderOp(RenderOp* renderOp, bool atFront)`<br>

`void removeRenderOp(RenderOp* renderOp)`<br>

`void setNeedsFullRedraw()`<br>

`void invalidateRect(const RECT & rect)`<br>

[`COLOR`](/ref/graphics_group/COLOR)` getTintColor()`<br>

`void setTintColor(`[`COLOR`](/ref/graphics_group/COLOR)` tintColor)`<br>Sets tint color.

`float getAlpha()`<br>Get the current alpha.
Defaults to 1.0. Note that the actual alpha value used in rendering inherits (i.e. is effectively multiplied by) all ancestor alpha values.

`void setAlpha(float alpha)`<br>Sets alpha.


### State

`void setState(STATE mask, STATE value)`<br>Set one or more state bits at once.
The STATESET parameter has a 'mask' member which determines the state bits that get updated.

`bool isPressed()`<br>

`void setPressed(bool isPressed)`<br>

`bool isEnabled()`<br>

`void setEnabled(bool enabled)`<br>

`bool isChecked()`<br>

`void setChecked(bool checked)`<br>

`bool isSelected()`<br>

`void setSelected(bool checked)`<br>

`bool isFocused()`<br>

`void setFocused(bool focused)`<br>

`bool isErrored()`<br>

`void setErrored(bool errored)`<br>

`void onStateChanged(STATESET changes)`<br>Called whenever state changes.


### Styles

`bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)`* value)`<br>Applies a single style value for the given attribute name.
Custom views should override this method to add support for custom attributes.


### Touch

[`View`](/ref/views/View)`* hitTest(POINT & pt)`<br>

[`View`](/ref/views/View)`* subviewContainingPoint(POINT pt)`<br>

`int indexOfSubviewContainingPoint(POINT pt)`<br>


` View()`<br>

` ~View()`<br>

`IKeyboardInputHandler* getKeyboardInputHandler()`<br>

`ITextInputReceiver* getTextInputReceiver()`<br>

`bool requestFocus()`<br>

`void animateAlpha(float target, float duration)`<br>

`void animateTranslate(POINT translation, float duration)`<br>

`Animation* animateInFromBottom(float duration, InterpolateFunc interpolater)`<br>

`Animation* animateOutToBottom(float duration, InterpolateFunc interpolater)`<br>

`void setTranslate(POINT translation)`<br>


`void inflate(const `[`string`](/ref/base_group/string)` & layoutFile)`<br>

`void bind(T*& rview, const `[`string`](/ref/base_group/string)` & id)`<br>


