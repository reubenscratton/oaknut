//
//  Beeb.cpp
//  libbeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <algorithm>
#include <list>


#include "beeb.h"
#include "libbeeb.h"
#include "beebkey.h"
#include "6502.h"
#include "video.hpp"
#include "uservia.hpp"
#include "sysvia.hpp"
#include "acia.h"

Beeb* the_beeb;




Beeb::Beeb() {
    the_beeb = this;
    _mem = (uint8_t*)calloc(65536, 1);

    extern unsigned char os_rom[];
    extern unsigned char DFS_0_9_rom[];
    extern unsigned char BASIC_ROM[];
    
    roms[0] = DFS_0_9_rom;
    roms[1] = BASIC_ROM;

	// TODO: loadcmos(models[curmodel]);
	
	// OS ROM first.
    memcpy(_mem+0xc000, os_rom, 0x4000);
 
	//return [[BeebModelInfo alloc] initWithName:@"BBC B w/8271 FDC" roms:@[] flags:FLAG_I8271];
	
    /*
    Other model types to add at some point
	new ModelInfo("BBC A",             "os",   "a/BASIC.ROM", FLAG_I8271 | FLAG_MODELA),
	new ModelInfo("BBC B w/8271+SWRAM","os",   "b/DFS-0.9.rom;b/BASIC.ROM", FLAG_I8271 | FLAG_SWRAM),
	new ModelInfo("BBC B w/1770 FDC",  "os",   "b/DFS-0.9.rom;b/BASIC.ROM", FLAG_WD1770 | FLAG_SWRAM),
	new ModelInfo("BBC B+ 64K",        "bpos", "bp/dfs.rom;bp/BASIC.ROM;bp/zADFS.ROM", FLAG_WD1770 | FLAG_BPLUS),
    */

	//if (models[curmodel].swram) fillswram();

#ifdef TARGET_IOS
    _audioOutput = new NativeAudioOutput();
#else
    _audioOutput = new AudioOutput(4096);
#endif
    _audioOutput->open();
    
    BeebKey_init();

	cpu.mem = _mem;
    autoboot = 0;

    reset();

    initsound();
    initadc();
}


// TODO: this is IOS-specific
/*uint32_t Beeb_audioInputProc(HBEEB hbeeb, int8_t* buffer, uint32_t cb) {
    Beeb* beeb = (Beeb*)hbeeb;
    uint32_t cbThis = beeb->audiobuff->read((uint8_t*)buffer, cb);
	cb -= cbThis;
	if (cb > 0) {
		memset(buffer+cbThis, 0, cb);
	}
    return cbThis;
}



void Beeb_deleteChar(HBEEB hbeeb) {
    Beeb_postKeyboardEvent(hbeeb, ScanCode_Delete, true);
    Beeb_postKeyboardEvent(hbeeb, ScanCode_Delete, false);
}

*/


void Beeb::postKeyboardEvent(int bbcKey, bool isDown) {
	BeebKeyEvent event;
	event.scancode = bbcKey;
	event.shift = (bbcKey>=256);
	event.down = isDown;
	keyEventQueue.push_back(event);
}

void Beeb::typeText(char const* text) {
    int len = (int)strlen(text);
    for (int i=0 ; i<len ; i++) {
		int scanCode = BeebKey_scancodeForChar(text[i]);
		if (scanCode != 0) {
            postKeyboardEvent(scanCode, true);
            postKeyboardEvent(scanCode, false);
		}
	}
}


void Beeb::tick() {
	
	// Drain the keyboard event queue every other frame (i.e. at 25fps)
	altFrame = !altFrame;
	if (altFrame) {
		if (keyEventQueue.size() > 0) {
			BeebKeyEvent keyEvent = keyEventQueue.front();
			keyEventQueue.pop_front();
            int vkey = keyEvent.scancode;
            int down = keyEvent.down ? 1 : 0;
            
            if ((vkey&255)==0xaa && down) {
                breakKey();
                //return;
            } else {
            
                bool implicitShift = vkey & 0x100;
                int col=vkey&15;
                int row=(vkey>>4)&15;
                
                
                // Press / unpress SHIFT
                if (implicitShift) {
                    sysvia.keys[0][0] =  down;
                }
                
                // Press / unpress the key
                sysvia.keys[col][row] = down;
                sysvia.updateKeys();
                //app.log("Key event %d,%d = %d", col, row, down);
                
                // If code-coverage is pending (waiting for a key down) then turn it on.
                if (down) {
                    if (cpu.codeCoverageMode == 2) {
                        //Beeb_setCodeCoverageMode(hbeeb, 1);
                    } else if (cpu.codeCoverageMode == 1) {
                        //Beeb_setCodeCoverageMode(hbeeb, 0);
                    }
                }
            }

		}
	}
	
	// Execute 1/50th of a second of BBC micro!
	if (autoboot)
		autoboot--;
	
#ifdef USE_ASM
	exec6502(&cpu);
#else
	exec6502_c();
#endif
	
	// Execute any trigger callbacks
	int nt = cpu.num_triggers;
	for (int i=0 ; i<nt ; i++) {
		pc_trigger* trigger = &cpu.pc_triggers[i];
		if (trigger->hit) {
			trigger->hit = 0;
			trigger->callback(trigger->callback_param1, trigger->callback_param2);
		}
	}
		
	// Break!
	if (resetting) {
		bbcLogI("resetting... ");
		reset6502();
		reset8271();
		resetting = 0;
	}
	
}


