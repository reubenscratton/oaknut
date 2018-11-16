//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

class WorkerApple : public Worker {
public:
    
    void start(const variant& config) override;
    void process(const variant& data_in, std::function<void(const variant&)> callback) override;
    void stop(std::function<void()> onStop) override;

protected:
    virtual void start_(const variant& config) { }
    virtual const variant process_(const variant& data_in) = 0;
    virtual void stop_() { }

    dispatch_queue_t _queue;
};


#endif
