/*B-em v2.1 by Tom Walker
  ADC emulation*/

#include "libbeeb.h"
#include "beeb.h"

int joy1x,joy1y,joy2x,joy2y;

uint8_t adcstatus,adchigh,adclow,adclatch;
int adctime;


void initadc()
{
	adcstatus=0x40;            /*Not busy, conversion complete*/
	adchigh=adclow=adclatch=0;
	adctime=0;
	//rjh install_joystick(JOY_TYPE_AUTODETECT);
}

uint8_t readadc(uint16_t addr)
{
        switch (addr&3)
        {
                case 0:
                return adcstatus;
                break;
                case 1:
                return adchigh;
                break;
                case 2:
                return adclow;
                break;
        }
        return 0x40;
}

void writeadc(uint16_t addr, uint8_t val)
{
	if (!(addr&3)) {
		adctime = (val & 0x08) ? 20000 : 8000;
		adcstatus = (val & 0x0f) | 0x80;
		adclatch=val;
		the_beeb->sysvia.setcb1(true);
	}
}

void polladc(int cycles) {
	if (!adctime) return;
	adctime -= cycles;
	if (adctime > 0) return;
	adctime = 0;
	int val = 0x0000;
	// TODO: switch on bottom two bits of adc_status and pick a value corresponding
	// to the appropriate axis.
	adcstatus = (adcstatus & 0x0f) | 0x40 | ((val >> 10) & 0x03);
	adclow = val & 0xff;
	adchigh = (val >> 8) & 0xff;
	the_beeb->sysvia.setcb1(false);
	
}



void saveadcstate(FILE *f)
{
        putc(adcstatus,f);
        putc(adclow,f);
        putc(adchigh,f);
        putc(adclatch,f);
        putc(adctime,f);
}

void loadadcstate(FILE *f)
{
        adcstatus=getc(f);
        adclow=getc(f);
        adchigh=getc(f);
        adclatch=getc(f);
        adctime=getc(f);
}
