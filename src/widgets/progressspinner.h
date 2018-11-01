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
    void layout(RECT constraint) override;
    void redraw() override;
    void attachToWindow(Window* window) override;
    void detachFromWindow() override;
    
    sp<Timer> _animTimer;
    int _phase;
};

