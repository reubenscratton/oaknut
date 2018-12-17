//
//  BeebKey.cpp
//  libbeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "beebkey.h"
#include <map>
#include <vector>
#include <cstring>

extern "C" {


BeebKey beebKeyNone = {0, "<None>"};
BeebKey beebKeyF0 = {ScanCode_F0, "F0", "f0"};
BeebKey beebKeyF1 = {ScanCode_F1, "F1", "f1"};
BeebKey beebKeyF2 = {ScanCode_F2, "F2", "f2"};
BeebKey beebKeyF3 = {ScanCode_F3, "F3", "f3"};
BeebKey beebKeyF4 = {ScanCode_F4, "F4", "f4"};
BeebKey beebKeyF5 = {ScanCode_F5, "F5", "f5"};
BeebKey beebKeyF6 = {ScanCode_F6, "F6", "f6"};
BeebKey beebKeyF7 = {ScanCode_F7, "F7", "f7"};
BeebKey beebKeyF8 = {ScanCode_F8, "F8", "f8"};
BeebKey beebKeyF9 = {ScanCode_F9, "F9", "f9"};
BeebKey beebKeyBreak = {ScanCode_Break, "BREAK", "BRK"};
	
BeebKey beebKeyEscape = {ScanCode_Escape, "ESC"};
BeebKey beebKey1 = {ScanCode_1, "1", "1", "!"};
BeebKey beebKey2 = {ScanCode_2, "2", "2", "\""};
BeebKey beebKey3 = {ScanCode_3, "3", "3", "#"};
BeebKey beebKey4 = {ScanCode_4, "4", "4", "$"};
BeebKey beebKey5 = {ScanCode_5, "5", "5", "%"};
BeebKey beebKey6 = {ScanCode_6, "6", "6", "&"};
BeebKey beebKey7 = {ScanCode_7, "7", "7", "'"};
BeebKey beebKey8 = {ScanCode_8, "8", "8", "("};
BeebKey beebKey9 = {ScanCode_9, "9", "9", ")"};
BeebKey beebKey0 = {ScanCode_0, "0"};
BeebKey beebKeyMinus = {ScanCode_Minus, "-", "-", "="};
BeebKey beebKeyCaret = {ScanCode_Caret, "^", "^", "~"};
BeebKey beebKeyBackslash = {ScanCode_Backslash, "\\", "\\", "|"};
BeebKey beebKeyLeft = {ScanCode_Left, "Left", "\u2190"};
BeebKey beebKeyRight = {ScanCode_Right, "Right", "\u2192"};

BeebKey beebKeyTab = {ScanCode_Tab, "TAB"};
BeebKey beebKeyQ = {ScanCode_Q, "Q"};
BeebKey beebKeyW = {ScanCode_W, "W"};
BeebKey beebKeyE = {ScanCode_E, "E"};
BeebKey beebKeyR = {ScanCode_R, "R"};
BeebKey beebKeyT = {ScanCode_T, "T"};
BeebKey beebKeyY = {ScanCode_Y, "Y"};
BeebKey beebKeyU = {ScanCode_U, "U"};
BeebKey beebKeyI = {ScanCode_I, "I"};
BeebKey beebKeyO = {ScanCode_O, "O"};
BeebKey beebKeyP = {ScanCode_P, "P"};
BeebKey beebKeyAt = {ScanCode_At, "@"};
BeebKey beebKeyBracketLeft = {ScanCode_BracketLeft, "[", "[", "{"};
BeebKey beebKeyUnderscore = {ScanCode_Underscore, "_", "_", "\u00a3"};
BeebKey beebKeyUp = {ScanCode_Up, "Up", "\u2191"};
BeebKey beebKeyDown = {ScanCode_Down, "Down", "\u2193"};

BeebKey beebKeyCaps = {ScanCode_Caps, "CAP"};
BeebKey beebKeyCtrl = {ScanCode_Ctrl, "CTRL", "CTL"};
BeebKey beebKeyA = {ScanCode_A, "A"};
BeebKey beebKeyS = {ScanCode_S, "S"};
BeebKey beebKeyD = {ScanCode_D, "D"};
BeebKey beebKeyF = {ScanCode_F, "F"};
BeebKey beebKeyG = {ScanCode_G, "G"};
BeebKey beebKeyH = {ScanCode_H, "H"};
BeebKey beebKeyJ = {ScanCode_J, "J"};
BeebKey beebKeyK = {ScanCode_K, "K"};
BeebKey beebKeyL = {ScanCode_L, "L"};
BeebKey beebKeySemicolon = {ScanCode_Semicolon, ";", ";", "+"};
BeebKey beebKeyColon = {ScanCode_Colon, ":", ":", "*"};
BeebKey beebKeyBracketRight = {ScanCode_ArrowRight, "]", "]", "}"};
BeebKey beebKeyReturn = {ScanCode_Return, "RETURN"};

BeebKey beebKeyShiftLock = {ScanCode_ShiftLock, "SHIFT LOCK", "SHL"};
BeebKey beebKeyShift = {ScanCode_Shift, "SHIFT"};
BeebKey beebKeyZ = {ScanCode_Z, "Z"};
BeebKey beebKeyX = {ScanCode_X, "X"};
BeebKey beebKeyC = {ScanCode_C, "C"};
BeebKey beebKeyV = {ScanCode_V, "V"};
BeebKey beebKeyB = {ScanCode_B, "B"};
BeebKey beebKeyN = {ScanCode_N, "N"};
BeebKey beebKeyM = {ScanCode_M, "M"};
BeebKey beebKeyComma = {ScanCode_Comma, ",", ",", "<"};
BeebKey beebKeyPeriod = {ScanCode_Period, ".", ".", ">"};
BeebKey beebKeySlash = {ScanCode_Slash, "/", "/", "?"};
BeebKey beebKeyDelete = {ScanCode_Delete, "DEL"};
BeebKey beebKeyCopy = {ScanCode_Copy, "COPY", "CPY"};

BeebKey beebKeySpace = {ScanCode_Space, "SPACE", " "};
	

static std::vector<BeebKey*> s_bbcKeyList;

// A map of text characters to BBC scan codes
static std::map<unsigned short, uint32_t> s_charsToScancodes;

void BeebKey_init() {

    s_bbcKeyList.push_back(&beebKeyNone);
    s_bbcKeyList.push_back(&beebKeyF0);
    s_bbcKeyList.push_back(&beebKeyF1);
    s_bbcKeyList.push_back(&beebKeyF2);
    s_bbcKeyList.push_back(&beebKeyF3);
    s_bbcKeyList.push_back(&beebKeyF4);
    s_bbcKeyList.push_back(&beebKeyF5);
    s_bbcKeyList.push_back(&beebKeyF6);
    s_bbcKeyList.push_back(&beebKeyF7);
    s_bbcKeyList.push_back(&beebKeyF8);
    s_bbcKeyList.push_back(&beebKeyF9);
    s_bbcKeyList.push_back(&beebKeyBreak);
    s_bbcKeyList.push_back(&beebKeyEscape);
    s_bbcKeyList.push_back(&beebKey1);
    s_bbcKeyList.push_back(&beebKey2);
    s_bbcKeyList.push_back(&beebKey3);
    s_bbcKeyList.push_back(&beebKey4);
    s_bbcKeyList.push_back(&beebKey5);
    s_bbcKeyList.push_back(&beebKey6);
    s_bbcKeyList.push_back(&beebKey7);
    s_bbcKeyList.push_back(&beebKey8);
    s_bbcKeyList.push_back(&beebKey9);
    s_bbcKeyList.push_back(&beebKey0);
    s_bbcKeyList.push_back(&beebKeyMinus);
    s_bbcKeyList.push_back(&beebKeyCaret);
    s_bbcKeyList.push_back(&beebKeyBackslash);
    s_bbcKeyList.push_back(&beebKeyLeft);
    s_bbcKeyList.push_back(&beebKeyRight);
    s_bbcKeyList.push_back(&beebKeyTab);
    s_bbcKeyList.push_back(&beebKeyQ);
    s_bbcKeyList.push_back(&beebKeyW);
    s_bbcKeyList.push_back(&beebKeyE);
    s_bbcKeyList.push_back(&beebKeyR);
    s_bbcKeyList.push_back(&beebKeyT);
    s_bbcKeyList.push_back(&beebKeyY);
    s_bbcKeyList.push_back(&beebKeyU);
    s_bbcKeyList.push_back(&beebKeyI);
    s_bbcKeyList.push_back(&beebKeyO);
    s_bbcKeyList.push_back(&beebKeyP);
    s_bbcKeyList.push_back(&beebKeyAt);
    s_bbcKeyList.push_back(&beebKeyBracketLeft);
    s_bbcKeyList.push_back(&beebKeyUnderscore);
    s_bbcKeyList.push_back(&beebKeyUp);
    s_bbcKeyList.push_back(&beebKeyDown);
    s_bbcKeyList.push_back(&beebKeyCaps);
    s_bbcKeyList.push_back(&beebKeyCtrl);
    s_bbcKeyList.push_back(&beebKeyA);
    s_bbcKeyList.push_back(&beebKeyS);
    s_bbcKeyList.push_back(&beebKeyD);
    s_bbcKeyList.push_back(&beebKeyF);
    s_bbcKeyList.push_back(&beebKeyG);
    s_bbcKeyList.push_back(&beebKeyH);
    s_bbcKeyList.push_back(&beebKeyJ);
    s_bbcKeyList.push_back(&beebKeyK);
    s_bbcKeyList.push_back(&beebKeyL);
    s_bbcKeyList.push_back(&beebKeySemicolon);
    s_bbcKeyList.push_back(&beebKeyColon);
    s_bbcKeyList.push_back(&beebKeyBracketRight);
    s_bbcKeyList.push_back(&beebKeyReturn);
    s_bbcKeyList.push_back(&beebKeyShiftLock);
    s_bbcKeyList.push_back(&beebKeyShift);
    s_bbcKeyList.push_back(&beebKeyZ);
    s_bbcKeyList.push_back(&beebKeyX);
    s_bbcKeyList.push_back(&beebKeyC);
    s_bbcKeyList.push_back(&beebKeyV);
    s_bbcKeyList.push_back(&beebKeyB);
    s_bbcKeyList.push_back(&beebKeyN);
    s_bbcKeyList.push_back(&beebKeyM);
    s_bbcKeyList.push_back(&beebKeyComma);
    s_bbcKeyList.push_back(&beebKeyPeriod);
    s_bbcKeyList.push_back(&beebKeySlash);
    s_bbcKeyList.push_back(&beebKeyDelete);
    s_bbcKeyList.push_back(&beebKeyCopy);
    s_bbcKeyList.push_back(&beebKeySpace);

    s_charsToScancodes['A'] = ScanCode_A|0x100;
    s_charsToScancodes['B'] = ScanCode_B|0x100;
    s_charsToScancodes['C'] = ScanCode_C|0x100;
    s_charsToScancodes['D'] = ScanCode_D|0x100;
    s_charsToScancodes['E'] = ScanCode_E|0x100;
    s_charsToScancodes['F'] = ScanCode_F|0x100;
    s_charsToScancodes['G'] = ScanCode_G|0x100;
    s_charsToScancodes['H'] = ScanCode_H|0x100;
    s_charsToScancodes['I'] = ScanCode_I|0x100;
    s_charsToScancodes['J'] = ScanCode_J|0x100;
    s_charsToScancodes['K'] = ScanCode_K|0x100;
    s_charsToScancodes['L'] = ScanCode_L|0x100;
    s_charsToScancodes['M'] = ScanCode_M|0x100;
    s_charsToScancodes['N'] = ScanCode_N|0x100;
    s_charsToScancodes['O'] = ScanCode_O|0x100;
    s_charsToScancodes['P'] = ScanCode_P|0x100;
    s_charsToScancodes['Q'] = ScanCode_Q|0x100;
    s_charsToScancodes['R'] = ScanCode_R|0x100;
    s_charsToScancodes['S'] = ScanCode_S|0x100;
    s_charsToScancodes['T'] = ScanCode_T|0x100;
    s_charsToScancodes['U'] = ScanCode_U|0x100;
    s_charsToScancodes['V'] = ScanCode_V|0x100;
    s_charsToScancodes['W'] = ScanCode_W|0x100;
    s_charsToScancodes['X'] = ScanCode_X|0x100;
    s_charsToScancodes['Y'] = ScanCode_Y|0x100;
    s_charsToScancodes['Z'] = ScanCode_Z|0x100;
    s_charsToScancodes['a'] = ScanCode_A;
    s_charsToScancodes['b'] = ScanCode_B;
    s_charsToScancodes['c'] = ScanCode_C;
    s_charsToScancodes['d'] = ScanCode_D;
    s_charsToScancodes['e'] = ScanCode_E;
    s_charsToScancodes['f'] = ScanCode_F;
    s_charsToScancodes['g'] = ScanCode_G;
    s_charsToScancodes['h'] = ScanCode_H;
    s_charsToScancodes['i'] = ScanCode_I;
    s_charsToScancodes['j'] = ScanCode_J;
    s_charsToScancodes['k'] = ScanCode_K;
    s_charsToScancodes['l'] = ScanCode_L;
    s_charsToScancodes['m'] = ScanCode_M;
    s_charsToScancodes['n'] = ScanCode_N;
    s_charsToScancodes['o'] = ScanCode_O;
    s_charsToScancodes['p'] = ScanCode_P;
    s_charsToScancodes['q'] = ScanCode_Q;
    s_charsToScancodes['r'] = ScanCode_R;
    s_charsToScancodes['s'] = ScanCode_S;
    s_charsToScancodes['t'] = ScanCode_T;
    s_charsToScancodes['u'] = ScanCode_U;
    s_charsToScancodes['v'] = ScanCode_V;
    s_charsToScancodes['w'] = ScanCode_W;
    s_charsToScancodes['x'] = ScanCode_X;
    s_charsToScancodes['y'] = ScanCode_Y;
    s_charsToScancodes['z'] = ScanCode_Z;
    s_charsToScancodes[' '] = ScanCode_Space;
    s_charsToScancodes['0'] = ScanCode_0;
    s_charsToScancodes['1'] = ScanCode_1;
    s_charsToScancodes['2'] = ScanCode_2;
    s_charsToScancodes['3'] = ScanCode_3;
    s_charsToScancodes['4'] = ScanCode_4;
    s_charsToScancodes['5'] = ScanCode_5;
    s_charsToScancodes['6'] = ScanCode_6;
    s_charsToScancodes['7'] = ScanCode_7;
    s_charsToScancodes['8'] = ScanCode_8;
    s_charsToScancodes['9'] = ScanCode_9;
    s_charsToScancodes['!'] = ScanCode_ExclamationMark;
    s_charsToScancodes['@'] = ScanCode_At;
    s_charsToScancodes['#'] = ScanCode_3|0x100;
    s_charsToScancodes[L'£'] = ScanCode_Pound;
    s_charsToScancodes['&'] = ScanCode_Ampersand;
    s_charsToScancodes['\''] = ScanCode_Apostrophe;
    s_charsToScancodes['\"'] = ScanCode_DoubleQuotes;
    s_charsToScancodes['$'] = ScanCode_Dollar;
    s_charsToScancodes['%'] = ScanCode_Percent;

    s_charsToScancodes['*'] = ScanCode_Star;
    s_charsToScancodes['('] = ScanCode_BracketLeft;
    s_charsToScancodes[')'] = ScanCode_BracketRight;
    s_charsToScancodes['\\'] = ScanCode_Backslash;
    s_charsToScancodes['/'] = ScanCode_Slash;
    s_charsToScancodes['-'] = ScanCode_Minus;
    s_charsToScancodes['+'] = ScanCode_Plus;
    s_charsToScancodes['='] = ScanCode_Equals;
    s_charsToScancodes[':'] = ScanCode_Colon;
    s_charsToScancodes[';'] = ScanCode_Semicolon;
    s_charsToScancodes['?'] = ScanCode_QuestionMark;
    s_charsToScancodes[','] = ScanCode_Comma;
    s_charsToScancodes['_'] = ScanCode_Underscore;
    s_charsToScancodes['['] = ScanCode_ArrowLeft;
    s_charsToScancodes[']'] = ScanCode_ArrowRight;
    s_charsToScancodes['<'] = ScanCode_LessThan;
    s_charsToScancodes['>'] = ScanCode_MoreThan;
    s_charsToScancodes['.'] = ScanCode_Period;
    s_charsToScancodes['~'] = ScanCode_Tilde;
    s_charsToScancodes['|'] = ScanCode_Pipe;
    s_charsToScancodes['\r'] = ScanCode_Return;
    s_charsToScancodes['\n'] = ScanCode_Return;
    s_charsToScancodes[63232] = ScanCode_Up;
    s_charsToScancodes[63233] = ScanCode_Down;
    s_charsToScancodes[63234] = ScanCode_Left;
    s_charsToScancodes[63235] = ScanCode_Right;
    s_charsToScancodes[127] = ScanCode_Delete;
}


BeebKey* BeebKey_keyByName(char const* name) {
    for (std::vector<BeebKey*>::iterator it=s_bbcKeyList.begin() ; it!=s_bbcKeyList.end() ; it++) {
        if (0==strcmp(name, (*it)->name)) {
            return *it;
        }
    }
    return NULL;
}


static int s_macVKeycodesToScancodes[] = {
    ScanCode_A, //    kVK_ANSI_A  = 0x00,
    ScanCode_S, //    kVK_ANSI_S  = 0x01,
    ScanCode_D, //    kVK_ANSI_D  = 0x02,
    ScanCode_F, //    kVK_ANSI_F  = 0x03,
    ScanCode_H, //    kVK_ANSI_H  = 0x04,
    ScanCode_G, //    kVK_ANSI_G  = 0x05,
    ScanCode_Z, //    kVK_ANSI_Z  = 0x06,
    ScanCode_X, //    kVK_ANSI_X  = 0x07,
    ScanCode_C, //    kVK_ANSI_C  = 0x08,
    ScanCode_V, //    kVK_ANSI_V  = 0x09,
    0, //                     ??  = 0x0A,
    ScanCode_B, //    kVK_ANSI_B  = 0x0B,
    ScanCode_Q, //    kVK_ANSI_Q  = 0x0C,
    ScanCode_W, //    kVK_ANSI_W  = 0x0D,
    ScanCode_E, //    kVK_ANSI_E  = 0x0E,
    ScanCode_R, //    kVK_ANSI_R  = 0x0F,
    ScanCode_Y, //    kVK_ANSI_Y  = 0x10,
    ScanCode_T, //    kVK_ANSI_T  = 0x11,
    ScanCode_1, //    kVK_ANSI_1  = 0x12,
    ScanCode_2, //    kVK_ANSI_2  = 0x13,
    ScanCode_3, //    kVK_ANSI_3  = 0x14,
    ScanCode_4, //    kVK_ANSI_4  = 0x15,
    ScanCode_6, //    kVK_ANSI_6  = 0x16,
    ScanCode_5, //    kVK_ANSI_5  = 0x17,
    ScanCode_Equals,//kVK_ANSI_Equal = 0x18,
    ScanCode_9, //    kVK_ANSI_9  = 0x19,
    ScanCode_7, //    kVK_ANSI_7  = 0x1A,
    ScanCode_Minus, //kVK_ANSI_Minus = 0x1B,
    ScanCode_8, //    kVK_ANSI_8  = 0x1C,
    ScanCode_0, //    kVK_ANSI_0  = 0x1D,
    ScanCode_BracketRight, //    kVK_ANSI_RightBracket = 0x1E,
    ScanCode_O, //    kVK_ANSI_O  = 0x1F,
    ScanCode_U, //    kVK_ANSI_U  = 0x20,
    ScanCode_BracketLeft, //    kVK_ANSI_LeftBracket = 0x21,
    ScanCode_I, //    kVK_ANSI_I  = 0x22,
    ScanCode_P, //    kVK_ANSI_P  = 0x23,
    ScanCode_Return, // kVK_Return = 0x24,
    ScanCode_L, //    kVK_ANSI_L  = 0x25,
    ScanCode_J, //    kVK_ANSI_J  = 0x26,
    0, //    kVK_ANSI_Quote                = 0x27,
    ScanCode_K, //    kVK_ANSI_K  = 0x28,
    ScanCode_Semicolon, // kVK_ANSI_Semicolon = 0x29,
    ScanCode_Backslash, // kVK_ANSI_Backslash = 0x2A,
    ScanCode_Comma, //   kVK_ANSI_Comma = 0x2B,
    ScanCode_Slash, //   kVK_ANSI_Slash = 0x2C,
    ScanCode_N,     //   kVK_ANSI_N     = 0x2D,
    ScanCode_M,     //   kVK_ANSI_M     = 0x2E,
    ScanCode_Period,//   kVK_ANSI_Period = 0x2F,
    ScanCode_Tab,   //   kVK_Tab        = 0x30,
    ScanCode_Space, //   kVK_Space      = 0x31,
    0, //    kVK_ANSI_Grave                = 0x32,
    ScanCode_Delete, // kVK_Delete      = 0x33,
    0, //                             ? = 0x34
    0, //    kVK_Escape                 = 0x35,
    0, //                             ? = 0x36
    0, //    kVK_Command                = 0x37,
    ScanCode_Shift, //  kVK_Shift       = 0x38,
    ScanCode_Caps,  // kVK_CapsLock     = 0x39,
    0, //    kVK_Option                 = 0x3A,
    ScanCode_Ctrl, // kVK_Control       = 0x3B,
    ScanCode_Shift, // kVK_RightShift   = 0x3C,
    0, // kVK_RightOption               = 0x3D,
    ScanCode_Ctrl, // kVK_RightControl  = 0x3E,
    0, // kVK_Function                  = 0x3F,
    0, //kVK_F17                       = 0x40,
    0, //    kVK_ANSI_KeypadDecimal        = 0x41,
    0, //                             ? = 0x42
    0, //    kVK_ANSI_KeypadMultiply    = 0x43,
    ScanCode_Plus, //    kVK_ANSI_KeypadPlus = 0x45,
    0, //    kVK_ANSI_KeypadClear          = 0x47,
    0, // kVK_VolumeUp                  = 0x48,
    0, // kVK_VolumeDown                = 0x49,
    0, // kVK_Mute                      = 0x4A,
    0, //    kVK_ANSI_KeypadDivide         = 0x4B,
    ScanCode_Return, //    kVK_ANSI_KeypadEnter          = 0x4C,
    0, //                             ? = 0x4D
    ScanCode_Minus, //    kVK_ANSI_KeypadMinus = 0x4E,
    0, // kVK_F18                       = 0x4F,
    0, // kVK_F19                       = 0x50,
    ScanCode_Equals, //    kVK_ANSI_KeypadEquals  = 0x51,
    ScanCode_0, //    kVK_ANSI_Keypad0              = 0x52,
    ScanCode_1, //    kVK_ANSI_Keypad1              = 0x53,
    ScanCode_2, //    kVK_ANSI_Keypad2              = 0x54,
    ScanCode_3, //    kVK_ANSI_Keypad3              = 0x55,
    ScanCode_4, //    kVK_ANSI_Keypad4              = 0x56,
    ScanCode_5, //    kVK_ANSI_Keypad5              = 0x57,
    ScanCode_6, //    kVK_ANSI_Keypad6              = 0x58,
    ScanCode_7, //    kVK_ANSI_Keypad7              = 0x59,
    0,          //    kVK_F20                       = 0x5A,
    ScanCode_8, //    kVK_ANSI_Keypad8              = 0x5B,
    ScanCode_9, //    kVK_ANSI_Keypad9              = 0x5C
    0,          //                                  = 0x5D
    0,          //                                  = 0x5E
    0,          //                                  = 0x5F
    ScanCode_F5, //    kVK_F5                        = 0x60,
    ScanCode_F6, //    kVK_F6                        = 0x61,
    ScanCode_F7, //    kVK_F7                        = 0x62,
    ScanCode_F3, //    kVK_F3                        = 0x63,
    ScanCode_F8, //    kVK_F8                        = 0x64,
    ScanCode_F9, //    kVK_F9                        = 0x65,
    0,          //                                  = 0x66
    0, //    kVK_F11                       = 0x67,
    0,          //                                  = 0x68
    0, //    kVK_F13                       = 0x69,
    0, //    kVK_F16                       = 0x6A,
    0, //    kVK_F14                       = 0x6B,
    0, //                                  = 0x6C
    0, //    kVK_F10                       = 0x6D,
    0, //                                  = 0x6E
    0, //    kVK_F12                       = 0x6F,
    0, //                                  = 0x70
    0, //    kVK_F15                       = 0x71,
    0, //    kVK_Help                      = 0x72,
    0, //    kVK_Home                      = 0x73,
    0, //    kVK_PageUp                    = 0x74,
    0, //    kVK_ForwardDelete             = 0x75,
    ScanCode_F4, // kVK_F4                  = 0x76,
    0, //    kVK_End                       = 0x77,
    ScanCode_F2, //    kVK_F2              = 0x78,
    0, //    kVK_PageDown                  = 0x79,
    ScanCode_F1, //    kVK_F1              = 0x7A,
    ScanCode_Left, //    kVK_LeftArrow     = 0x7B,
    ScanCode_Right, //    kVK_RightArrow    = 0x7C,
    ScanCode_Down, //    kVK_DownArrow      = 0x7D,
    ScanCode_Up, //    kVK_UpArrow                   = 0x7E
};

//
// See http://www.usb.org/developers/hidpage/Hut1_12v2.pdf for USB/HID keycodes
//
static int s_usbHIDKeycodesToScancodes[] = {
	0,							// 0x00
	0,							// 0x01
	0,							// 0x02
	0,							// 0x03
	ScanCode_A,					// 0x04
	ScanCode_B,					// 0x05
	ScanCode_C,					// 0x06
	ScanCode_D,					// 0x07
	ScanCode_E,					// 0x08
	ScanCode_F,					// 0x09
	ScanCode_G,					// 0x0A
	ScanCode_H,					// 0x0B
	ScanCode_I,					// 0x0C
	ScanCode_J,					// 0x0D
	ScanCode_K,					// 0x0E
	ScanCode_L,					// 0x0F
	ScanCode_M,					// 0x10
	ScanCode_N,					// 0x11
	ScanCode_O,					// 0x12
	ScanCode_P,					// 0x13
	ScanCode_Q,					// 0x14
	ScanCode_R,					// 0x15
	ScanCode_S,					// 0x16
	ScanCode_T,					// 0x17
	ScanCode_U,					// 0x18
	ScanCode_V,					// 0x19
	ScanCode_W,					// 0x1A
	ScanCode_X,					// 0x1B
	ScanCode_Y,					// 0x1C
	ScanCode_Z,					// 0x1D
	ScanCode_1,					// 0x1E
	ScanCode_2,					// 0x1F
	ScanCode_3,					// 0x20
	ScanCode_4,					// 0x21
	ScanCode_5,					// 0x22
	ScanCode_6,					// 0x23
	ScanCode_7,					// 0x24
	ScanCode_8,					// 0x25
	ScanCode_9,					// 0x26
	ScanCode_0,					// 0x27
	ScanCode_Return,			// 0x28
	ScanCode_Escape,			// 0x29
	ScanCode_Delete,			// 0x2A
	ScanCode_Tab,				// 0x2B
	ScanCode_Space,				// 0x2C
	ScanCode_Minus,				// 0x2D
	ScanCode_Equals,			// 0x2e
	ScanCode_BracketLeft,		// 0x2f
	ScanCode_BracketRight,		// 0x30
	ScanCode_Backslash,			// 0x31
	0 /*KEY_EUROPE_1*/,			// 0x32
	ScanCode_Semicolon,			// 0x33
	ScanCode_Apostrophe,		// 0x34
	0 /*KEY_GRAVE*/,			// 0x35
	ScanCode_Comma,				// 0x36
	ScanCode_Period,			// 0x37
	ScanCode_Slash,				// 0x38
	ScanCode_Caps,				// 0x39
	ScanCode_F1,				// 0x3A
	ScanCode_F2,				// 0x3B
	ScanCode_F3,				// 0x3C
	ScanCode_F4,				// 0x3D
	ScanCode_F5,				// 0x3E
	ScanCode_F6,				// 0x3F
	ScanCode_F7,				// 0x40
	ScanCode_F8,				// 0x41
	ScanCode_F9,				// 0x42
	ScanCode_Break,	/*F10*/		// 0x43
	ScanCode_Break,	/*F11*/		// 0x44
	ScanCode_Break,	/*F12*/		// 0x45
	0, /*KEY_PRINT_SCREEN*/		// 0x46
	0, /*KEY_SCROLL_LOCK*/		// 0x47
	0, /*KEY_PAUSE*/			// 0x48
	0, /*KEY_INSERT*/			// 0x49
	0, /*KEY_HOME*/				// 0x4A
	0, /*KEY_PAGE_UP*/			// 0x4B
	ScanCode_Delete,			// 0x4C
	0, /*KEY_END*/				// 0x4D
	0, /*KEY_PAGE_DOWN*/		// 0x4E
	ScanCode_Right,				// 0x4F
	ScanCode_Left,				// 0x50
	ScanCode_Down,				// 0x51
	ScanCode_Up,				// 0x52
	0, /*KEY_NUM_LOCK*/			// 0x53
	ScanCode_Slash,				// 0x54 (KEY_KEYPAD_DIVIDE)
	ScanCode_Star,				// 0x55 (KEY_KEYPAD_MULTIPLY)
	ScanCode_Minus,				// 0x56 (KEY_KEYPAD_SUBTRACT)
	ScanCode_Plus,				// 0x57 (KEY_KEYPAD_ADD)
	ScanCode_Return,			// 0x58 (KEY_KEYPAD_ENTER)
	ScanCode_1,					// 0x59 (KEY_KEYPAD_1)
	ScanCode_2,					// 0x5A (KEY_KEYPAD_2)
	ScanCode_3,					// 0x5B (KEY_KEYPAD_3)
	ScanCode_4,					// 0x5C (KEY_KEYPAD_4)
	ScanCode_5,					// 0x5D (KEY_KEYPAD_5)
	ScanCode_6,					// 0x5E (KEY_KEYPAD_6)
	ScanCode_7,					// 0x5F (KEY_KEYPAD_7)
	ScanCode_8,					// 0x60 (KEY_KEYPAD_8)
	ScanCode_9,					// 0x61 (KEY_KEYPAD_9)
	ScanCode_0,					// 0x62 (KEY_KEYPAD_0)
	ScanCode_Period,			// 0x63 (KEY_KEYPAD_DECIMAL)
	0,							// 0x64 (KEY_EUROPE_2)
	0,							// 0x65 (KEY_APPLICATION)
	0,							// 0x66 (KEY_POWER)
	ScanCode_Equals,			// 0x67 (KEY_KEYPAD_EQUAL)
	0,							// 0x68 (KEY_F13)
	0,							// 0x69 (KEY_F14)
	0,							// 0x6A (KEY_F15)
};


/*	final byte KEY_CONTROL_LEFT       =byte(0xE0);
	final byte KEY_SHIFT_LEFT         =byte(0xE1);
	final byte KEY_ALT_LEFT           =byte(0xE2);
	final byte KEY_GUI_LEFT           =byte(0xE3);
	final byte KEY_CONTROL_RIGHT      =byte(0xE4);
	final byte KEY_SHIFT_RIGHT        =byte(0xE5);
	final byte KEY_ALT_RIGHT          =byte(0xE6);
	final byte KEY_GUI_RIGHT          =byte(0xE7);
*/


int BeebKey_scancodeForChar(unsigned short chr) {
    std::map<unsigned short, uint32_t>::iterator it = s_charsToScancodes.find(chr);
    if (it != s_charsToScancodes.end()) {
        return it->second;
    }
    return 0;
}

int BeebKey_scancodeForMacVKeycode(int keycode) {
    if (keycode < sizeof(s_macVKeycodesToScancodes)/sizeof(s_macVKeycodesToScancodes[0])) {
        return s_macVKeycodesToScancodes[keycode];
    }
    return 0;
}

    
int BeebKey_scancodeForUsbHIDKeycode(int hidKeycode) {
	if (hidKeycode==0xe1 || hidKeycode==0xe5) return ScanCode_Shift;
	if (hidKeycode <= 0x6a) {
		return s_usbHIDKeycodesToScancodes[hidKeycode];
	}
	return 0;
}

//static HashMap<Integer, BeebKey> bbcKeyMap = new HashMap<Integer,BeebKey>();



/*	@Override
	public String toString() {
		return name;
	}
	
	public static BeebKey findByname(String name) {
		for (BeebKey beebKey : bbcKeyList) {
			if (name.equalsIgnoreCase(beebKey.name)) {
				return beebKey;
			}
		}
		return BeebKey.NONE;
	}
	public static int findIndexByname(String name) {
		if (!TextUtils.isEmpty(name)) {
			for (int i = 0; i < bbcKeyList.size(); i++) {
				if (name.equalsIgnoreCase(bbcKeyList.get(i).name)) {
					return i;
				}
			}
		}
		return 0;
	}
*/

}


