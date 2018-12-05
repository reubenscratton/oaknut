---
layout: default
---
<iframe src="samples/minesweeper/xx.html" style="margin-top:24px; margin-left:16px;" width="320" height="400" align="right">
</iframe>
Oaknut is an experimental GUI framework for truly cross-platform C++ apps. An
Oaknut app can be built to run natively on any major OS, and can also run in
a web browser. The 'Minesweeper' sample app is
running in an iframe to the right of this text.

Oaknut is currently at the "proof of concept" stage, few parts are fully-featured
and no API is frozen. However all the main problems are solved and all work from
the current point lies in building out the many APIs and UI widgetry that modern
developers need.

## Getting started
1. Clone the Oaknut repository:

    `git clone https://github.com/reubenscratton/oaknut`

2. Set the `OAKNUT_DIR` environment variable to point to it:

    `export set OAKNUT_DIR=/path/to/oaknut`

3. Select one of the samples in `oaknut/samples`

	`cd oaknut/samples/xxx`

4. If you would like to use an IDE, run a make command to build an appropriate project file:

   - XCode `make xcode`
   - CLion `make cmake`
   - Android Studio `make androidstudio`
   - (more coming soon)

Alternatively, if your IDE is not yet supported or if you prefer to not use any IDE, then just run `make` on its own:

  `make`

The compiled binary will be under the generated `.build` folder in the project
root (see [Building](guides/building.md) for details).









#### Writing UI
Oaknut supports declarative layout files whose syntax is a "light" form of JSON,
with far fewer redundant declarations. Unlike Android resource qualifiers are
allowed on individual attributes as well as files, e.g.

```
Label: {
  id: hello
  text@en_GB: "Hello!"
  text@en_US: "Hi!"
  text@fr_FR: "Bonjour!"
}
```

In this "light" JSON quotes are unnecessary for field names and are
optional for string values - the only real use for quotes is for multiline
text.  There is also no need for commas to separate fields.

In a layout file each object declaration must be the name of a View-derived
class, and each non-object attribute is some property supported by that class.


#### Debugging

One of the more attractive features of Oaknut is the ability
to debug on the native platform (Mac, Linux, Windows) and then
later deploy to another (Android, iOS, Web). The fast build system
and not having to deploy to another machine or virtual machine make
incremental rebuild times a fraction of what is normal for mobile development.