extern "C" {
void loaddisc(int drive, int loader, unsigned char* discimg, int cb);
}

void Beeb::loadDisc(uint8_t* directBuffer, int cb, int aautoboot) {
	loaddisc(0, 0, directBuffer, cb);

	if (aautoboot) {
		autoboot = 150;
		resetting = 1;
		sysvia.reset();
	}
}

/*int Beeb_codeCoverageMode(HBEEB hbeeb) {
	return ((Beeb*)hbeeb)->cpu.codeCoverageMode;
}

void Beeb_setCodeCoverageMode(HBEEB hbeeb, int mode) {
    Beeb* beeb = (Beeb*)hbeeb;
	if (mode != beeb->cpu.codeCoverageMode) {
		beeb->cpu.codeCoverageMode = mode;
		if (mode == 1) { //on
			if (!beeb->cpu.codeCoverageBuff) {
				beeb->cpu.codeCoverageBuff = (uint8_t*)malloc(65536);
			}
			memset(beeb->cpu.codeCoverageBuff, 0, 65536);
		}
		if (mode == 0) {
			uint8_t* buff = beeb->cpu.codeCoverageBuff;
			if (buff) {
				for (int i=0 ; i<32768 ; i++) {
					if ((buff[i] & 0xc0) == 0xc0) {
						int count = (buff[i] & 0x3f);
						if (count <= 5) {
							bbcLogI("%04X: JUMP! (%d)", i, count);
						}
					}
				}
				for (int i=0 ; i<32768 ; i++) {
					if ((buff[i] & 0xc0) == 0x80) {
						int count = (buff[i] & 0x3f);
						if (count <= 5) {
							bbcLogI("%04X: Branch (%d)", i, count);
						}
					}
				}
				for (int i=0 ; i<32768 ; i++) {
					if (buff[i] == 1) {
						//bbcLogI("%04X: exec", i);
					}
				}
			}
			
		}
	}
}

uint8_t* Beeb_codeCoverageBuffer(HBEEB hbeeb) {
    Beeb* beeb = (Beeb*)hbeeb;
	return beeb->cpu.codeCoverageBuff;
}

void Beeb_resetTriggers(HBEEB hbeeb) {
    Beeb* beeb = (Beeb*)hbeeb;
	for (int i=0 ; i<beeb->cpu.num_triggers ; i++) {
		//todo: CFBridgingRelease(beeb->cpu.pc_triggers[i].callback_param);
	}
	beeb->cpu.num_triggers = 0;
}

void Beeb_addTrigger(HBEEB hbeeb, uint16_t addr, PCALLBACK2 callback, void* param1, void* param2) {
    Beeb* beeb = (Beeb*)hbeeb;
	if (beeb->cpu.num_triggers == 8) {
		bbcLogE("Too many triggers");
		return;
	}
	pc_trigger* trigger = &beeb->cpu.pc_triggers[beeb->cpu.num_triggers++];
	trigger->addr = addr;
	trigger->hit = 0;
	trigger->callback = callback;
	trigger->callback_param1 = param1;
	trigger->callback_param2 = param2;
}
bool Beeb_isCapsLockOn(HBEEB hbeeb) {
    Beeb* beeb = (Beeb*)hbeeb;
	return beeb->isCapsLockLEDon();
}
*/


void resetsound();

void Beeb::reset() {
	disc_reset();
	ssd_reset();
	reset6502();
	sysvia.reset();
	uservia.reset();
	reset8271();
	resetsound();
}


void Beeb::breakKey() {
	autoboot = 0;
	resetting = 1;
	reset();
}



uint8_t* Beeb::serialize(bool saving, uint8_t* p) {
	p = cpu.serialize(saving, p);
	p = video.serialize(saving, p);
	p = serialize_sound(saving, p);
	p = sysvia.serialize(saving, p);
	p = uservia.serialize(saving, p);
	p = serialize_ssd(saving, p);
	p = serialize_8271(saving, p);
	p = serialize_disc(saving, p);
	return p;
}





bool Beeb::isCapsLockLEDon() {
	return !(sysvia.IC32 & 0x40);
}
bool Beeb::isShiftLockLEDon() {
	return !(sysvia.IC32 & 0x80);
}



void Beeb::setVideoBitmapParams(uint8_t* bitmapPixels, int bitmapStride) {
    video.bitmapPixels = bitmapPixels;
    video.bitmapStride = bitmapStride;
}

