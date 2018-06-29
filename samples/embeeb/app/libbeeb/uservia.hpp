//
//  uservia.hpp
//  libbeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef uservia_hpp
#define uservia_hpp

#include "via.hpp"

class UserVIA : public VIA {
public:
	
	UserVIA();
	virtual void writePortA(uint8_t val);
	virtual void writePortB(uint8_t val);
	virtual uint8_t readPortA();
	virtual uint8_t readPortB();

	uint8_t* serialize(bool saving, uint8_t* p);

};



#endif /* uservia_hpp */
