/*
 * main.h
 *
 *
 * Written by Reuben Scratton, based on original code by Tom Walker
 *
 */
#pragma once
#include <oaknut.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 32bpp is actually slower on an iPhone 6S!
//#ifndef EMSCRIPTEN // Emscripten is faster at 16bpp
#define USE_32BPP // Faster on iPod, emulator...
//#endif

#define FLAG_I8271		1
#define FLAG_WD1770		2
#define FLAG_x65c02		4
#define FLAG_BPLUS		8
#define FLAG_MASTER		16
#define FLAG_SWRAM		32
#define FLAG_MODELA		64
#define FLAG_OS01		128
#define FLAG_COMPACT	256


//extern int swram[16];
extern int motorspin;
extern int fdctime;
extern int motoron;
extern int disctime;
extern int frames;

#define SAVE(x) memcpy(p, &x, sizeof(x)); p+=sizeof(x)
#define LOAD(x) memcpy(&x, p, sizeof(x)); p+=sizeof(x)
#define SERIALIZE(x) if (saving) {SAVE(x);} else {LOAD(x);}

#define BORDER_LEFT (10*16)
#define BORDER_RIGHT (6*16)
#define BORDER_TOP 12
#define BORDER_BOTTOM 13

#define SURFACE_WIDTH  1024
#define SURFACE_HEIGHT 625

#define DISPLAY_ASPECT ((SURFACE_HEIGHT-(BORDER_TOP+BORDER_BOTTOM)) / (float)(SURFACE_WIDTH-(BORDER_LEFT+BORDER_RIGHT)))


#ifndef PCALLBACK
typedef void(*PCALLBACK)(const void*);
typedef void(*PCALLBACK2)(const void*, void*);
#endif

#define bbcLogE app->log
#define bbcLogI app->log

#ifdef __cplusplus
extern "C" {
#endif


void writesound(uint8_t data);
void logvols();
void polladc(int clocks);
void closedisc(int drive);

uint8_t* serialize_sound(bool saving, uint8_t* p);
uint8_t* serialize_ssd(bool saving, uint8_t* p);
uint8_t* serialize_8271(bool saving, uint8_t* p);
uint8_t* serialize_disc(bool saving, uint8_t* p);



extern void (*fdcwriteprotect)();
extern void (*fdccallback)();
extern void (*fdcdata)(uint8_t dat);
extern void (*fdcspindown)();
extern void (*fdcfinishread)();
extern void (*fdcnotfound)();
extern void (*fdcdatacrcerror)();
extern void (*fdcheadercrcerror)();
extern int  (*fdcgetdata)(int last);

extern int writeprot[2],fwriteprot[2];



void ssd_reset();
void ssd_load(int drive, unsigned char *img, int cb);
void ssd_close(int drive);
void dsd_load(int drive, unsigned char *img, int cb);
void ssd_seek(int drive, int track);
void ssd_readsector(int drive, int sector, int track, int side, int density);
void ssd_writesector(int drive, int sector, int track, int side, int density);
void ssd_readaddress(int drive, int sector, int side, int density);
void ssd_format(int drive, int sector, int side, int density);
void ssd_poll();

void disc_reset();
void disc_poll();
void disc_seek(int drive, int track);
void disc_readsector(int drive, int sector, int track, int side, int density);
void disc_writesector(int drive, int sector, int track, int side, int density);
void disc_readaddress(int drive, int track, int side, int density);
void disc_format(int drive, int track, int side, int density);

void reset8271();
uint8_t read8271(uint16_t addr);
void write8271(uint16_t addr, uint8_t val);


typedef struct {
        void (*seek)(int drive, int track);
        void (*readsector)(int drive, int sector, int track, int side, int density);
        void (*writesector)(int drive, int sector, int track, int side, int density);
        void (*readaddress)(int drive, int track, int side, int density);
        void (*format)(int drive, int track, int side, int density);
        void (*poll)();
} DRIVE;

extern DRIVE drives[2];
extern int curdrive;
extern int joybutton[2];

void initsound();

void initadc();
uint8_t readadc(uint16_t addr);
void writeadc(uint16_t addr, uint8_t val);

extern int defaultwriteprot;

extern int idecallback;

	
#ifdef __cplusplus
}
#endif

