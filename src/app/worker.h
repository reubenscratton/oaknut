//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A 'worker' in Oaknut is effectively a remote process that consumes and produces binary data.
 Like a thread but without shared memory, the inputs and outputs are serialized.
 
 Worker exists to provide a common abstraction of background processing that will work on web
 as well as it does natively. If the W3C and/or browser makers ever restore SharedArrayBuffer
 to the WebWorker context then we can revisit this cos pthreads will be usable everywhere then.
 
 Workers are instantiated by their registered name. Note that web Workers must be built separately
 (see the docs).
 */

class Worker : public Object {
public:
    virtual void start(const variant& config)=0;
    virtual void process(const variant& data_in,
                         std::function<void(const variant&)> callback)=0;
    virtual void stop(std::function<void()> onStop)=0;
    
protected:
    Worker();
    
};

