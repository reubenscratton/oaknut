//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/*
 
 */


class Fling {
public:
    Fling(float start, float velocity, float min, float max);
    virtual void update(float timePassedSeconds);
    
    float _start;
    float _final;
    float _min;
    float _max;
    float _curr;
    TIMESTAMP _startTime;
    int _duration;
    float _coeff;
    float _velocity;
};


class ScrollInfo {
public:
    friend class View;
    friend class Surface;
    
    enum Show {
        Default,
        Never,
        Always
    } _show;
    bool _bounce;
    bool _canScroll;

    void updateVisibility(View* view, bool isVertical);
    bool canScroll(View* view, bool isVertical);
    bool handleEvent(View* view, bool isVertical, INPUTEVENT* event);

    float flingUpdate();
    void flingCancel();
    void detach();


protected:
    float _dragStart, _dragLast, _dragTotal;
    float _offsetStart;
    float _alpha;
    ObjPtr<class Animation> _fadeAnim;
    ObjPtr<Timer> _fadeTimer;
    Fling* _fling;
    RectRenderOp* _renderOp;
    
    void startFadeAnim(float targetAlpha);
    void updateRect(View* view);

};



