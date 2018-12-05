---
layout: default
---

# Styles

Styles are named resource values in a global namespace accessible
through `App::getStyleValue()` and related APIs.

Some default styles are hardcoded into the Oaknut runtime, see the source file
`/src/app/styles.res` for details.

A secondary source of default styling is the `styles.res` file in the
root of the application assets bundle. The runtime loads this automatically
at startup, if it exists.

Other style resource files may be loaded at runtime with `App::loadStyleAsset()`.


### Style References

String style values prefixed with a '$' are references to styles declared
elsewhere:
````
  {
    class: Label
    forecolor: $colors.success
    text: $strings.salutation
  }

  colors: {
    success: #0F0
    failure: #F00
  }
  strings: {
    salutation: "Hello!"
  }
````
