//
//  via.hpp
//  libbeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef via_hpp
#define via_hpp

#include <stdio.h>
#include <stdint.h>


class VIA {
public:
	uint8_t ora,orb,ira,irb;
	uint8_t ddra,ddrb;
	uint8_t sr;
	uint32_t t1l,t2l;
	int t1c,t2c;
	uint8_t acr,pcr,ifr,ier;
	int t1hit,t2hit;
	uint8_t porta,portb;
	int ca1,ca2;
	int cb1,cb2;
	int justhit;
	int irq;
	
	VIA(int irq);
	void reset();
	void polltime(int cycles);
	void updateIFR();
	void write(uint16_t addr, uint8_t val);
	uint8_t read(uint16_t addr);
	void setca1(bool val);
	void setca2(bool val);
	void setcb1(bool val);
	void setcb2(bool val);
	
	virtual void writePortA(uint8_t val) = 0;
	virtual void writePortB(uint8_t val) = 0;
	virtual uint8_t readPortA() = 0;
	virtual uint8_t readPortB() = 0;

	
	uint8_t* serialize(bool saving, uint8_t* p);

};


#define TIMER1INT 0x40
#define TIMER2INT 0x20
#define PORTBINT  0x18
#define PORTAINT  0x03

#define		ORB     0x00
#define		ORA		0x01
#define		DDRB	0x02
#define		DDRA	0x03
#define		T1CL	0x04
#define		T1CH	0x05
#define		T1LL	0x06
#define		T1LH	0x07
#define		T2CL	0x08
#define		T2CH	0x09
#define		SR	0x0a
#define		ACR	0x0b
#define		PCR	0x0c
#define		IFR	0x0d
#define		IER	0x0e
#define		ORAnh   0x0f

#define INT_CA1 0x02
#define INT_CA2 0x01
#define INT_CB1 0x10
#define INT_CB2 0x08


#endif /* via_hpp */
