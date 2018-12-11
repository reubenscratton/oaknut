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

| | |
|-|-|
|`void addSubview(`[`View`](/ref/views/View)` * subview)`|Adds a subview to the end of the subviews list, i.e.|
|`void insertSubview(`[`View`](/ref/views/View)` * subview, int index)`|Inserts a subview at a particular index, z-order ascending.|
|`int getSubviewCount()`|Get the number of subviews.|
|[`View`](/ref/views/View)` * getSubview(int index)`|Get a subview at a particular index, z-order ascending.|
|`void removeSubview(`[`View`](/ref/views/View)` * subview)`|Removes a subview.|
|[`View`](/ref/views/View)` * getParent()`|Return the parent view of this view.|
|`void removeFromParent()`|Removes this view from its parent view.|
|`int indexOfSubview(`[`View`](/ref/views/View)` * subview)`|Find the index of the given subview, returns -1 if given view is not a subview.|
|[`View`](/ref/views/View)` * findViewById(const `[`string`](/ref/base_group/string)` & id)`|Find a view among the subviews (and all descendent views) given a string id.|
|[`View`](/ref/views/View)` * getRootView()`|Return the view at the root of the view tree.|
|`void removeSubviewsNotInVisibleArea()`|Helper to remove any subviews currently scrolled out of sight.|


### Window & Surface

| | |
|-|-|
|`void setUsePrivateSurface(bool usePrivateSurface)`|Controls whether the view should render to a private surface or not.|
|[`Window`](/ref/views/Window)` * getWindow()`|Return the Window that this View is attached to.|
|`void attachToWindow(`[`Window`](/ref/views/Window)` * window)`|Called when the view is attached to the application Window and will be drawn and able to receive input events, etc.|
|`void detachFromWindow()`|Called when the view is detached from the application Window.|


### Size & Position

| | |
|-|-|
|`float getWidth()`|Get the width of the view.|
|`float getHeight()`|Get the height of the view.|
|`float getLeft()`|Get the X-coordinate of the left edge, in parent coords.|
|`float getRight()`|Get the X-coordinate of the right edge, in parent coords.|
|`float getTop()`|Get the Y-coordinate of the top edge, in parent coords.|
|`float getBottom()`|Get the Y-coordinate of the bottom edge, in parent coords.|
|`RECT getRect()`|Get the current view rect, in parent coords.|
|`RECT getOwnRect()`|Get the view rect in it's own coordinate system, i.e.|
|`RECT getOwnRectPadded()`|Same as getOwnRect() but with current padding insets applied.|
|`POINT mapPointToWindow(POINT pt)`|Convert a point from own-rect coordinates to window coordinates.|
|`void setPadding(EDGEINSETS padding)`|Set padding insets, i.e.|
|`void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)`|Set the size this view would like to have, given various constraints (see MEASURESPEC and measure()).|
|`void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)`|Set the preferred alignment within the parent view.|
|`void setNeedsLayout()`|Signal that the view needs to recalculate its size and position.|
|`void layout(RECT constraint)`|Recalculate and apply this view's size and position (as well as all subviews).|
|`void setRectSize(const SIZE & size)`|Sets the size of the view rect.|
|`void setRectOrigin(const POINT & origin)`|Sets the position of the view.|
|`void setVisibility(Visibility visibility)`||


### Content & Scrolling

| | |
|-|-|
|`SIZE getContentSize()`|Returns the current content size, as calculated by updateContentSize() during layout.|
|`void invalidateContentSize()`|Mark the view's contentSize as invalid and call setNeedsLayout()|
|`void updateContentSize(SIZE constrainingSize)`|Called during measure() views should set the _contentSize property here.|
|`void setGravity(GRAVITY gravity)`|Set the gravity flags.|
|`POINT getContentOffset()`||
|`void setContentOffset(POINT contentOffset)`||
|`void setScrollInsets(EDGEINSETS scrollInsets)`||
|`bool canScrollHorizontally()`||
|`bool canScrollVertically()`||
|`float getMaxScrollY()`||
|`bool getClipsContent()`||
|`void setClipsContent(bool clipsContent)`||
|`void scrollBy(POINT scrollAmount)`||


### Drawing

| | |
|-|-|
|`void setBackground(RenderOp * renderOp)`||
|`RenderOp * getBackgroundOp()`||
|`void setBackgroundColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)`||
|`void addRenderOp(RenderOp * renderOp)`||
|`void addRenderOp(RenderOp * renderOp, bool atFront)`||
|`void removeRenderOp(RenderOp * renderOp)`||
|`void setNeedsFullRedraw()`||
|`void invalidateRect(const RECT & rect)`||
|[`COLOR`](/ref/graphics_group/COLOR)` getTintColor()`||
|`void setTintColor(`[`COLOR`](/ref/graphics_group/COLOR)` tintColor)`|Sets tint color.|
|`float getAlpha()`|Get the current alpha.|
|`void setAlpha(float alpha)`|Sets alpha.|


