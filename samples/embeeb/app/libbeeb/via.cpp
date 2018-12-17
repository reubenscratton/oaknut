//
//  via.cpp
//  libbeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "via.hpp"
#include "libbeeb.h"
#include "beeb.h"

VIA::VIA(int irq) {
	this->irq = irq;
}
void VIA::reset() {
	ora = orb = 0xff;
	ddra = ddrb = 0xff;
	ifr = ier = 0x00;
	t1c = t1l = t2c = t2l = 0x1fffe;
	t1hit = t2hit = true;
	acr = pcr = 0;
}


void VIA::polltime(int cycles) {
	justhit = 0;
	int newT1c = t1c - cycles;
	if (newT1c < -2 && t1c > -3) {
		if (!t1hit) {
			ifr |= TIMER1INT;
			updateIFR();
			if (newT1c == -3) {
				justhit |= 1;
			}
			// b-em comment is "Output to PB7"
			orb ^= (acr & 0x80);
		}
		if (!(acr & 0x40)) t1hit = true;
	}
	while (newT1c < -3) newT1c += t1l + 4;
	t1c = newT1c;
	
	if (!(acr & 0x20)) {
		int newT2c = t2c - cycles;
		if (newT2c < -2) {
			if (!t2hit) {
				ifr |= TIMER2INT;
				updateIFR();
				if (newT2c == -3) {
					justhit |= 2;
				}
				t2hit = true;
			}
			newT2c += 0x20000;
		}
		t2c = newT2c;
	}
}

void VIA::updateIFR() {
	if (ifr & ier & 0x7f) {
		ifr |= 0x80;
		the_beeb->cpu.interrupt |= irq;
	} else {
		ifr &= ~0x80;
		the_beeb->cpu.interrupt &= ~irq;
	}
}


void VIA::write(uint16_t addr, uint8_t val) {
	int mode;
	switch (addr & 0xf) {
		case ORA:
			ifr &= ~INT_CA1;
			if ((pcr & 0x0a) != 0x02) {
				// b-em: Not independent interrupt for CA2
				ifr &= ~INT_CA2;
			}
			updateIFR();
			
			mode = (pcr & 0x0e);
			if (mode == 8) { // Handshake mode
				setca2(false);
			} else if (mode == 0x0a) { // Pulse mode
				setca2(false);
				setca2(true);
			}
			/* falls through */
		case ORAnh:
			ora = val;
			writePortA(((ora & ddra) | ~ddra) & 0xff);
			break;
			
		case ORB:
			ifr &= ~INT_CB1;
			if ((pcr & 0xa0) != 0x20) {
				// b-em: Not independent interrupt for CB2
				ifr &= ~INT_CB2;
			}
			updateIFR();
			
			orb = val;
			writePortB(((orb & ddrb) | ~ddrb) & 0xff);
			
			mode = (pcr & 0xe0) >> 4;
			if (mode == 8) { // Handshake mode
				setcb2(0);
			} else if (mode == 0x0a) { // Pulse mode
				setcb2(0);
				setcb2(1);
			}
			break;
			
		case DDRA:
			ddra = val;
			writePortA(((ora & ddra) | ~ddra) & 0xff);
			break;
			
		case DDRB:
			ddrb = val;
			writePortB(((orb & ddrb) | ~ddrb) & 0xff);
			break;
			
		case ACR:
			acr = val;
			break;
			
		case PCR:
			pcr = val;
			if ((val & 0xe) == 0xc) setca2(false);
			else if (val & 0x08) setca2(true);
			if ((val & 0xe0) == 0xc0) setcb2(0);
			else if (val & 0x80) setcb2(1);
			break;
			
		case SR:
			sr = val;
			break;
			
		case T1LL:
		case T1CL:
			t1l &= 0x1fe00;
			t1l |= (val << 1);
			break;
			
		case T1LH:
			t1l &= 0x1fe;
			t1l |= (val << 9);
			if (acr & 0x40) {
				ifr &= ~TIMER1INT;
				updateIFR();
			}
			break;
			
		case T1CH:
			if ((acr & 0xc0) == 0x80) orb &= ~0x80; // One-shot timer
			t1l &= 0x1fe;
			t1l |= (val << 9);
			t1c = t1l + 1;
			t1hit = false;
			ifr &= ~TIMER1INT;
			updateIFR();
			break;
			
		case T2CL:
			t2l &= 0x1fe00;
			t2l |= (val << 1);
			break;
			
		case T2CH:
			t2l &= 0x1fe;
			t2l |= (val << 9);
			t2c = t2l + 1;
			ifr &= ~TIMER2INT;
			updateIFR();
			t2hit = false;
			break;
			
		case IER:
			if (val & 0x80)
				ier |= (val & 0x7f);
			else
				ier &= ~(val & 0x7f);
			updateIFR();
			break;
			
		case IFR:
			ifr &= ~(val & 0x7f);
			updateIFR();
			break;
	}
}

