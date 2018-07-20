//
//  BeebKey.h
//  libbeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef BEEBKEY_H_INCLUDED
#define BEEBKEY_H_INCLUDED


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A BeebKey corresponds to one physical key on the BBC keyboard.
 */

typedef struct {
    int32_t scancode;
    char const* name;
    char const* mainLabel;
    char const* shiftLabel;
} BeebKey;

void BeebKey_init();
int BeebKey_scancodeForChar(unsigned short chr);
BeebKey* BeebKey_keyByName(char const* name);
int BeebKey_scancodeForUsbHIDKeycode(int hidKeycode);
int BeebKey_scancodeForMacVKeycode(int keycode);

extern BeebKey beebKeyNone;
extern BeebKey beebKeyF0;
extern BeebKey beebKeyF1;
extern BeebKey beebKeyF2;
extern BeebKey beebKeyF3;
extern BeebKey beebKeyF4;
extern BeebKey beebKeyF5;
extern BeebKey beebKeyF6;
extern BeebKey beebKeyF7;
extern BeebKey beebKeyF8;
extern BeebKey beebKeyF9;
extern BeebKey beebKeyBreak;

extern BeebKey beebKeyEscape;
extern BeebKey beebKey1;
extern BeebKey beebKey2;
extern BeebKey beebKey3;
extern BeebKey beebKey4;
extern BeebKey beebKey5;
extern BeebKey beebKey6;
extern BeebKey beebKey7;
extern BeebKey beebKey8;
extern BeebKey beebKey9;
extern BeebKey beebKey0;
extern BeebKey beebKeyMinus;
extern BeebKey beebKeyCaret;
extern BeebKey beebKeyBackslash;
extern BeebKey beebKeyLeft;
extern BeebKey beebKeyRight;

extern BeebKey beebKeyTab;
extern BeebKey beebKeyQ;
extern BeebKey beebKeyW;
extern BeebKey beebKeyE;
extern BeebKey beebKeyR;
extern BeebKey beebKeyT;
extern BeebKey beebKeyY;
extern BeebKey beebKeyU;
extern BeebKey beebKeyI;
extern BeebKey beebKeyO;
extern BeebKey beebKeyP;
extern BeebKey beebKeyAt;
extern BeebKey beebKeyBracketLeft;
extern BeebKey beebKeyUnderscore;
extern BeebKey beebKeyUp;
extern BeebKey beebKeyDown;

extern BeebKey beebKeyCaps;
extern BeebKey beebKeyCtrl;
extern BeebKey beebKeyA;
extern BeebKey beebKeyS;
extern BeebKey beebKeyD;
extern BeebKey beebKeyF;
extern BeebKey beebKeyG;
extern BeebKey beebKeyH;
extern BeebKey beebKeyJ;
extern BeebKey beebKeyK;
extern BeebKey beebKeyL;
extern BeebKey beebKeySemicolon;
extern BeebKey beebKeyColon;
extern BeebKey beebKeyBracketRight;
extern BeebKey beebKeyReturn;

extern BeebKey beebKeyShiftLock;
extern BeebKey beebKeyShift;
extern BeebKey beebKeyZ;
extern BeebKey beebKeyX;
extern BeebKey beebKeyC;
extern BeebKey beebKeyV;
extern BeebKey beebKeyB;
extern BeebKey beebKeyN;
extern BeebKey beebKeyM;
extern BeebKey beebKeyComma;
extern BeebKey beebKeyPeriod;
extern BeebKey beebKeySlash;
extern BeebKey beebKeyDelete;
extern BeebKey beebKeyCopy;

extern BeebKey beebKeySpace;



