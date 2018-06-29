#ifndef acia_hpp
#define acia_hpp

#include <stdio.h>

class ACIA {
public:
	int acia_tapespeed;
	uint8_t acia_sr;
	uint8_t acia_cr;
	uint8_t acia_dr;

	void reset();
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t val);
	void poll();
	void receive(uint8_t val);
	//void acia_savestate(FILE *f);
	//void acia_loadstate(FILE *f);
	void dcd();
	void dcdlow();
	void updateint();

};

#endif
