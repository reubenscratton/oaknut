---
layout: default
---
<iframe src="xx.html" style="margin-top:24px; margin-left:16px;" width="320" height="320" align="right">
</iframe>
# Hello World!

The traditional first program.

```cpp
#include <oaknut.h>

void App::main() {
    Label* label = new Label();
    label->setBackgroundColor(0xFFFFFFFF);
    label->setGravity({GRAVITY_CENTER, GRAVITY_CENTER});
    label->setText("Hello World!");
    ViewController* vc = new ViewController();
    vc->setView(label);
    _window->setRootViewController(vc);
}
```
