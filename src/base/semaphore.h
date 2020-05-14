//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class semaphore {
public:
    semaphore(uint32_t value=0);
    ~semaphore();
    bool wait(int timeoutMillis=-1);
    void signal();

protected:
#ifdef __APPLE__
    dispatch_semaphore_t _sem;
#else
    sem_t _sem;
#endif

};
