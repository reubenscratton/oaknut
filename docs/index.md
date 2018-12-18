---
layout: default
---
<iframe src="samples/minesweeper/xx.html" style="margin-top:24px; margin-left:16px;" width="320" height="360" align="right">
</iframe>
Oaknut is a new GUI framework for truly cross-platform C++ apps. An
Oaknut app can be built to run natively on any major OS, and can also run in
a web browser. The 'Minesweeper' sample app is
running in an iframe to the right of this text.

Oaknut is currently at the "proof of concept" stage, few parts are fully-featured
and no API is frozen.

## Getting started
1. Clone the Oaknut repository:

    `git clone https://github.com/reubenscratton/oaknut`

2. Set the `OAKNUT_DIR` environment variable to point to it:

    `export set OAKNUT_DIR=/path/to/oaknut`

3. Select one of the samples in `oaknut/samples`

	`cd oaknut/samples/xxx`

4. If you would like to use an IDE, run a make command in the project directory
to build the appropriate project files:

IDE | Command | Tested Versions
--- | ------- | ----
Android Studio | `make androidstudio` | 3.0, 3.1, 3.2
CLion | `make clion` | 2018.1
Visual Studio Code | `make vscode` | 1.30
XCode | `make xcode` | 9.3

If your IDE is not yet supported or if you prefer to not use any IDE then just run `make` on its own with no arguments to create a debug build for the host platform.

The compiled binary will be under the generated `.build` folder in the project
root (see [Building](guides/2_building) for details).
