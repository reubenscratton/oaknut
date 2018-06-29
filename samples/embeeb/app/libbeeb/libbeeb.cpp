/*
	libbeeb.cpp - entry points for libbeeb

*/

#include "libbeeb.h"
#include "beeb.h"

int joybutton[2];

DRIVE drives[2];

void (*fdcwriteprotect)();
void (*fdccallback)();
void (*fdcdata)(uint8_t dat);
void (*fdcspindown)();
void (*fdcfinishread)();
void (*fdcnotfound)();
void (*fdcdatacrcerror)();
void (*fdcheadercrcerror)();
int  (*fdcgetdata)(int last);

void loaddisc(int drive, int loader, unsigned char* discimg, int cb);
void dump_pages();



void closebbc() {
    closedisc(0);
    closedisc(1);
}

