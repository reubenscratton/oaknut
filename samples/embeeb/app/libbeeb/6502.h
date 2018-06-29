//
//  6502.h
//

#ifndef _6502_H_INCLUDED_
#define _6502_H_INCLUDED_


//#define USE_ASM


#define FLAG_C 1
#define FLAG_Z 2
#define FLAG_I 4
#define FLAG_D 8
#define FLAG_V 0x40
#define FLAG_N 0x80

#ifdef __LP64__
#define OFFSET_TO_pc 8
#define OFFSET_TO_a 10
#define OFFSET_TO_x 11
#define OFFSET_TO_y 12
#define OFFSET_TO_s 13
#define OFFSET_TO_p 14
#define OFFSET_TO_interrupt 16
#define OFFSET_TO_nmi 20
#define OFFSET_TO_takeint 24
#define OFFSET_TO_cycles  28
#define OFFSET_TO_pendingCycles 32
#define OFFSET_TO_codeCoverageMode 36
#define OFFSET_TO_codeCoverageBuff 40
#define OFFSET_TO_c_fns 48
#define OFFSET_TO_a_fns 56
#define OFFSET_TO_num_triggers 64
#define OFFSET_TO_pc_triggers 72
#define SIZEOF_pc_trigger 32
#else
#define OFFSET_TO_pc 4
#define OFFSET_TO_a 6
#define OFFSET_TO_x 7
#define OFFSET_TO_y 8
#define OFFSET_TO_s 9
#define OFFSET_TO_p 10
#define OFFSET_TO_interrupt 12
#define OFFSET_TO_nmi 16
#define OFFSET_TO_takeint 20
#define OFFSET_TO_cycles 24
#define OFFSET_TO_pendingCycles 28
#define OFFSET_TO_codeCoverageMode 32
#define OFFSET_TO_codeCoverageBuff 36
#define OFFSET_TO_c_fns 40
#define OFFSET_TO_a_fns 44
#define OFFSET_TO_num_triggers 48
#define OFFSET_TO_pc_triggers 52
#define SIZEOF_pc_trigger 16
#endif

#ifndef _ASSEMBLY_


#include <stdint.h>
#include <stddef.h>


typedef void(*PCALLBACK)(const void*);
typedef void(*PCALLBACK2)(const void*,void*);


typedef struct {
	uint16_t addr;
	uint16_t hit;
	PCALLBACK2 callback;
	void* callback_param1;
	void* callback_param2;
} pc_trigger;

typedef struct _M6502 {
	uint8_t* mem;				// +0
	uint16_t pc;				// +4
	uint8_t a;					// +6
	uint8_t x;					// +7
	uint8_t y;					// +8
	uint8_t s;					// +9
	uint8_t p;					// +10
	uint8_t unused; //
	int32_t interrupt;			// +12
	int32_t nmi;				// +16
	int32_t takeint;			// +20
	uint32_t cycles;				// +24
	uint32_t pendingCycles;		// +28
	int32_t codeCoverageMode;	// +32
	uint8_t* codeCoverageBuff;	// +36
	void* c_fns;
	void* a_fns;
	uint16_t num_triggers;
	pc_trigger pc_triggers[8];
	
#ifdef __cplusplus
	uint8_t* serialize(bool saving, uint8_t* p);
#endif
	
} M6502;



#define CASSERT(predicate) typedef char assertion_failed_##__LINE__[2*!!(predicate)-1];

CASSERT(OFFSET_TO_pc == offsetof(M6502, pc));
CASSERT(OFFSET_TO_a == offsetof(M6502, a));
CASSERT(OFFSET_TO_x == offsetof(M6502, x));
CASSERT(OFFSET_TO_y == offsetof(M6502, y));
CASSERT(OFFSET_TO_s == offsetof(M6502, s));
CASSERT(OFFSET_TO_p == offsetof(M6502, p));
CASSERT(OFFSET_TO_interrupt == offsetof(M6502, interrupt));
CASSERT(OFFSET_TO_nmi == offsetof(M6502, nmi));
CASSERT(OFFSET_TO_takeint == offsetof(M6502, takeint));
CASSERT(OFFSET_TO_cycles == offsetof(M6502, cycles));
CASSERT(OFFSET_TO_pendingCycles == offsetof(M6502, pendingCycles));
CASSERT(OFFSET_TO_codeCoverageMode == offsetof(M6502, codeCoverageMode));
CASSERT(OFFSET_TO_codeCoverageBuff == offsetof(M6502, codeCoverageBuff));
CASSERT(OFFSET_TO_c_fns == offsetof(M6502, c_fns));
CASSERT(OFFSET_TO_a_fns == offsetof(M6502, a_fns));
CASSERT(OFFSET_TO_num_triggers == offsetof(M6502, num_triggers));
CASSERT(OFFSET_TO_pc_triggers == offsetof(M6502, pc_triggers));
CASSERT(SIZEOF_pc_trigger == sizeof(pc_trigger));


#ifdef __cplusplus
extern "C" {
#endif
void reset6502();
void exec6502(M6502*);
void exec6502_c();
void execSingle();
#ifdef __cplusplus
}
#endif

#endif

#endif

