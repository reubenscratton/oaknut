//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ProgressSpinner : public CanvasView {
public:
    
    // API
    ProgressSpinner();
    
    // Overrides
    virtual void layout();
    void redraw();
    void attachToWindow(Window* window);
    void detachFromWindow();
    
    sp<Timer> _animTimer;
    int _phase;
};

