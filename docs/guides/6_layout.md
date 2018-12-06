---
layout: default
---
# Layout

The size and position of a `View` as it appears within it's parent is in its
`_rect` property. The `_rect` property is seldom set directly, it is normally
set by the `View::layout()` method, called when needed by the runtime for all
visible views.

The default implementation of `View::layout()` uses the following five
`View` properties plus the given `RECT constraint` parameter to determine the
View's rect:
````
  SIZE _contentSize;
  MEASURESPEC _widthMeasureSpec;
  MEASURESPEC _heightMeasureSpec;
  ALIGNSPEC _alignspecHorz;
  ALIGNSPEC _alignspecVert;
````

Considering each in turn:

### Content size
If a View has 'content', i.e. something it draws, then it should set its
`_contentSize` property appropriately. Among the stock widgets, an `ImageView`'s
content size is the displayed size of the image (NB: not necessarily the same as
the image size), and a `Label`'s content size is the size of the displayed text.

If a view's content changes in some way (e.g. a Label updates it's text or it's
font size) then this will trigger a call to `View::invalidateContentSize()`.
This in turn triggers a call to the `View::updateContentSize()` method whose job
it is to set the _contentSize property appropriately. The default implementation
in `View::updateContentSize()` sets the content size as zero if there are no
subviews, or if it does contain subviews then the content size is set to
encompass the furthest extent of its subviews.

If a view's content size is larger than the view's `_rect` then the view becomes
scrollable. If the content size is smaller than the view's own size then the
content will be positioned with respect to the view's `_gravity` property.


### MEASURESPEC

The two `MEASURESPEC` properties `_widthMeasureSpec` and `_heightMeasureSpec`
control the view's width and height in `_rect.size`. A MEASURESPEC is
essentially two numbers: a multiplier that's applied to a reference size,
plus a constant:
````
    size = reference size * multiplier + constant
````
What gets used as the reference size depends on the MEASURESPEC's *type*,
`Relative`, `Content`, or `Aspect`:
 - `Relative`: The reference is a View, either the parent view or a sibling
   view. (NB: a NULL value implies the parent).
 - `Content`: The reference is the view's own content size.
 - `Aspect`: The reference size is the view's other dimension.


### ALIGNSPEC
The two `ALIGNSPEC` properties determine the view's position in `_rect.origin`.
They are similar to `MEASURESPEC` except they have two multipliers rather than
one, the second multiplier is applied to the view's own size and can therefore
be used to control the view's alignment w.r.t. the reference position.
````
    position = reference position * reference multiplier
             + self size * self multiplier
             + constant
````



## Layout resources

A layout resource file contains a single View object which may of course
contain subviews, etc. `App::layoutInflate()` may be used to 'inflate' the layout
resource file into a `View`.

Each view object in the resource file must have a `class` field which identifies
the View-derived C++ class to be instantiated, and can optionally have a
`subviews` field whose value is an array of view objects. All other fields
are style values and the inflate process will call
`View::applyStyleValue()` for each of them.



### Sizes (size,width,height)
The 'size' value may be a single value, in which case it applies to both width and height, or it may be an  array of two values (e.g. 'fill, 30dp'). Alternatively `width` and `height`
may be specified separately.

A single size value has up to 3 components: a ref, a multiplier, and a constant. The 'ref' is a string: "fill", "wrap", "aspect", or the id of a previously-declared sibling view. If the ref is omitted but a multiplier is used then the parent view is assumed as the ref.

The 'multiplier' tells how to multiply the reference size (the default multiplier when a reference is specified is 1.0) and the constant part is added.

Some examples:

| Size value         | ref     | mult | constant | Meaning  |
|--------------------|---------|------|----------|----------|
| wrap			         | content | 1.0  |     0    | Size will match view's content size. |
| 30dp			         |         |  0   |  30dp    | 30dp     |
| 100% - 32dp        | parent  | 1.0  | -32dp    | 100% of parent size, - 32dp |
| 50% + 32dp         | parent  | 0.5  |  32dp    | 50% of parent size, + 32dp |
| view_id            | view_id | 1.0  |     0    | Same size as view_id |
| view_id(25%) - 8px | view_id | 0.25 |  -8px    | 25% of view_id size, minus 8px |
| aspect(0.75)       | self    | 0.75 |     0    | Size is 75% of the other dimension |


There is potential for ambiguity when a multiplier or constant is given but not both: Should the simple value "2" be interpreted as a multiplier of 2x or as a constant of 2 pixels? It will be interpreted as a constant, unless the percentage notation was used in which case it is of course a multiplier.


### Alignments (align, alignX, alignY)

Alignment values are expressed in one of the following forms:

| Form     | Example |
|----------|--------|
| *constant* | `16dp` |
| *keyword(constant)* | `right(-8dp)` |
| *keyword(reference + constant)* | `below(label + 8dp)` |


The keywords and how they map to
the multipliers in the `ALIGNSPEC` are:

| <br>Keyword         | <br>ref    | ref<br>mult | self<br>mult |
|---------------|--------|------|------|
| `left`          | parent |   0  |   0  |
| `top`          | parent |   0  |   0  |
| `center`        | parent |  0.5 | -0.5 |
| `right`         | parent |  1.0 | -1.0 |
| `bottom`        | parent |  1.0 | -1.0 |
| `toLeftOf` | sibling | 1.0 | -1.0 |
| `toRightOf` | sibling | 1.0 | 0 |
| `above` | sibling | 1.0 | -1.0 |
| `below` | sibling | 1.0 | 0 |

If the keyword implies that the reference is the parent view then no reference is needed. If the reference is a sibling view
then the reference should be its ID

If the keyword implies a sibling view (`toLeftOf`, `toRightOf`, `below`, or `above`) and no reference ID is given then the previously-declared view is assumed.
