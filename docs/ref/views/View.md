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

`void addSubview(`[`View`](/oaknut/ref/views/View)` * subview)`<br>Adds a subview to the end of the subviews list, i.e.

`void insertSubview(`[`View`](/oaknut/ref/views/View)` * subview, int index)`<br>Inserts a subview at a particular index, z-order ascending.

`int getSubviewCount()`<br>Get the number of subviews.

[`View`](/oaknut/ref/views/View)` * getSubview(int index)`<br>Get a subview at a particular index, z-order ascending.

`void removeSubview(`[`View`](/oaknut/ref/views/View)` * subview)`<br>Removes a subview.

[`View`](/oaknut/ref/views/View)` * getParent()`<br>Return the parent view of this view.

`void removeFromParent()`<br>Removes this view from its parent view.

`int indexOfSubview(`[`View`](/oaknut/ref/views/View)` * subview)`<br>Find the index of the given subview, returns -1 if given view is not a subview.

[`View`](/oaknut/ref/views/View)` * findViewById(const `[`string`](/oaknut/ref/base_group/string)` & id)`<br>Find a view among the subviews (and all descendent views) given a string id.

[`View`](/oaknut/ref/views/View)` * getRootView()`<br>Return the view at the root of the view tree.

`void removeSubviewsNotInVisibleArea()`<br>Helper to remove any subviews currently scrolled out of sight.



### Window & Surface

`void setUsePrivateSurface(bool usePrivateSurface)`<br>Controls whether the view should render to a private surface or not.

[`Window`](/oaknut/ref/views/Window)` * getWindow()`<br>Return the Window that this View is attached to.

`void attachToWindow(`[`Window`](/oaknut/ref/views/Window)` * window)`<br>Called when the view is attached to the application Window and will be drawn and able to receive input events, etc.

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

`RECT getOwnRectPadded()`<br>Same as getOwnRect() but with current padding insets applied.

`POINT mapPointToWindow(POINT pt)`<br>Convert a point from own-rect coordinates to window coordinates.

`void setPadding(EDGEINSETS padding)`<br>Set padding insets, i.e.

`void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)`<br>Set the size this view would like to have, given various constraints (see MEASURESPEC and measure()).

`void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)`<br>Set the preferred alignment within the parent view.

`void setNeedsLayout()`<br>Signal that the view needs to recalculate its size and position.

`void layout(RECT constraint)`<br>Recalculate and apply this view's size and position (as well as all subviews).

`void setRectSize(const SIZE & size)`<br>Sets the size of the view rect.

`void setRectOrigin(const POINT & origin)`<br>Sets the position of the view.

`void setVisibility(Visibility visibility)`<br>



### Content & Scrolling

`SIZE getContentSize()`<br>Returns the current content size, as calculated by updateContentSize() during layout.

`void invalidateContentSize()`<br>Mark the view's contentSize as invalid and call setNeedsLayout()

`void updateContentSize(SIZE constrainingSize)`<br>Called during measure() views should set the _contentSize property here.

`void setGravity(GRAVITY gravity)`<br>Set the gravity flags.

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

`void setBackground(RenderOp * renderOp)`<br>

`RenderOp * getBackgroundOp()`<br>

