//
//  Beeb.h
//  iBeeb
//
//  Created by Reuben Scratton on 28/09/2015.
//  Copyright © 2015 Sandcastle Software Ltd. All rights reserved.
//

#include "libbeeb.h"

#include <stdio.h>
#include <list>
#include "6502.h"
#include "video.hpp"
#include "uservia.hpp"
#include "sysvia.hpp"
#include "acia.h"

typedef struct  {
    int scancode;
    bool shift;
    bool down;
} BeebKeyEvent;



class IBeebDisplayCallbacks {
public:
	virtual void setVisibleArea(int left, int top, int width, int height) = 0;
	virtual void drawFrame() = 0;
};
class IBeebKeyboardCallbacks {
public:
	virtual void LEDsChanged() = 0;
};



class Beeb : public Object {
public:
	M6502 cpu;
	Video video;
	SysVIA sysvia;
	UserVIA uservia;
	int autoboot;
	int resetting;
	ACIA acia;
    
    uint8_t* _mem;
    uint8_t* roms[16];
    AudioOutput* _audioOutput;
    bool altFrame;
    std::list<BeebKeyEvent> keyEventQueue;
    IBeebDisplayCallbacks* displayCallbacks;
    IBeebKeyboardCallbacks* keyboardCallbacks;

	Beeb();
	void reset();
	void tick();
	void breakKey();
	void loadDisc(uint8_t* directBuffer, int cb, int aautoboot);
	void postKeyboardEvent(int bbcKey, bool isDown);
	void typeText(char const* text);
	
	uint8_t* serialize(bool saving, uint8_t* p);

	bool isCapsLockLEDon();
	bool isShiftLockLEDon();
    
    void setVideoBitmapParams(uint8_t* bitmapPixels, int bitmapStride);

};

extern Beeb* the_beeb;
