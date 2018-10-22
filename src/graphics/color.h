//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class COLOR {
public:
    COLOR() : _val(0) {}
    COLOR(uint32_t val) : _val(val) {}
    operator uint32_t() const { return _val; }
    COLOR operator=(uint32_t val) { _val=val; return *this; }
    bool operator==(uint32_t val) const { return val==_val;}
    
    
    static COLOR interpolate(COLOR start, COLOR end, float val);
    
private:
    uint32_t _val;
};
