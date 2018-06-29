/*B-em v2.2 by Tom Walker
  6850 ACIA emulation*/

#include "libbeeb.h"
#include "beeb.h"
#include "acia.h"
//#include "serial.h"
//#include "csw.h"
//#include "uef.h"
//#include "tapenoise.h"

#define DCD     4
#define RECIEVE 1

static uint16_t newdat;
extern int ueftoneon,cswtoneon;


void ACIA::updateint() {
	if ((acia_sr&0x80) && (acia_cr&0x80))
	   the_beeb->cpu.interrupt|=4;
	else
	   the_beeb->cpu.interrupt&=~4;
}

void ACIA::reset() {
        acia_sr = (acia_sr & 8) | 4;
        updateint();
}

uint8_t ACIA::read(uint16_t addr) {
	uint8_t temp;
	if (addr & 1) {
		temp = acia_dr;
		acia_sr &= ~0x81;
		updateint();
		return temp;
	}
	else {
		return (acia_sr & 0x7F) | (acia_sr & acia_cr & 0x80) | 2;
	}
}

void ACIA::write(uint16_t addr, uint8_t val) {
	if (addr & 1) {
		acia_sr &= 0xFD;
		updateint();
	}
	else {
		acia_cr = val;
		if (val == 3)
		   reset();
		switch (val & 3) {
				case 1: acia_tapespeed=0; break;
				case 2: acia_tapespeed=1; break;
		}
	}
}

void ACIA::dcd() {
        if (acia_sr & DCD) return;
        acia_sr |= DCD | 0x80;
        updateint();
}

void ACIA::dcdlow() {
        acia_sr &= ~DCD;
        updateint();
}


void ACIA::receive(uint8_t val) /*Called when the acia recives some data*/
{
        acia_dr = val;
        acia_sr |= RECIEVE | 0x80;
        updateint();
        newdat=val|0x100;
}

/*Every 128 clocks, ie 15.625khz*/
/*Div by 13 gives roughly 1200hz*/

void ACIA::poll() {
	/*if (motor) {
			startblit();
			if (csw_ena) csw_poll();
			else         uef_poll();
			endblit();

			if (newdat&0x100)
			{
					newdat&=0xFF;
					tapenoise_adddat(newdat);
			}
			else if (csw_toneon || uef_toneon) tapenoise_addhigh();
	}*/
}

/*
void acia_savestate(FILE *f) {
	putc(acia_cr,f);
	putc(acia_sr,f);
}

void acia_loadstate(FILE *f) {
	acia_cr=getc(f);
	acia_sr=getc(f);
}*/
