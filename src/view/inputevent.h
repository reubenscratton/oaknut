//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define INPUT_SOURCE_TYPE_KEY 0
#define INPUT_SOURCE_TYPE_MOUSE 1
#define INPUT_SOURCE_TYPE_FINGER 2
#define INPUT_SOURCE_TYPE_SCROLLER 3

#define MAKE_SOURCE(type, id) ((type<<8)|id)
#define SOURCE_TYPE(source) (source>>8)
#define SOURCE_ID(source) (source&255)

#define INPUT_EVENT_DOWN 0
#define INPUT_EVENT_MOVE 1
#define INPUT_EVENT_UP   2
#define INPUT_EVENT_CANCEL 3
#define INPUT_EVENT_DRAG 4
#define INPUT_EVENT_TAP 6
#define INPUT_EVENT_TAP_CONFIRMED 7
#define INPUT_EVENT_FLING 8
#define INPUT_EVENT_LONG_PRESS 9

#define NUM_PAST 10

// TODO: these constants should be in platform styles
#define TOUCH_SLOP 10 // DPs
#define MULTI_CLICK_THRESHOLD 400 // ms
#define LONG_PRESS_THRESHOLD 800 // ms

struct INPUTEVENT {
    int type;
    enum {
        Mouse,
        Touch,
        ScrollWheel //
    } deviceType;
    int deviceIndex;
    TIMESTAMP time;
    POINT pt, ptLocal;
    POINT delta; // specific to ScrollWheel
    POINT velocity; // specific to flings
};