typedef enum {
	/*
	 * BBC Model B Keyboard : taken from 	http://www.flickr.com/photos/39013214@N03/5660684665/sizes/o/in/photostream/
	 
	 0    1    2    3    4    5    6   7    8    9
	 ----------------------------------------------
	 0x70     ESC  F1   F2   F3   F5   F6   F8   F9   \|  right
	 0x10     Q    3    4    5    F4   8    F7   -=   ^~  left
	 0x20     F0   W    E    T    7    I    9    0    _£  down
	 0x30     1    2    D    R    6    U    O    P    [(  up
	 0x40     CAP  A    X    F    Y    J    K    @    :*  RET
	 0x50     SLC  S    C    G    H    N    L    ;+   ])  DEL
	 0x60     TAB  Z    SPC  V    B    M    ,<   .>   /?  CPY
	 */
	
	ScanCode_Break = 0xaa,
	//  0x00
	ScanCode_Shift = 0x100,
	ScanCode_Ctrl = 0x01,
	// 	0x10     Q    3    4    5    F4   8    F7   -=   ^~
	ScanCode_Q = 0x10,
	ScanCode_3 = 0x11,
	ScanCode_4 = 0x12,
	ScanCode_Dollar = 0x112,
	ScanCode_5 = 0x13,
	ScanCode_Percent = 0x113,
	ScanCode_F4 = 0x14,
	ScanCode_8 = 0x15,
	ScanCode_BracketLeft = 0x115,
	ScanCode_F7 = 0x16,
	ScanCode_Minus = 0x17,
	ScanCode_Equals = 0x117,
	ScanCode_Tilde = 0x118,
	ScanCode_Caret = 0x18,
	ScanCode_Left = 0x19,
	// 0x20     F0   W    E    T    7    I    9    0  _
	ScanCode_F0 = 0x20,
	ScanCode_W = 0x21,
	ScanCode_E = 0x22,
	ScanCode_T = 0x23,
	ScanCode_7 = 0x24,
	ScanCode_Apostrophe = 0x124,
	ScanCode_I = 0x25,
	ScanCode_9 = 0x26,
	ScanCode_BracketRight = 0x126,
	ScanCode_0 = 0x27,
	ScanCode_Underscore = 0x28,
	ScanCode_Pound = 0x128,
	ScanCode_Down = 0x29,
	// 0x30     1    2    O    R    6   U    O    P    [(
	ScanCode_1 = 0x30,
	ScanCode_ExclamationMark = 0x130,
	ScanCode_2 = 0x31,
	ScanCode_DoubleQuotes = 0x131,
	ScanCode_D = 0x32,
	ScanCode_R = 0x33,
	ScanCode_6 = 0x34,
	ScanCode_Ampersand = 0x134,
	ScanCode_U = 0x35,
	ScanCode_O = 0x36,
	ScanCode_P = 0x37,
	ScanCode_OneQuarter = 0x138,
	ScanCode_ArrowLeft = 0x38,
	ScanCode_Up = 0x39,
	// 0x40     CAP  A    X    F    Y    J    K    @    :*
	ScanCode_Caps = 0x40,
	ScanCode_A = 0x41,
	ScanCode_X = 0x42,
	ScanCode_F = 0x43,
	ScanCode_Y = 0x44,
	ScanCode_J = 0x45,
	ScanCode_K = 0x46,
	ScanCode_At = 0x47,
	ScanCode_Colon = 0x48,
	ScanCode_Star = 0x148,
	ScanCode_Return = 0x49,
	// 	0x50     SLC  S    C    G    H    N    L    ,+   ])
	ScanCode_ShiftLock = 0x50,
	ScanCode_S = 0x51,
	ScanCode_C = 0x52,
	ScanCode_G = 0x53,
	ScanCode_H = 0x54,
	ScanCode_N = 0x55,
	ScanCode_L = 0x56,
	ScanCode_Semicolon = 0x57,
	ScanCode_Plus = 0x157,
	ScanCode_ArrowRight = 0x58,
	ScanCode_Delete = 0x59,
	// 0x60     TAB  Z    SPC  V    B    M    ,<   .>   /?
	ScanCode_Tab = 0x60,
	ScanCode_Z = 0x61,
	ScanCode_Space = 0x62,
	ScanCode_V = 0x63,
	ScanCode_B = 0x64,
	ScanCode_M = 0x65,
	ScanCode_Comma = 0x66,
	ScanCode_LessThan = 0x166,
	ScanCode_Period = 0x67,
	ScanCode_MoreThan = 0x167,
	ScanCode_Slash = 0x68,
	ScanCode_QuestionMark = 0x168,
	ScanCode_Copy = 0x69,
	// 0x70     ESC  F1   F2   F3   F5   F6   F8   F9   \|
	ScanCode_Escape = 0x70,
	ScanCode_F1 = 0x71,
	ScanCode_F2 = 0x72,
	ScanCode_F3 = 0x73,
	ScanCode_F5 = 0x74,
	ScanCode_F6 = 0x75,
	ScanCode_F8 = 0x76,
	ScanCode_F9 = 0x77,
	ScanCode_Backslash = 0x78,
	ScanCode_Pipe = 0x178,
	ScanCode_Right = 0x79,
} BeebScanCode;

#ifdef __cplusplus
}
#endif

#endif // BEEBKEY_H_INCLUDED
