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

   - XCode `make xcode`
   - CLion `make cmake`
   - Android Studio `make androidstudio`
   - (more coming soon)

Alternatively, if your IDE is not yet supported or if you prefer to not use any IDE, then just run `make` on its own with no arguments.

The compiled binary will be under the generated `.build` folder in the project
root (see [Building](guides/building.md) for details).