### State

| | |
|-|-|
|`void setState(STATE mask, STATE value)`|Set one or more state bits at once.|
|`bool isPressed()`||
|`void setPressed(bool isPressed)`||
|`bool isEnabled()`||
|`void setEnabled(bool enabled)`||
|`bool isChecked()`||
|`void setChecked(bool checked)`||
|`bool isSelected()`||
|`void setSelected(bool checked)`||
|`bool isFocused()`||
|`void setFocused(bool focused)`||
|`bool isErrored()`||
|`void setErrored(bool errored)`||
|`void onStateChanged(STATESET changes)`|Called whenever state changes.|


### Styles

| | |
|-|-|
|`bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)` * value)`|Applies a single style value for the given attribute name.|


### Touch

| | |
|-|-|
|[`View`](/ref/views/View)` * hitTest(POINT & pt)`||
|[`View`](/ref/views/View)` * subviewContainingPoint(POINT pt)`||
|`int indexOfSubviewContainingPoint(POINT pt)`||


| | |
|-|-|
|` View()`||
|` ~View()`||
|`IKeyboardInputHandler * getKeyboardInputHandler()`||
|`ITextInputReceiver * getTextInputReceiver()`||
|`bool requestFocus()`||
|`void animateAlpha(float target, float duration)`||
|`void animateTranslate(POINT translation, float duration)`||
|`Animation * animateInFromBottom(float duration, InterpolateFunc interpolater)`||
|`Animation * animateOutToBottom(float duration, InterpolateFunc interpolater)`||
|`void setTranslate(POINT translation)`||


| | |
|-|-|
|`void inflate(const `[`string`](/ref/base_group/string)` & layoutFile)`||
|`void bind(T *& rview, const `[`string`](/ref/base_group/string)` & id)`||


## Methods

| | |
|-|-|
| *addSubview* | `void addSubview(`[`View`](/ref/views/View)` * subview)` | last in the z-order |
| *insertSubview* | `void insertSubview(`[`View`](/ref/views/View)` * subview, int index)` |  |
| *getSubviewCount* | `int getSubviewCount()` |  |
| *getSubview* | [`View`](/ref/views/View)` * getSubview(int index)` | Returns NULL if no such index. |
| *removeSubview* | `void removeSubview(`[`View`](/ref/views/View)` * subview)` |  |
| *getParent* | [`View`](/ref/views/View)` * getParent()` |  |
| *removeFromParent* | `void removeFromParent()` |  |
| *indexOfSubview* | `int indexOfSubview(`[`View`](/ref/views/View)` * subview)` |  |
| *findViewById* | [`View`](/ref/views/View)` * findViewById(const `[`string`](/ref/base_group/string)` & id)` |  |
| *getRootView* | [`View`](/ref/views/View)` * getRootView()` |  |
| *removeSubviewsNotInVisibleArea* | `void removeSubviewsNotInVisibleArea()` |  |
| *setUsePrivateSurface* | `void setUsePrivateSurface(bool usePrivateSurface)` | A private surface is useful when rendering is particularly expensive and likely to contribute to lag. |
| *getWindow* | [`Window`](/ref/views/Window)` * getWindow()` | In practice this is the main application window or NULL. |
| *attachToWindow* | `void attachToWindow(`[`Window`](/ref/views/Window)` * window)` |  |
| *detachFromWindow* | `void detachFromWindow()` |  |
| *getWidth* | `float getWidth()` |  |
| *getHeight* | `float getHeight()` |  |
| *getLeft* | `float getLeft()` |  |
| *getRight* | `float getRight()` |  |
| *getTop* | `float getTop()` |  |
| *getBottom* | `float getBottom()` |  |
| *getRect* | `RECT getRect()` |  |
| *getOwnRect* | `RECT getOwnRect()` | (0,0,_rect.size.width,_rect.size.height) |
| *getOwnRectPadded* | `RECT getOwnRectPadded()` |  |
| *mapPointToWindow* | `POINT mapPointToWindow(POINT pt)` |  |
| *setPadding* | `void setPadding(EDGEINSETS padding)` | the space between the view rect and its content |
| *setMeasureSpecs* | `void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)` |  |
| *setAlignSpecs* | `void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)` | See ALIGNSPEC and layout() |
| *setNeedsLayout* | `void setNeedsLayout()` |  |
| *layout* | `void layout(RECT constraint)` | Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour. |
| *setRectSize* | `void setRectSize(const SIZE & size)` | CAUTION! This API is dumb and will probably be removed. |
| *setRectOrigin* | `void setRectOrigin(const POINT & origin)` | CAUTION! This API exists so Views may override the positioning of subviews during layout. Ideally this would not be a `public` API. |
| *setVisibility* | `void setVisibility(Visibility visibility)` |  |
| *getContentSize* | `SIZE getContentSize()` | The content size is not directly settable, each view should determine it's own. |
| *invalidateContentSize* | `void invalidateContentSize()` |  |
| *updateContentSize* | `void updateContentSize(SIZE constrainingSize)` | The defaut implementation does nothing. |
| *setGravity* | `void setGravity(GRAVITY gravity)` | Gravity affects how the content is aligned within the view rect when the view rect size exceeds the content size. |
| *getContentOffset* | `POINT getContentOffset()` |  |
| *setContentOffset* | `void setContentOffset(POINT contentOffset)` |  |
| *setScrollInsets* | `void setScrollInsets(EDGEINSETS scrollInsets)` |  |
| *canScrollHorizontally* | `bool canScrollHorizontally()` |  |
| *canScrollVertically* | `bool canScrollVertically()` |  |
| *getMaxScrollY* | `float getMaxScrollY()` |  |
| *getClipsContent* | `bool getClipsContent()` |  |
| *setClipsContent* | `void setClipsContent(bool clipsContent)` |  |
| *scrollBy* | `void scrollBy(POINT scrollAmount)` |  |
| *setBackground* | `void setBackground(RenderOp * renderOp)` |  |
| *getBackgroundOp* | `RenderOp * getBackgroundOp()` |  |
| *setBackgroundColor* | `void setBackgroundColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)` |  |
| *addRenderOp* | `void addRenderOp(RenderOp * renderOp)` |  |
| *addRenderOp* | `void addRenderOp(RenderOp * renderOp, bool atFront)` |  |
| *removeRenderOp* | `void removeRenderOp(RenderOp * renderOp)` |  |
| *setNeedsFullRedraw* | `void setNeedsFullRedraw()` |  |
| *invalidateRect* | `void invalidateRect(const RECT & rect)` |  |
| *getTintColor* | [`COLOR`](/ref/graphics_group/COLOR)` getTintColor()` |  |
| *setTintColor* | `void setTintColor(`[`COLOR`](/ref/graphics_group/COLOR)` tintColor)` |  |
| *getAlpha* | `float getAlpha()` | Defaults to 1.0. Note that the actual alpha value used in rendering inherits (i.e. is effectively multiplied by) all ancestor alpha values. |
| *setAlpha* | `void setAlpha(float alpha)` |  |
| *setState* | `void setState(STATE mask, STATE value)` | The STATESET parameter has a 'mask' member which determines the state bits that get updated. |
| *isPressed* | `bool isPressed()` |  |
| *setPressed* | `void setPressed(bool isPressed)` |  |
| *isEnabled* | `bool isEnabled()` |  |
| *setEnabled* | `void setEnabled(bool enabled)` |  |
| *isChecked* | `bool isChecked()` |  |
| *setChecked* | `void setChecked(bool checked)` |  |
| *isSelected* | `bool isSelected()` |  |
| *setSelected* | `void setSelected(bool checked)` |  |
| *isFocused* | `bool isFocused()` |  |
| *setFocused* | `void setFocused(bool focused)` |  |
| *isErrored* | `bool isErrored()` |  |
| *setErrored* | `void setErrored(bool errored)` |  |
| *onStateChanged* | `void onStateChanged(STATESET changes)` |  |
| *applyStyleValue* | `bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)` * value)` | Custom views should override this method to add support for custom attributes. |
| *hitTest* | [`View`](/ref/views/View)` * hitTest(POINT & pt)` |  |
| *subviewContainingPoint* | [`View`](/ref/views/View)` * subviewContainingPoint(POINT pt)` |  |
| *indexOfSubviewContainingPoint* | `int indexOfSubviewContainingPoint(POINT pt)` |  |
| *View* | ` View()` |  |
| *~View* | ` ~View()` |  |
| *getKeyboardInputHandler* | `IKeyboardInputHandler * getKeyboardInputHandler()` |  |
| *getTextInputReceiver* | `ITextInputReceiver * getTextInputReceiver()` |  |
| *requestFocus* | `bool requestFocus()` |  |
| *animateAlpha* | `void animateAlpha(float target, float duration)` |  |
| *animateTranslate* | `void animateTranslate(POINT translation, float duration)` |  |
| *animateInFromBottom* | `Animation * animateInFromBottom(float duration, InterpolateFunc interpolater)` |  |
| *animateOutToBottom* | `Animation * animateOutToBottom(float duration, InterpolateFunc interpolater)` |  |
| *setTranslate* | `void setTranslate(POINT translation)` |  |
| *inflate* | `void inflate(const `[`string`](/ref/base_group/string)` & layoutFile)` |  |
| *bind* | `void bind(T *& rview, const `[`string`](/ref/base_group/string)` & id)` |  |
