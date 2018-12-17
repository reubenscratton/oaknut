//
//  sysvia.hpp
//  libbeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#ifndef sysvia_hpp
#define sysvia_hpp

#include "via.hpp"

class SysVIA : public VIA {
public:
	
	uint8_t IC32;
	int scrsize;
	int keycol,keyrow;
	uint8_t keys[16][16];
	uint8_t sdbval;
	uint8_t sdbout;
	int lns;

	SysVIA();
	void reset();
	void writePortA(uint8_t val);
	void writePortB(uint8_t val);
	uint8_t readPortA();
	uint8_t readPortB();

	uint8_t* serialize(bool saving, uint8_t* p);
	
	void setVblankInt(bool val);
	void updateSdb();
	void writeIC32(uint8_t val);

	void updateKeys();

	
};


#endif /* sysvia_hpp */
