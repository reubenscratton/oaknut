//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup base_group
 * @class Timer
 * @brief A simple timer class.
 */
class Timer : public Object {
public:
    static Timer* start(const std::function<void()>& del, int intervalMillis, bool repeats);
    virtual void stop();
    
protected:
    Timer(const std::function<void()>& del, int intervalMillis, bool repeats);
    ~Timer();

    std::function<void()> _del;
    bool _repeats;
    bool _active;
};