`void setBackgroundColor(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` color)`<br>

`void addRenderOp(RenderOp * renderOp)`<br>

`void addRenderOp(RenderOp * renderOp, bool atFront)`<br>

`void removeRenderOp(RenderOp * renderOp)`<br>

`void setNeedsFullRedraw()`<br>

`void invalidateRect(const RECT & rect)`<br>

[`COLOR`](/oaknut/ref/graphics_group/COLOR)` getTintColor()`<br>

`void setTintColor(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` tintColor)`<br>Sets tint color.

`float getAlpha()`<br>Get the current alpha.

`void setAlpha(float alpha)`<br>Sets alpha.



### State

`void setState(STATE mask, STATE value)`<br>Set one or more state bits at once.

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

`bool applyStyleValue(const `[`string`](/oaknut/ref/base_group/string)` & name, const `[`StyleValue`](/oaknut/ref/app_group/StyleValue)` * value)`<br>Applies a single style value for the given attribute name.



### Touch

[`View`](/oaknut/ref/views/View)` * hitTest(POINT & pt)`<br>

[`View`](/oaknut/ref/views/View)` * subviewContainingPoint(POINT pt)`<br>

`int indexOfSubviewContainingPoint(POINT pt)`<br>



` View()`<br>

` ~View()`<br>

`IKeyboardInputHandler * getKeyboardInputHandler()`<br>

`ITextInputReceiver * getTextInputReceiver()`<br>

`bool requestFocus()`<br>

`void animateAlpha(float target, float duration)`<br>

`void animateTranslate(POINT translation, float duration)`<br>

`Animation * animateInFromBottom(float duration, InterpolateFunc interpolater)`<br>

`Animation * animateOutToBottom(float duration, InterpolateFunc interpolater)`<br>

`void setTranslate(POINT translation)`<br>



`void inflate(const `[`string`](/oaknut/ref/base_group/string)` & layoutFile)`<br>

`void bind(T *& rview, const `[`string`](/oaknut/ref/base_group/string)` & id)`<br>



## Methods

| | |
|-|-|
| *View* | ` View()` |  |
| *addRenderOp* | `void addRenderOp(RenderOp * renderOp)` |  |
| *addRenderOp* | `void addRenderOp(RenderOp * renderOp, bool atFront)` |  |
| *addSubview* | `void addSubview(`[`View`](/oaknut/ref/views/View)` * subview)` | last in the z-order |
| *animateAlpha* | `void animateAlpha(float target, float duration)` |  |
| *animateInFromBottom* | `Animation * animateInFromBottom(float duration, InterpolateFunc interpolater)` |  |
| *animateOutToBottom* | `Animation * animateOutToBottom(float duration, InterpolateFunc interpolater)` |  |
| *animateTranslate* | `void animateTranslate(POINT translation, float duration)` |  |
| *applyStyleValue* | `bool applyStyleValue(const `[`string`](/oaknut/ref/base_group/string)` & name, const `[`StyleValue`](/oaknut/ref/app_group/StyleValue)` * value)` | Custom views should override this method to add support for custom attributes. |
| *attachToWindow* | `void attachToWindow(`[`Window`](/oaknut/ref/views/Window)` * window)` |  |
| *bind* | `void bind(T *& rview, const `[`string`](/oaknut/ref/base_group/string)` & id)` |  |
| *canScrollHorizontally* | `bool canScrollHorizontally()` |  |
| *canScrollVertically* | `bool canScrollVertically()` |  |
| *detachFromWindow* | `void detachFromWindow()` |  |
| *findViewById* | [`View`](/oaknut/ref/views/View)` * findViewById(const `[`string`](/oaknut/ref/base_group/string)` & id)` |  |
| *getAlpha* | `float getAlpha()` | Defaults to 1.0. Note that the actual alpha value used in rendering inherits (i.e. is effectively multiplied by) all ancestor alpha values. |
| *getBackgroundOp* | `RenderOp * getBackgroundOp()` |  |
| *getBottom* | `float getBottom()` |  |
| *getClipsContent* | `bool getClipsContent()` |  |
| *getContentOffset* | `POINT getContentOffset()` |  |
| *getContentSize* | `SIZE getContentSize()` | The content size is not directly settable, each view should determine it's own. |
| *getHeight* | `float getHeight()` |  |
| *getKeyboardInputHandler* | `IKeyboardInputHandler * getKeyboardInputHandler()` |  |
| *getLeft* | `float getLeft()` |  |
| *getMaxScrollY* | `float getMaxScrollY()` |  |
| *getOwnRect* | `RECT getOwnRect()` | (0,0,_rect.size.width,_rect.size.height) |
| *getOwnRectPadded* | `RECT getOwnRectPadded()` |  |
| *getParent* | [`View`](/oaknut/ref/views/View)` * getParent()` |  |
| *getRect* | `RECT getRect()` |  |
| *getRight* | `float getRight()` |  |
| *getRootView* | [`View`](/oaknut/ref/views/View)` * getRootView()` |  |
| *getSubview* | [`View`](/oaknut/ref/views/View)` * getSubview(int index)` | Returns NULL if no such index. |
| *getSubviewCount* | `int getSubviewCount()` |  |
| *getTextInputReceiver* | `ITextInputReceiver * getTextInputReceiver()` |  |
| *getTintColor* | [`COLOR`](/oaknut/ref/graphics_group/COLOR)` getTintColor()` |  |
| *getTop* | `float getTop()` |  |
| *getWidth* | `float getWidth()` |  |
| *getWindow* | [`Window`](/oaknut/ref/views/Window)` * getWindow()` | In practice this is the main application window or NULL. |
| *hitTest* | [`View`](/oaknut/ref/views/View)` * hitTest(POINT & pt)` |  |
| *indexOfSubview* | `int indexOfSubview(`[`View`](/oaknut/ref/views/View)` * subview)` |  |
| *indexOfSubviewContainingPoint* | `int indexOfSubviewContainingPoint(POINT pt)` |  |
| *inflate* | `void inflate(const `[`string`](/oaknut/ref/base_group/string)` & layoutFile)` |  |
| *insertSubview* | `void insertSubview(`[`View`](/oaknut/ref/views/View)` * subview, int index)` |  |
| *invalidateContentSize* | `void invalidateContentSize()` |  |
| *invalidateRect* | `void invalidateRect(const RECT & rect)` |  |
| *isChecked* | `bool isChecked()` |  |
| *isEnabled* | `bool isEnabled()` |  |
| *isErrored* | `bool isErrored()` |  |
| *isFocused* | `bool isFocused()` |  |
| *isPressed* | `bool isPressed()` |  |
| *isSelected* | `bool isSelected()` |  |
| *layout* | `void layout(RECT constraint)` | Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour. |
| *mapPointToWindow* | `POINT mapPointToWindow(POINT pt)` |  |
| *onStateChanged* | `void onStateChanged(STATESET changes)` |  |
| *removeFromParent* | `void removeFromParent()` |  |
| *removeRenderOp* | `void removeRenderOp(RenderOp * renderOp)` |  |
| *removeSubview* | `void removeSubview(`[`View`](/oaknut/ref/views/View)` * subview)` |  |
| *removeSubviewsNotInVisibleArea* | `void removeSubviewsNotInVisibleArea()` |  |
| *requestFocus* | `bool requestFocus()` |  |
| *scrollBy* | `void scrollBy(POINT scrollAmount)` |  |
| *setAlignSpecs* | `void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)` | See ALIGNSPEC and layout() |
| *setAlpha* | `void setAlpha(float alpha)` |  |
| *setBackground* | `void setBackground(RenderOp * renderOp)` |  |
| *setBackgroundColor* | `void setBackgroundColor(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` color)` |  |
| *setChecked* | `void setChecked(bool checked)` |  |
| *setClipsContent* | `void setClipsContent(bool clipsContent)` |  |
| *setContentOffset* | `void setContentOffset(POINT contentOffset)` |  |
| *setEnabled* | `void setEnabled(bool enabled)` |  |
| *setErrored* | `void setErrored(bool errored)` |  |
| *setFocused* | `void setFocused(bool focused)` |  |
| *setGravity* | `void setGravity(GRAVITY gravity)` | Gravity affects how the content is aligned within the view rect when the view rect size exceeds the content size. |
| *setMeasureSpecs* | `void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)` |  |
| *setNeedsFullRedraw* | `void setNeedsFullRedraw()` |  |
| *setNeedsLayout* | `void setNeedsLayout()` |  |
| *setPadding* | `void setPadding(EDGEINSETS padding)` | the space between the view rect and its content |
| *setPressed* | `void setPressed(bool isPressed)` |  |
| *setRectOrigin* | `void setRectOrigin(const POINT & origin)` | CAUTION! This API exists so Views may override the positioning of subviews during layout. Ideally this would not be a `public` API. |
| *setRectSize* | `void setRectSize(const SIZE & size)` | CAUTION! This API is dumb and will probably be removed. |
| *setScrollInsets* | `void setScrollInsets(EDGEINSETS scrollInsets)` |  |
| *setSelected* | `void setSelected(bool checked)` |  |
| *setState* | `void setState(STATE mask, STATE value)` | The STATESET parameter has a 'mask' member which determines the state bits that get updated. |
| *setTintColor* | `void setTintColor(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` tintColor)` |  |
| *setTranslate* | `void setTranslate(POINT translation)` |  |
| *setUsePrivateSurface* | `void setUsePrivateSurface(bool usePrivateSurface)` | A private surface is useful when rendering is particularly expensive and likely to contribute to lag. |
| *setVisibility* | `void setVisibility(Visibility visibility)` |  |
| *subviewContainingPoint* | [`View`](/oaknut/ref/views/View)` * subviewContainingPoint(POINT pt)` |  |
| *updateContentSize* | `void updateContentSize(SIZE constrainingSize)` | The defaut implementation does nothing. |
| *~View* | ` ~View()` |  |
