---
layout: default
---

# View

```
class View
    : public Styleable
```


Fundamental UI building block.     

    
## Identity & Hierarchy

| `void addSubview(<a href="class_view.html">View</a> * subview)` | Adds a subview to the end of the subviews list, i.e. |
| `void insertSubview(<a href="class_view.html">View</a> * subview, int index)` | Inserts a subview at a particular index, z-order ascending. |
| `int getSubviewCount()` | Get the number of subviews. |
| `<a href="class_view.html">View</a> * getSubview(int index)` | Get a subview at a particular index, z-order ascending. |
| `void removeSubview(<a href="class_view.html">View</a> * subview)` | Removes a subview. |
| `<a href="class_view.html">View</a> * getParent()` | Return the parent view of this view. |
| `void removeFromParent()` | Removes this view from its parent view. |
| `int indexOfSubview(<a href="class_view.html">View</a> * subview)` | Find the index of the given subview, returns -1 if given view is not a subview. |
| `<a href="class_view.html">View</a> * findViewById(const <a href="classstring.html">string</a> & id)` | Find a view among the subviews (and all descendent views) given a string id. |
| `<a href="class_view.html">View</a> * getRootView()` | Return the view at the root of the view tree. |
| `void removeSubviewsNotInVisibleArea()` | Helper to remove any subviews currently scrolled out of sight. |


## Window & Surface

| `void setUsePrivateSurface(bool usePrivateSurface)` | Controls whether the view should render to a private surface or not. |
| `<a href="class_window.html">Window</a> * getWindow()` | Return the Window that this View is attached to. |
| `void attachToWindow(<a href="class_window.html">Window</a> * window)` | Called when the view is attached to the application Window and will be drawn and able to receive input events, etc. |
| `void detachFromWindow()` | Called when the view is detached from the application Window. |


## Size & Position

| `float getWidth()` | Get the width of the view. |
| `float getHeight()` | Get the height of the view. |
| `float getLeft()` | Get the X-coordinate of the left edge, in parent coords. |
| `float getRight()` | Get the X-coordinate of the right edge, in parent coords. |
| `float getTop()` | Get the Y-coordinate of the top edge, in parent coords. |
| `float getBottom()` | Get the Y-coordinate of the bottom edge, in parent coords. |
| `RECT getRect()` | Get the current view rect, in parent coords. |
| `RECT getOwnRect()` | Get the view rect in it's own coordinate system, i.e. |
| `RECT getOwnRectPadded()` | Same as getOwnRect() but with current padding insets applied. |
| `POINT mapPointToWindow(POINT pt)` | Convert a point from own-rect coordinates to window coordinates. |
| `void setPadding(EDGEINSETS padding)` | Set padding insets, i.e. |
| `void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)` | Set the size this view would like to have, given various constraints (see MEASURESPEC and measure()). |
| `void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)` | Set the preferred alignment within the parent view. |
| `void setNeedsLayout()` | Signal that the view needs to recalculate its size and position. |
| `void layout(RECT constraint)` | Recalculate and apply this view's size and position (as well as all subviews). |
| `void setRectSize(const SIZE & size)` | Sets the size of the view rect. |
| `void setRectOrigin(const POINT & origin)` | Sets the position of the view. |
| `void setVisibility(Visibility visibility)` |  |


## Content & Scrolling

| `SIZE getContentSize()` | Returns the current content size, as calculated by updateContentSize() during layout. |
| `void invalidateContentSize()` | Mark the view's contentSize as invalid and call setNeedsLayout() |
| `void updateContentSize(SIZE constrainingSize)` | Called during measure() views should set the _contentSize property here. |
| `void setGravity(GRAVITY gravity)` | Set the gravity flags. |
| `POINT getContentOffset()` |  |
| `void setContentOffset(POINT contentOffset)` |  |
| `void setScrollInsets(EDGEINSETS scrollInsets)` |  |
| `bool canScrollHorizontally()` |  |
| `bool canScrollVertically()` |  |
| `float getMaxScrollY()` |  |
| `bool getClipsContent()` |  |
| `void setClipsContent(bool clipsContent)` |  |
| `void scrollBy(POINT scrollAmount)` |  |


## Drawing

