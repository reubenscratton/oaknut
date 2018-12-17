//
//  uservia.cpp
//  libbeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "uservia.hpp"
#include <string.h>
#include "libbeeb.h"


int mx=0,my=0;
int timerout=1;


UserVIA::UserVIA() : VIA(2) {
}


void UserVIA::writePortA(uint8_t val) {
	// printer port
}

void UserVIA::writePortB(uint8_t val) {
	// user port
}

uint8_t UserVIA::readPortA() {
	return 0xff; // printer port
}

uint8_t UserVIA::readPortB() {
	return 0xff; // user port (TODO: mouse, compact joystick)
};



/*
 int mxs=0,mys=0;
 int mfirst=1;
 int mon=0;
 int beebmousex=0,beebmousey=0;
 void domouse()
 {
 int x,y;
 if (mouse_x>0 && mouse_x<640 && mouse_y>0 && mouse_y<512)
 {
 beebmousex=mouse_x;
 beebmousey=256-(mouse_y/2);
 }
 if (ifr&0x18) return;
 if (mouse_x!=mx)
 {
 ifr|=0x10;
 if (mxs==((mouse_x>mx)?1:-1)) portb^=8;
 mxs=(mouse_x>mx)?1:-1;
 mx+=mxs;
 }
 if (mouse_y!=my)
 {
 if (mfirst)
 {
 mfirst=0;
 portb|=0x10;
 }
 ifr|=0x08;
 if (mys==((mouse_y>my)?1:-1)) portb^=0x10;
 mys=(mouse_y>my)?1:-1;
 my+=mys;
 }
 updateuserIFR();
 if (mouse_b&1) portb&=~1;
 else           portb|=1;
 if (mouse_b&2) portb&=~4;
 else           portb|=4;
 portb|=2;
 mem[0x2821]=beebmousey>>8;
 mem[0x2822]=beebmousey&0xFF;
 mem[0x2823]=beebmousex>>8;
 mem[0x2824]=beebmousex&0xFF;
 }
 
 void getmousepos(uint16_t *AX, uint16_t *CX, uint16_t *DX)
 {
 int c=mouse_b&1;
 if (mouse_b&2) c|=4;
 if (mouse_b&4) c|=2;
 *AX=c;
 *CX=beebmousex;
 *DX=beebmousey;
 }
 */

uint8_t* UserVIA::serialize(bool saving, uint8_t* p) {
	p = VIA::serialize(saving, p);
	SERIALIZE(mx);
	SERIALIZE(my);
	SERIALIZE(timerout);
	return p;
}


