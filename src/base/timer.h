//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


typedef std::function<void ()> TimerDelegate;

class Timer : public Object {
public:
    static Timer* start(const TimerDelegate& del, int intervalMillis, bool repeats);
    virtual void stop();
    
protected:
    Timer(const TimerDelegate& del, int intervalMillis, bool repeats);
    ~Timer();

    TimerDelegate _del;
    bool _repeats;
    bool _active;
};


