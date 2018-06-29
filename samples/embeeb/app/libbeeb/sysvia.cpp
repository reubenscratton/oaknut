//
//  sysvia.cpp
//  iBeeb
//
//  Created by Reuben Scratton on 15/12/2015.
//  Copyright © 2015 Sandcastle Software Ltd. All rights reserved.
//

#include "sysvia.hpp"
#include "libbeeb.h"
#include "beeb.h"

SysVIA::SysVIA() : VIA(1) {
}

void SysVIA::reset() {
	VIA::reset();
	memset(keys, 0, sizeof(keys));
}


void SysVIA::writePortA(uint8_t val) {
	sdbout = val;
	updateSdb();
	//if (isMaster) cmos.write(self.IC32, self.sdbval);
}

void SysVIA::writePortB(uint8_t val) {
	writeIC32(val);
	//if (isMaster) cmos.writeAddr(val, self.sdbval);
}

uint8_t SysVIA::readPortA() {
	updateSdb();
	return sdbval;
}

uint8_t SysVIA::readPortB() {
	return 0xff;
}


void SysVIA::setVblankInt(bool val) {
	setca1(val);
}


void SysVIA::updateSdb() {
	sdbval = sdbout;
	//if (isMaster) self.sdbval &= cmos.read(self.IC32);
	uint8_t keyrow = (sdbval >> 4) & 7;
	keycol = sdbval & 0xf;
	updateKeys();
	if (!(IC32 & 8) && !keys[keycol][keyrow]) {
		sdbval &= 0x7f;
	}
	if (/*!isMaster &&*/ !(IC32 & 4)) {
		sdbval = 0xff; // unsure; taken from beebem
	}
	if (/*!isMaster &&*/ !(IC32 & 2)) {
		sdbval = 0x00;  // no speech
	}
};


void SysVIA::writeIC32(uint8_t val) {
	uint8_t oldIC32=IC32;
	if (val & 8)
		IC32 |= (1 << (val & 7));
	else
		IC32 &= ~(1 << (val & 7));
	
	
	updateSdb();

	// TODO: investigate jsbeeb's method and check we match
	//soundChip.updateSlowDataBus(sdbval, !(IC32 & 1));
	if (!(IC32&1) && (oldIC32&1))
		writesound(sdbval);
	
	// LEDs changed
	if ((oldIC32^IC32) & 0xc0) {
		the_beeb->keyboardCallbacks->LEDsChanged();
	}

	// TODO: investigate jsbeeb's method and check we match
	// video.setScreenAdd(((IC32 & 16) ? 2 : 0) | ((IC32 & 32) ? 1 : 0));
	scrsize=((IC32&16)?2:0)|((IC32&32)?1:0);

	//if (isMaster) cmos.write(self.IC32, self.sdbval);
};



/*
 * updateKeys - scans bbckey[] and updates sysvia registers accordingly
 */
void SysVIA::updateKeys()
{
	int c,d;
	if (IC32&8) {
		for (d=0;d<(/*(MASTER)?13:*/10);d++) {
			for (c=1;c<8;c++) {
				if (keys[d][c]) {
					//					LOGI("keydown 0x%X", (c<<8)|d);
					setca2(true);
					return;
				}
			}
		}
	}
	else {
		if (keycol<(/*(MASTER)?13:*/10)) {
			for (c=1;c<8;c++) {
				if (keys[keycol][c]) {
					//					LOGI("keydown 0x%X", (c<<8)|d);
					setca2(true);
					return;
				}
			}
		}
	}
	setca2(false);
}



uint8_t* SysVIA::serialize(bool saving, uint8_t* p) {
	p = VIA::serialize(saving, p);
	SERIALIZE(IC32);
	SERIALIZE(scrsize);
	SERIALIZE(keycol);
	SERIALIZE(keyrow);
	SERIALIZE(keys);
	SERIALIZE(keys);
	SERIALIZE(sdbval);
	SERIALIZE(lns);
	return p;
}