uint8_t VIA::read(uint16_t addr) {
	uint8_t temp;
	switch (addr & 0xf) {
		case ORA:
			ifr &= ~INT_CA1;
			if ((pcr & 0xa) != 0x2)
				ifr &= ~INT_CA2;
			updateIFR();
			/* falls through */
		case ORAnh:
			temp = ora & ddra;
			if (acr & 1)
				return temp | (ira & ~ddra);
			else
				return temp | (readPortA() & ~ddra);
			break;
			
		case ORB:
			ifr &= ~INT_CB1;
			if ((pcr & 0xa0) != 0x20)
				ifr &= ~INT_CB2;
			updateIFR();
			
			temp = orb & ddrb;
			if (acr & 2)
				return temp | (irb & ~ddrb);
			else
				return temp | (readPortB() & ~ddrb);
			break;
			
		case DDRA:
			return ddra;
		case DDRB:
			return ddrb;
		case T1LL:
			return ((t1l & 0x1fe) >> 1) & 0xff;
		case T1LH:
			return (t1l >> 9) & 0xff;
			
		case T1CL:
			if (!(justhit & 1)) {
				ifr &= ~TIMER1INT;
				updateIFR();
			}
			return ((t1c + 1) >> 1) & 0xff;
			
		case T1CH:
			return ((t1c + 1) >> 9) & 0xff;
			
		case T2CL:
			if (!(justhit & 2)) {
				ifr &= ~TIMER2INT;
				updateIFR();
			}
			return ((t2c + 1) >> 1) & 0xff;
			
		case T2CH:
			return ((t2c + 1) >> 9) & 0xff;
			
		case SR:
			return sr;
		case ACR:
			return acr;
		case PCR:
			return pcr;
		case IER:
			return ier | 0x80;
		case IFR:
			return ifr;
		default:
			bbcLogE("Unknown VIA read");
            return 0;
	}
}

void VIA::setca1(bool val) {
	bool level = val;
	if (level == ca1) return;
	bool pcrSet = (pcr & 1);
	if (pcrSet == level) {
		if (acr & 1) ira = readPortA();
		ifr |= INT_CA1;
		updateIFR();
		if ((pcr & 0xc) == 0x8) { // handshaking
			setca2(1);
		}
	}
	ca1 = level;
}

void VIA::setca2(bool val) {
	bool level = val;
	if (level == ca2) return;
	if (pcr & 8) return; // output
	bool pcrSet = (pcr & 4);
	if (pcrSet == level) {
		ifr |= INT_CA2;
		updateIFR();
	}
	ca2 = level;
}

void VIA::setcb1(bool val) {
	bool level = val;
	if (level == cb1) return;
	bool pcrSet = (pcr & 0x10);
	if (pcrSet == level) {
		if (acr & 2) irb = readPortB();
		ifr |= INT_CB1;
		updateIFR();
		if ((pcr & 0xc0) == 0x80) { // handshaking
			setcb2(1);
		}
	}
	cb1 = level;
}

void VIA::setcb2(bool val) {
	bool level = val;
	if (level == cb2) return;
	if (pcr & 0x80) return; // output
	bool pcrSet = (pcr & 0x40);
	if (pcrSet == level) {
		ifr |= INT_CB2;
		updateIFR();
	}
	cb2 = level;
}


uint8_t* VIA::serialize(bool saving, uint8_t* p) {
	SERIALIZE(ora);
	SERIALIZE(orb);
	SERIALIZE(ira);
	SERIALIZE(irb);
	SERIALIZE(ddra);
	SERIALIZE(ddrb);
	SERIALIZE(sr);
	SERIALIZE(t1l);
	SERIALIZE(t2l);
	SERIALIZE(t1c);
	SERIALIZE(t2c);
	SERIALIZE(acr);
	SERIALIZE(pcr);
	SERIALIZE(ifr);
	SERIALIZE(ier);
	SERIALIZE(t1hit);
	SERIALIZE(t2hit);
	SERIALIZE(porta);
	SERIALIZE(portb);
	SERIALIZE(ca1);
	SERIALIZE(ca2);
	return p;
}