| `void setBackground(RenderOp * renderOp)` |  |
| `RenderOp * getBackgroundOp()` |  |
| `void setBackgroundColor(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| `void addRenderOp(RenderOp * renderOp)` |  |
| `void addRenderOp(RenderOp * renderOp, bool atFront)` |  |
| `void removeRenderOp(RenderOp * renderOp)` |  |
| `void setNeedsFullRedraw()` |  |
| `void invalidateRect(const RECT & rect)` |  |
| `<a href="class_c_o_l_o_r.html">COLOR</a> getTintColor()` |  |
| `void setTintColor(<a href="class_c_o_l_o_r.html">COLOR</a> tintColor)` | Sets tint color. |
| `float getAlpha()` | Get the current alpha. |
| `void setAlpha(float alpha)` | Sets alpha. |


## State

| `void setState(STATE mask, STATE value)` | Set one or more state bits at once. |
| `bool isPressed()` |  |
| `void setPressed(bool isPressed)` |  |
| `bool isEnabled()` |  |
| `void setEnabled(bool enabled)` |  |
| `bool isChecked()` |  |
| `void setChecked(bool checked)` |  |
| `bool isSelected()` |  |
| `void setSelected(bool checked)` |  |
| `bool isFocused()` |  |
| `void setFocused(bool focused)` |  |
| `bool isErrored()` |  |
| `void setErrored(bool errored)` |  |
| `void onStateChanged(STATESET changes)` | Called whenever state changes. |


## Styles

| `bool applyStyleValue(const <a href="classstring.html">string</a> & name, const <a href="class_style_value.html">StyleValue</a> * value)` | Applies a single style value for the given attribute name. |


## Touch

| `<a href="class_view.html">View</a> * hitTest(POINT & pt)` |  |
| `<a href="class_view.html">View</a> * subviewContainingPoint(POINT pt)` |  |
| `int indexOfSubviewContainingPoint(POINT pt)` |  |


## 



## 

| ` View()` |  |
| ` ~View()` |  |
| `<a href="class_i_keyboard_input_handler.html">IKeyboardInputHandler</a> * getKeyboardInputHandler()` |  |
| `<a href="class_i_text_input_receiver.html">ITextInputReceiver</a> * getTextInputReceiver()` |  |
| `bool requestFocus()` |  |
| `void animateAlpha(float target, float duration)` |  |
| `void animateTranslate(POINT translation, float duration)` |  |
| `Animation * animateInFromBottom(float duration, InterpolateFunc interpolater)` |  |
| `Animation * animateOutToBottom(float duration, InterpolateFunc interpolater)` |  |
| `void setTranslate(POINT translation)` |  |


## 

| `void inflate(const <a href="classstring.html">string</a> & layoutFile)` |  |
| `void bind(T *& rview, const <a href="classstring.html">string</a> & id)` |  |


# Methods

| *addSubview* |  `void <a href="todo">addSubview</a>(<a href="class_view.html">View</a> * subview)` | last in the z-order |
| *insertSubview* |  `void <a href="todo">insertSubview</a>(<a href="class_view.html">View</a> * subview, int index)` |  |
| *getSubviewCount* |  `int <a href="todo">getSubviewCount</a>()` |  |
| *getSubview* |  `<a href="class_view.html">View</a> * <a href="todo">getSubview</a>(int index)` | Returns NULL if no such index. |
| *removeSubview* |  `void <a href="todo">removeSubview</a>(<a href="class_view.html">View</a> * subview)` |  |
| *getParent* |  `<a href="class_view.html">View</a> * <a href="todo">getParent</a>()` |  |
| *removeFromParent* |  `void <a href="todo">removeFromParent</a>()` |  |
| *indexOfSubview* |  `int <a href="todo">indexOfSubview</a>(<a href="class_view.html">View</a> * subview)` |  |
| *findViewById* |  `<a href="class_view.html">View</a> * <a href="todo">findViewById</a>(const <a href="classstring.html">string</a> & id)` |  |
| *getRootView* |  `<a href="class_view.html">View</a> * <a href="todo">getRootView</a>()` |  |
| *removeSubviewsNotInVisibleArea* |  `void <a href="todo">removeSubviewsNotInVisibleArea</a>()` |  |
| *setUsePrivateSurface* |  `void <a href="todo">setUsePrivateSurface</a>(bool usePrivateSurface)` | A private surface is useful when rendering is particularly expensive and likely to contribute to lag. |
| *getWindow* |  `<a href="class_window.html">Window</a> * <a href="todo">getWindow</a>()` | In practice this is the main application window or NULL. |
| *attachToWindow* |  `void <a href="todo">attachToWindow</a>(<a href="class_window.html">Window</a> * window)` |  |
| *detachFromWindow* |  `void <a href="todo">detachFromWindow</a>()` |  |
| *getWidth* |  `float <a href="todo">getWidth</a>()` |  |
| *getHeight* |  `float <a href="todo">getHeight</a>()` |  |
| *getLeft* |  `float <a href="todo">getLeft</a>()` |  |
| *getRight* |  `float <a href="todo">getRight</a>()` |  |
| *getTop* |  `float <a href="todo">getTop</a>()` |  |
| *getBottom* |  `float <a href="todo">getBottom</a>()` |  |
| *getRect* |  `RECT <a href="todo">getRect</a>()` |  |
| *getOwnRect* |  `RECT <a href="todo">getOwnRect</a>()` | (0,0,_rect.size.width,_rect.size.height) |
| *getOwnRectPadded* |  `RECT <a href="todo">getOwnRectPadded</a>()` |  |
| *mapPointToWindow* |  `POINT <a href="todo">mapPointToWindow</a>(POINT pt)` |  |
| *setPadding* |  `void <a href="todo">setPadding</a>(EDGEINSETS padding)` | the space between the view rect and its content |
| *setMeasureSpecs* |  `void <a href="todo">setMeasureSpecs</a>(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec)` |  |
| *setAlignSpecs* |  `void <a href="todo">setAlignSpecs</a>(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert)` | See ALIGNSPEC and layout() |
| *setNeedsLayout* |  `void <a href="todo">setNeedsLayout</a>()` |  |
| *layout* |  `void <a href="todo">layout</a>(RECT constraint)` | Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour. |
| *setRectSize* |  `void <a href="todo">setRectSize</a>(const SIZE & size)` | CAUTION! This API is dumb and will probably be removed. |
| *setRectOrigin* |  `void <a href="todo">setRectOrigin</a>(const POINT & origin)` | CAUTION! This API exists so Views may override the positioning of subviews during layout. Ideally this would not be a `public` API. |
| *setVisibility* |  `void <a href="todo">setVisibility</a>(Visibility visibility)` |  |
| *getContentSize* |  `SIZE <a href="todo">getContentSize</a>()` | The content size is not directly settable, each view should determine it's own. |
| *invalidateContentSize* |  `void <a href="todo">invalidateContentSize</a>()` |  |
| *updateContentSize* |  `void <a href="todo">updateContentSize</a>(SIZE constrainingSize)` | The defaut implementation does nothing. |
| *setGravity* |  `void <a href="todo">setGravity</a>(GRAVITY gravity)` | Gravity affects how the content is aligned within the view rect when the view rect size exceeds the content size. |
| *getContentOffset* |  `POINT <a href="todo">getContentOffset</a>()` |  |
| *setContentOffset* |  `void <a href="todo">setContentOffset</a>(POINT contentOffset)` |  |
| *setScrollInsets* |  `void <a href="todo">setScrollInsets</a>(EDGEINSETS scrollInsets)` |  |
| *canScrollHorizontally* |  `bool <a href="todo">canScrollHorizontally</a>()` |  |
| *canScrollVertically* |  `bool <a href="todo">canScrollVertically</a>()` |  |
| *getMaxScrollY* |  `float <a href="todo">getMaxScrollY</a>()` |  |
| *getClipsContent* |  `bool <a href="todo">getClipsContent</a>()` |  |
| *setClipsContent* |  `void <a href="todo">setClipsContent</a>(bool clipsContent)` |  |
| *scrollBy* |  `void <a href="todo">scrollBy</a>(POINT scrollAmount)` |  |
| *setBackground* |  `void <a href="todo">setBackground</a>(RenderOp * renderOp)` |  |
| *getBackgroundOp* |  `RenderOp * <a href="todo">getBackgroundOp</a>()` |  |
| *setBackgroundColor* |  `void <a href="todo">setBackgroundColor</a>(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| *addRenderOp* |  `void <a href="todo">addRenderOp</a>(RenderOp * renderOp)` |  |
| *addRenderOp* |  `void <a href="todo">addRenderOp</a>(RenderOp * renderOp, bool atFront)` |  |
| *removeRenderOp* |  `void <a href="todo">removeRenderOp</a>(RenderOp * renderOp)` |  |
| *setNeedsFullRedraw* |  `void <a href="todo">setNeedsFullRedraw</a>()` |  |
| *invalidateRect* |  `void <a href="todo">invalidateRect</a>(const RECT & rect)` |  |
| *getTintColor* |  `<a href="class_c_o_l_o_r.html">COLOR</a> <a href="todo">getTintColor</a>()` |  |
| *setTintColor* |  `void <a href="todo">setTintColor</a>(<a href="class_c_o_l_o_r.html">COLOR</a> tintColor)` |  |
| *getAlpha* |  `float <a href="todo">getAlpha</a>()` | Defaults to 1.0. Note that the actual alpha value used in rendering inherits (i.e. is effectively multiplied by) all ancestor alpha values. |
| *setAlpha* |  `void <a href="todo">setAlpha</a>(float alpha)` |  |
| *setState* |  `void <a href="todo">setState</a>(STATE mask, STATE value)` | The STATESET parameter has a 'mask' member which determines the state bits that get updated. |
| *isPressed* |  `bool <a href="todo">isPressed</a>()` |  |
| *setPressed* |  `void <a href="todo">setPressed</a>(bool isPressed)` |  |
| *isEnabled* |  `bool <a href="todo">isEnabled</a>()` |  |
| *setEnabled* |  `void <a href="todo">setEnabled</a>(bool enabled)` |  |
| *isChecked* |  `bool <a href="todo">isChecked</a>()` |  |
| *setChecked* |  `void <a href="todo">setChecked</a>(bool checked)` |  |
| *isSelected* |  `bool <a href="todo">isSelected</a>()` |  |
| *setSelected* |  `void <a href="todo">setSelected</a>(bool checked)` |  |
| *isFocused* |  `bool <a href="todo">isFocused</a>()` |  |
| *setFocused* |  `void <a href="todo">setFocused</a>(bool focused)` |  |
| *isErrored* |  `bool <a href="todo">isErrored</a>()` |  |
| *setErrored* |  `void <a href="todo">setErrored</a>(bool errored)` |  |
| *onStateChanged* |  `void <a href="todo">onStateChanged</a>(STATESET changes)` |  |
| *applyStyleValue* |  `bool <a href="todo">applyStyleValue</a>(const <a href="classstring.html">string</a> & name, const <a href="class_style_value.html">StyleValue</a> * value)` | Custom views should override this method to add support for custom attributes. |
| *hitTest* |  `<a href="class_view.html">View</a> * <a href="todo">hitTest</a>(POINT & pt)` |  |
| *subviewContainingPoint* |  `<a href="class_view.html">View</a> * <a href="todo">subviewContainingPoint</a>(POINT pt)` |  |
| *indexOfSubviewContainingPoint* |  `int <a href="todo">indexOfSubviewContainingPoint</a>(POINT pt)` |  |
| *View* |  ` <a href="todo">View</a>()` |  |
| *~View* |  ` <a href="todo">~View</a>()` |  |
| *getKeyboardInputHandler* |  `<a href="class_i_keyboard_input_handler.html">IKeyboardInputHandler</a> * <a href="todo">getKeyboardInputHandler</a>()` |  |
| *getTextInputReceiver* |  `<a href="class_i_text_input_receiver.html">ITextInputReceiver</a> * <a href="todo">getTextInputReceiver</a>()` |  |
| *requestFocus* |  `bool <a href="todo">requestFocus</a>()` |  |
| *animateAlpha* |  `void <a href="todo">animateAlpha</a>(float target, float duration)` |  |
| *animateTranslate* |  `void <a href="todo">animateTranslate</a>(POINT translation, float duration)` |  |
| *animateInFromBottom* |  `Animation * <a href="todo">animateInFromBottom</a>(float duration, InterpolateFunc interpolater)` |  |
| *animateOutToBottom* |  `Animation * <a href="todo">animateOutToBottom</a>(float duration, InterpolateFunc interpolater)` |  |
| *setTranslate* |  `void <a href="todo">setTranslate</a>(POINT translation)` |  |
| *inflate* |  `void <a href="todo">inflate</a>(const <a href="classstring.html">string</a> & layoutFile)` |  |
| *bind* |  `void <a href="todo">bind</a>(T *& rview, const <a href="classstring.html">string</a> & id)` |  |
