//
//  video.hpp
//  libbeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef video_h
#define video_h

#include <stdint.h>
#include "libbeeb.h"

#ifdef __LP64__
typedef uint64_t uintN_t;
#else
typedef uint32_t uintN_t;
#endif


#ifdef USE_32BPP
typedef uint32_t uintBPP_t;
#else
typedef uint16_t uintBPP_t;
#endif
class BBCGlyphSet;


class Video {
public:

    // The 6845 CRT controller register block
#pragma pack(1)
    union {
	uint8_t raw[32];                //  #    Mode 7       AUG remark
    struct {                        // ==    ======       ==========
        uint8_t horzTotal;          // R0     63          Total # of displayed + non-displayed character time units across the screen minus one
        uint8_t horzDisplayed;      // R1     40          Number of displayed characters per horizontal line
        uint8_t horzSyncPos;        // R2     51          Horizontal sync. pulse position in character widths from left side of the screen
        uint8_t horzPulseWidth:4;   // R3      4          Horizontal sync. pulse width in character widths
        uint8_t vertPulseWidth:4;   //         2          Number of scan line times for the vertical sync. pulse
        uint8_t vertTotal;          // R4     30          Number of character lines minus one
        uint8_t vertTotalAdj;       // R5      2          Number of scan lines left added to vertTotal*charHeight to get to the right refresh rate
        uint8_t vertDisplayed;      // R6     25          Number of displayed character rows
        uint8_t vertSyncPos;        // R7     28          Vertical sync position wrt reference, in character row times.
        uint8_t interlaceMode:4;    // R8
        uint8_t displayBlankingDelay:2; //                0=no delay, 1=One char delay, 2=Two char delay, 3=disable video output
        uint8_t cursorBlankingDelay:2;
        uint8_t scanLinesPerChar;   // R9     18          Number of scan lines per character row including spacing, minus one
        uint8_t cursorStartRaster;  // R10
        uint8_t cursorEndRaster;    // R11
        uint8_t displayStartAddrHi; // R12
        uint8_t displayStartAddrLo; // R13
        uint8_t cursorAddressHi;    // R14
        uint8_t cursorAddressLo;    // R15
        uint8_t lightPenAddressHi;  // R16
        uint8_t lightPenAddressLo;  // R17
    };
    } crtc;
    
    // Video ULA control register
    union {
    uint8_t raw;
    struct {
        uint8_t selectedFlashColour:1;
        uint8_t teletextMode:1;
        uint8_t charsPerLine:2; // 0=10, 1=20, 2=40, 3=80
        uint8_t isHighFreqClock:1;
        uint8_t cursorWidth:2; // 0=1, 1=?, 2=2, 3=4  (bytes)
        uint8_t largeCursor:1;
    };
    } ulactrl;
#pragma pack()

    // CRTC data
    uint32_t frameCount;
    uint32_t clocks;
	uint16_t horzCounter;
    uint16_t vertCounter;
    uint16_t addr;
    uint16_t addrLine;
    uint16_t nextLineStartAddr;
    uint16_t lineStartAddr;
    uint16_t bitmapX;
    uint16_t bitmapY;
	uint8_t crtci;
    uint8_t dispEnabled;
    uint8_t hpulseCounter;
    uint8_t vpulseCounter;
    uint8_t scanlineCounter;
    uint8_t cursorDrawIndex;
    bool oddFrame;
    bool inHSync;
    bool inVSync;
    bool inVertAdjust;
    bool cursorOn;
    bool cursorOff;
    bool cursorOnThisFrame;
    bool drawHalfScanline;
	uintBPP_t ulapal[16];
	uint8_t bakpal[16];

    // Teletext data
    uint8_t mode7dataQueue[4];
	uint8_t mode7col, mode7prevCol;
    uint8_t mode7bg;
    uint8_t mode7flashTime;
    uint8_t mode7heldChar;
    BBCGlyphSet* mode7nextGlyphs;
    BBCGlyphSet* mode7heldGlyphs;
    BBCGlyphSet* mode7curGlyphs;
    bool mode7holdChar;
    bool mode7holdClear;
    bool mode7holdOff;
    bool mode7flash;
    bool mode7gfx;
    bool mode7sep;
    bool mode7dbl, mode7oldDbl;
    bool mode7wasDbl;
    bool mode7secondHalfOfDouble;
    bool mode7flashOn;

	// Bitmap
	int bitmapWidth;
	int bitmapHeight;
	int bitmapStride;
	uint8_t* bitmapPixels;
    
    uint8_t dirtyFrameCount;
    int16_t viewWidth;
    int16_t viewHeight;
    int16_t usedLeft, usedLeftPrev;
    int16_t usedTop, usedTopPrev;
    int16_t usedRight, usedRightPrev;
    int16_t usedBottom, usedBottomPrev;
    int16_t highRenderY;
	
	Video();
	void pollvideo(int clocks);
    void endOfLine();
    void endOfFrame();
    void teletextRender(int offset, uint8_t scanline);
    void teletextSetNextChars();
	void paint();
	void resetcrtc();
	void writecrtc(uint16_t addr, uint8_t val);
	uint8_t readcrtc(uint16_t addr);
	void writeula(uint16_t addr, uint8_t val);
	void renderMode7charline(BBCGlyphSet* tt, uint8_t dat, int x, int row, uint8_t bg, uint8_t fg);
	void renderColour(int offset, uintN_t col);
    void setViewSize(int width, int height);
	uint8_t* serialize(bool saving, uint8_t* p);

};

#endif /* video_h */
