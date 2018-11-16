//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

class WorkerWeb : public Worker {
public:
    worker_handle _worker;
    std::function<void()> _onStop;
    list<std::function<void(const variant&)>> _pending;

    WorkerWeb(const string& workerName);
    ~WorkerWeb();
    
    void start(const variant& config) override;
    void process(const variant& data_in, std::function<void(const variant&)> callback) override;
    
    virtual void handleCallback(const variant& data_out);
    
    void stop(std::function<void()> onStop) override;
    
    static void cback_encode(char* data, int size, void* arg);
    static void cback_stop(char* data, int size, void* arg);
};




#endif

