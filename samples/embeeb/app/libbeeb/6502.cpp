/*
 * 6502 CPU emulation (C version)
 *
 * Based on original code by Tom Walker
 *
 */

#include "6502.h"
#include "libbeeb.h"
#include "beeb.h"
#include "dis.h"
#undef ORA

//#define WANT_LOG_CPU

#if DEBUG
#define DBG_ASSERT(cond, log, ...) if (!(cond)) throw 1;
#else
#define DBG_ASSERT(cond,...)
#endif


// TODO: move these to M6502 struct
int otherstuffcount=0;
int romsel;



extern "C" {

#ifdef WANT_LOG_CPU
void log_cpu(M6502* cpu) {
    
    // skip the initial mem clear
    if (cpu->pc>=0xD9EE && cpu->pc<=0xD9F5) {
        return;
    }
	//unsigned char* p = cpu->mem;

	// Checksum RAM
#if 0
	unsigned int sum = 0;
	for (int i=0 ; i<65536 ; i++) {
		sum += cpu->mem[i];
	}
	app.log("PC:%04X (%02X %02X %02X) A:%02X X:%02X Y:%02X P:%02X S:%02X mem:%08X i=%d n=%d ta=%d", cpu->pc, p[cpu->pc],p[cpu->pc+1],p[cpu->pc+2], cpu->a, cpu->x, cpu->y, cpu->p, cpu->s, sum, cpu->interrupt, cpu->nmi, cpu->takeint);
#else
	char cmdbuff[256];
	uint16_t pc = cpu->pc;
	disassemble(&pc, cmdbuff, cpu->mem);
//	bbcLogI("%s A:%02X X:%02X Y:%02X %d\n", cmdbuff, cpu->a, cpu->x, cpu->y, the_beeb->uservia.t2c);
    app.log("%s %02X %02X %02X", cmdbuff, cpu->a, cpu->x, cpu->y);

#endif

}
#endif

	
bool is1MHzAccess(uint16_t addr) {
	static int FEslowdown[8]={1,0,1,1,0,0,1,0};
	return (addr >= 0xfc00 && addr < 0xff00 && (addr < 0xfe00 || FEslowdown[(addr >> 5) & 7]));
}



#define writemem(x, val) if (((uint16_t)x)<0x8000u) {cpu->mem[x]=val;} else writemem_ex(cpu, x, val)
	
#define SETFLAG(FLAG, EXPR) if (EXPR) cpu->p|=FLAG; else cpu->p&=~FLAG

#define setzn(v) if(v) cpu->p &=~FLAG_Z; else cpu->p|=FLAG_Z;  \
                 if ((v)&0x80) cpu->p|=FLAG_N; else cpu->p&=~FLAG_N;

#define push(v) cpu->mem[0x100+(cpu->s--)]=v
#define pull()  cpu->mem[0x100+(++cpu->s)]


#define ACCURATE 0

#if ACCURATE
#define TICK1          cpu->pendingCycles++;
#define TICK1_ADDR     cpu->pendingCycles++;     \
                      if (is1MHzAccess(addr)) { \
                        cpu->pendingCycles += ((cpu->pendingCycles + cpu->cycles) & 1) ? 1 : 2; \
                      }
#define CHECK_INT       TICK_SYNC()									\
                        cpu->takeint=(cpu->interrupt && !p_i);
#define TICK_SYNC() if (cpu->pendingCycles > 0) tickSync(cpu);
#define TICK(N)
#else
#define TICK1
#define TICK1_ADDR
#define TICK_SYNC()
#define CHECK_INT
#define TICK(N)     cpu->pendingCycles+=N; tickSync(cpu); cpu->takeint=(cpu->interrupt && !p_i);
#endif


void tickSync(M6502* cpu);
	
	
#define readmem(x)  (((x)<0xfc00) ? cpu->mem[x] : readmem_ex(cpu, x))
	
uint8_t readmem_ex(M6502* cpu, uint16_t addr) {
	TICK_SYNC();
	if (addr>=0xFE00 && addr<0xFE08) return the_beeb->video.readcrtc(addr);
	if (addr>=0xFE08 && addr<0xFE10) return the_beeb->acia.read(addr);
	if (addr>=0xFE40 && addr<0xFE60) return the_beeb->sysvia.read(addr);
	if (addr>=0xFE60 && addr<0xFE80) return the_beeb->uservia.read(addr);
	if (addr>=0xFE80 && addr<0xFEA0) return read8271(addr);
	if (addr>=0xFEC0 && addr<0xFEE0) return readadc(addr);
	if (addr>=0xFC00 && addr<0xFE00) return 0xFF;
	return the_beeb->cpu.mem[addr];
}


void writemem_ex(M6502* cpu, uint16_t addr, uint8_t val) {
    TICK_SYNC();
	if (addr>=0xFE00 && addr<0xFE08) return the_beeb->video.writecrtc(addr, val);
    if (addr>=0xFE08 && addr<0xFE10) return the_beeb->acia.write(addr, val);
    if (addr>=0xFE20 && addr<0xFE24) return the_beeb->video.writeula(addr,val);
    if (addr>=0xFE30 && addr<0xFE34) {
        if (romsel != (val&15)) {
            romsel = val&15;
            //bbcLogI("ROMSEL set to %d", romsel);
            uint8_t* rom = the_beeb->roms[15-romsel];
            if (rom) {
                memcpy(the_beeb->cpu.mem+0x8000, rom, (romsel==15)?8192:16384);
            }
        }
    }
    if (addr>=0xFE40 && addr<0xFE60) return the_beeb->sysvia.write(addr, val);
    if (addr>=0xFE60 && addr<0xFE80) return the_beeb->uservia.write(addr, val);
    if (addr>=0xFE80 && addr<0xFEA0) return write8271(addr, val);
    if (addr>=0xFEC0 && addr<0xFEE0) return writeadc(addr, val);
}

//extern void* fns_asm;

void reset6502() {
	the_beeb->cpu.cycles=0;
	the_beeb->cpu.pendingCycles=0;
	the_beeb->cpu.pc=*(uint16_t*)&(the_beeb->cpu.mem[0xfffc]);
#ifdef _TEST_6502_
	the_beeb->cpu.pc=0x400;
#else
	the_beeb->cpu.p |= FLAG_I;
#endif
	the_beeb->cpu.nmi =0;
    bbcLogI("\n\n6502 RESET!\n");
}



void adc_bcd(M6502* cpu, uint8_t temp) {
    //bbcLogI("Doing ADC BCD! %d", temp);
	int ah=0;
	uint8_t tempb = cpu->a+temp+((cpu->p & FLAG_C)?1:0);
	if (!tempb)
	   cpu->p |= FLAG_Z;
	int al=(cpu->a&0xF)+(temp&0xF)+((cpu->p & FLAG_C)?1:0);
	if (al>9) {
		al-=10;
		al&=0xF;
		ah=1;
	}
	ah+=((cpu->a>>4)+(temp>>4));
	if (ah&8)
		cpu->p |= FLAG_N;
	SETFLAG(FLAG_V, (((ah << 4) ^ cpu->a) & 128) && !((cpu->a ^ temp) & 128));
	cpu->p &= ~FLAG_C;
	if (ah>9) {
		cpu->p |= FLAG_C;
		ah-=10;
		ah&=0xF;
	}
	cpu->a=(al&0xF)|(ah<<4);
}

void sbc_bcd(M6502* cpu, uint8_t temp) {
	int hc6=0;
	cpu->p &= ~(FLAG_Z | FLAG_N);
	if (!((cpu->a-temp)-((cpu->p & FLAG_C)?0:1)))
	   cpu->p |= FLAG_Z;
	int al=(cpu->a&15)-(temp&15)-((cpu->p & FLAG_C)?0:1);
	if (al&16) {
		al-=6;
		al&=0xF;
		hc6=1;
	}
	int ah=(cpu->a>>4)-(temp>>4);
	if (hc6) ah--;
	if ((cpu->a-(temp+((cpu->p & FLAG_C)?0:1)))&0x80)
	   cpu->p |= FLAG_N;
	SETFLAG(FLAG_V, (((cpu->a-(temp+((cpu->p & FLAG_C)?0:1)))^temp)&128)&&((cpu->a^temp)&128));
	cpu->p |= FLAG_C;
	if (ah&16)  {
		cpu->p &= ~FLAG_C;
		ah -= 6;
		ah &= 0xF;
	}
	cpu->a=(al&0xF)|((ah&0xF)<<4);
}



void tickSync(M6502* cpu) {
	int c = cpu->pendingCycles;
	cpu->pendingCycles = 0;
	
	while (otherstuffcount<=0) {
		otherstuffcount+=128;
		logvols();
		if (motorspin) {
			motorspin--;
			if (!motorspin) fdcspindown();
		}
	}

	// Time to poll hardware?
	if (c > 0) {
		the_beeb->sysvia.polltime(c);
		the_beeb->uservia.polltime(c);
		the_beeb->video.pollvideo(c);
		otherstuffcount-=c;
		if (motoron) {
			if (fdctime) {
				fdctime-=c;
				if (fdctime<=0) fdccallback();
			}
			disctime-=c;
			if (disctime<=0) {
				disctime+=16; disc_poll();
			}
		}
		polladc(c);
		//tubecycle += (c);
	}

	cpu->cycles += c;
}



uint8_t* _M6502::serialize(bool saving, uint8_t* p) {
	memcpy(saving?p:mem, saving?mem:p, 65536); p += 65536;
	SERIALIZE(a);
	SERIALIZE(x);
	SERIALIZE(y);
	SERIALIZE(this->p);
	SERIALIZE(s);
	SERIALIZE(pc);
	SERIALIZE(nmi);
	SERIALIZE(interrupt);
	SERIALIZE(takeint);
	SERIALIZE(cycles);
	SERIALIZE(pendingCycles);
	//SERIALIZE(num_triggers);
	//SERIALIZE(pc_triggers);
	SERIALIZE(otherstuffcount);
	SERIALIZE(romsel);
	return p;
}


#define p_c (cpu->p&FLAG_C)
#define p_z (cpu->p&FLAG_Z)
#define p_i (cpu->p&FLAG_I)
#define p_d (cpu->p&FLAG_D)
#define p_v (cpu->p&FLAG_V)
#define p_n (cpu->p&FLAG_N)




/*ADC/SBC*/
#define ADC_real(temp)					 \
		uint16_t tempw = (cpu->a + temp + (p_c ? 1 : 0)); \
		SETFLAG(FLAG_V, (!((cpu->a ^ temp) & 0x80) && ((cpu->a ^ tempw) & 0x80)));  \
		cpu->a = tempw & 0xFF;					 \
		SETFLAG(FLAG_C, tempw & 0x100);          \
		setzn(cpu->a);



#define CCB_EXEC 0x40
#define CCB_BRANCH 0x80
#define CCB_JUMP 0xC0

void mark_code_cov_byte(uint16_t a, uint8_t b) {
	/*if (the_beeb->cpu.codeCoverageMode == 1) {
		uint8_t c = the_beeb->cpu.codeCoverageBuff[a];
		if (b > (c&0xc0)) {
			c = b | (c&0x7f);
		}
		if ((c & 0x3f) < 0x3F) {
			c = (c & 0xc0) | ((c & 0x3f)+1);
		}
		the_beeb->cpu.codeCoverageBuff[a] = c;
	}*/
}




#define ADC if (p_d) adc_bcd(cpu, val8); else {ADC_real(val8);}
#define SBC if (p_d) sbc_bcd(cpu, val8); else {val8^=255; ADC_real(val8);}
#define AND cpu->a&=val8; setzn(cpu->a);
#define ASL SETFLAG(FLAG_C, val8&0x80); \
			val8<<=1; \
			setzn(val8);
#define BIT SETFLAG(FLAG_Z, !(cpu->a&val8)); \
			SETFLAG(FLAG_V, val8&0x40); \
			SETFLAG(FLAG_N, val8&0x80);
#define CPX setzn(cpu->x-val8); SETFLAG(FLAG_C,cpu->x>=val8);
#define CPY setzn(cpu->y-val8); SETFLAG(FLAG_C,cpu->y>=val8);
#define CMP setzn(cpu->a-val8); SETFLAG(FLAG_C,cpu->a>=val8);
#define DEC val8--; setzn(val8);
#define EOR cpu->a^=val8; setzn(cpu->a);
#define INC val8++; setzn(val8);
#define LDA cpu->a=val8; setzn(cpu->a);
#define LDX cpu->x=val8; setzn(cpu->x);
#define LDY cpu->y=val8; setzn(cpu->y);
#define LAX cpu->a=cpu->x=val8; setzn(cpu->a);
#define LSR SETFLAG(FLAG_C, val8&1); val8>>=1; setzn(val8);
#define NOP val8=val8;
#define ORA cpu->a|=val8; setzn(cpu->a);
#define PLA	cpu->a=pull(); \
			setzn(cpu->a);
#define PLP	temp=pull(); \
			cpu->p = (temp & ~0x30);
#define ROL temp=p_c; \
			SETFLAG(FLAG_C, val8&0x80); \
			val8<<=1; \
			if (temp) val8|=1; \
			setzn(val8);
#define ROR temp=p_c; \
			SETFLAG(FLAG_C, val8&1); \
			val8>>=1; \
			if (temp) val8|=0x80; \
				setzn(val8);
	
// Undocumented
#define ANC AND SETFLAG(FLAG_C, p_n);
#define ASR cpu->a&=val8; SETFLAG(FLAG_C,cpu->a&1); cpu->a>>=1; setzn(cpu->a);
#define ARR AND ROR
#define DCP DEC	CMP
#define ISB INC	SBC
#define LAS cpu->a=cpu->x=cpu->s=cpu->s&val8; setzn(cpu->a); // No, really!
#define RRA ROR ADC
#define RLA ROL AND
#define SBX setzn((cpu->a&cpu->x)-val8); \
			SETFLAG(FLAG_C, (cpu->a&cpu->x)>=val8); \
			cpu->x=(cpu->a&cpu->x)-val8;
#define SLO ASL ORA
#define SRE LSR EOR
#define SHA val8 = cpu->a&cpu->x&((addr>>8)+1);
#define SHX val8 = cpu->x&((addr>>8)+1);
#define SHY val8 = cpu->y&((addr>>8)+1);
#define SHS val8 = cpu->a&cpu->x&((addr>>8)+1); \
				   cpu->s=cpu->a&cpu->x;
	


#define LD_IMM  val8 = readmem(cpu->pc); cpu->pc++;
#define LD_MEM	val8 = readmem(addr);
#define LD_A	val8 = cpu->a;
#define LD_X	val8 = cpu->x;
#define LD_Y	val8 = cpu->y;
#define LD_AX	val8 = cpu->a&cpu->x;

#define ST_A	cpu->a = val8;
#define ST_MEM  writemem(addr, val8);

    





// This is super-messy cos of CHECK_INT having to be on penultimate cycle
#define FN_BRANCH(COND) \
/*T0*/ TICK1;                                        \
        if (!(COND)) {												\
            CHECK_INT                                               \
/*T1*/ TICK1; /*int8_t offset=(int8_t)*/readmem(cpu->pc); cpu->pc++; \
        TICK(2); \
        } else {                                                    \
/*T1*/ TICK1; int8_t offset=(int8_t)readmem(cpu->pc); cpu->pc++; \
                            DBG_ASSERT(offset!=-2, "Infinite loop at 0x%X", cpu->pc-2); \
                            uint16_t pc_old=cpu->pc;                \
                            cpu->pc+=offset;                        \
                            if ((pc_old&0xFF00)^(cpu->pc&0xFF00)) { \
/*T2*/TICK1;                      \
                                CHECK_INT                           \
/*T3*/TICK1;                      \
                            } else {                                \
                                CHECK_INT                           \
/*T2*/ TICK1;                     \
                            }                                       \
                TICK(3) \
        }                                                           \
        mark_code_cov_byte(cpu->pc, CCB_BRANCH);
    




// 1. Single-byte instructions : 2 cycles

#define FN_2(LOAD, TRANSFORM, STORE)                  \
 /*T0*/ TICK1;    LOAD                                           \
                 CHECK_INT                                      \
                 TRANSFORM                                      \
                 STORE                                          \
 /*T1*/ TICK1; \
        TICK(2)


#define FN_ZP(LOAD, TRANSFORM, STORE)                               \
 /*T0*/ TICK1;	     										        \
 /*T1*/ TICK1;        addr=readmem(cpu->pc); cpu->pc++;     \
                     CHECK_INT						                \
 /*T2*/ TICK1;        LOAD										    \
                     TRANSFORM                                      \
                     STORE \
        TICK(3)

#define FN_RMW_ZP(TRANSFORM)                              \
 /*T0*/ TICK1;		    									        \
 /*T1*/ TICK1;        addr=readmem(cpu->pc); cpu->pc++;     \
 /*T2*/ TICK1;        LD_MEM											\
 /*T3*/ TICK1;        TRANSFORM                                          \
                     CHECK_INT									        \
 /*T4*/ TICK1;        ST_MEM \
        TICK(5)
    
    


#define FN_4(LOAD, TRANSFORM, STORE)                      \
 /*T0*/ TICK1;											            \
 /*T1*/ TICK1;       addr = readmem(cpu->pc); cpu->pc++;    \
 /*T2*/ TICK1;       addr |= (readmem(cpu->pc)<<8); cpu->pc++;       \
                    CHECK_INT										\
 /*T3*/ TICK1_ADDR;  LOAD										    \
                    TRANSFORM                                       \
                    STORE   \
        TICK(4)
    
    
#define FN_4_O(OFFSET, LOAD, TRANSFORM, STORE)            \
 /*T0*/ TICK1;											            \
 /*T1*/ TICK1;        bal = readmem(cpu->pc); cpu->pc++;	    \
 /*T2*/ TICK1;        bal += OFFSET;	/*NB: cant go beyond page 0*/	\
                     addr = bal;                           \
                     CHECK_INT                                      \
 /*T3*/ TICK1_ADDR;   LOAD										    \
                     TRANSFORM                                      \
                     STORE \
        TICK(4)
    
    
    
#define FN_INDZX(LOAD, TRANSFORM, STORE)              \
 /*T0*/ TICK1;											        \
 /*T1*/ TICK1;      bal = readmem(cpu->pc); cpu->pc++;	\
 /*T2*/ TICK1;      bal += cpu->x;								\
 /*T3*/ TICK1;      addr = readmem((uint16_t)bal);      \
 /*T4*/ TICK1;      addr |= (readmem((uint16_t)(bal + 1))<<8);   \
                   CHECK_INT									\
 /*T5*/ TICK1_ADDR; LOAD										\
                   TRANSFORM                                    \
                   STORE \
        TICK(6)
    
    
#define FN_RO_INDZX(TRANSFORM)  FN_INDZX(LD_MEM, TRANSFORM, )
#define FN_WO_INDZX(TRANSFORM)  FN_INDZX(       , TRANSFORM, ST_MEM)


#define FN_RMW_ZPXY(OFFSET, TRANSFORM)                    \
 /*T0*/ TICK1;											            \
 /*T1*/ TICK1;         bal = readmem(cpu->pc); cpu->pc++;    \
 /*T2*/ TICK1;         bal += OFFSET;	/*NB: cant go beyond page 0*/ \
                      addr = bal;                          \
 /*T3*/ TICK1;         LD_MEM									    \
 /*T4*/ TICK1;         TRANSFORM                                     \
                      CHECK_INT                                     \
 /*T5*/ TICK1;         ST_MEM \
        TICK(6)
    


#define FN_RO_IMM(TRANSFORM)     FN_2(LD_IMM, TRANSFORM, )
#define FN_RO_ZP(TRANSFORM)      FN_ZP(LD_MEM, TRANSFORM, )
#define FN_WO_ZP(TRANSFORM)      FN_ZP( , TRANSFORM, ST_MEM)
#define FN_RO_ABS(TRANSFORM)     FN_4(LD_MEM, TRANSFORM, )
#define FN_WO_ABS(TRANSFORM)     FN_4( , TRANSFORM, ST_MEM)
#define FN_RO_ZPXY(OFFSET, TRANSFORM) FN_4_O(OFFSET, LD_MEM, TRANSFORM, )
#define FN_WO_ZPXY(OFFSET, TRANSFORM) FN_4_O(OFFSET,       , TRANSFORM, ST_MEM)



// 2.5 Read-only (abs address + X or Y) : 4 or 5 cycles depending if page boundary crossed

#define ADDR_ABS_XY(OFFSET)                                          \
 /*T0*/ TICK1;											             \
 /*T1*/ TICK1;         bal = readmem(cpu->pc); cpu->pc++;     \
 /*T2*/ TICK1;         bah = readmem(cpu->pc); cpu->pc++;     \
                      addr = (bah<<8) | bal;                \
                      addr_c = (addr+OFFSET);              \
                      addr = (addr&0xFF00) | (addr_c&0xFF);


#define FN_RO_ABSXY(OFFSET, TRANSFORM)                     \
                      ADDR_ABS_XY(OFFSET)                            \
                      CHECK_INT									     \
 /*T3*/ TICK1_ADDR;                                                   \
                      if (addr != addr_c) {                          \
                          addr = addr_c;                             \
                          CHECK_INT									 \
 /*T4*/ TICK1_ADDR; 											         \
                      }                                              \
                      LD_MEM                                         \
                      TRANSFORM \
        TICK(5)
    
    
#define FN_WO_ABS_XY(OFFSET, TRANSFORM)                    \
                      ADDR_ABS_XY(OFFSET)                            \
 /*T3*/ TICK1_ADDR;    addr = addr_c;                                 \
                      CHECK_INT                                      \
 /*T4*/ TICK1_ADDR;    TRANSFORM                                      \
                      ST_MEM \
        TICK(5)

    
    
    
#define FN_RMW_ABS_XY(OFFSET, TRANSFORM)                   \
                      ADDR_ABS_XY(OFFSET)                            \
 /*T3*/ TICK1_ADDR;    addr = addr_c;                                 \
 /*T4*/ TICK1_ADDR;    LD_MEM                                         \
 /*T5*/ TICK1_ADDR;    TRANSFORM                                      \
                      CHECK_INT                                      \
 /*T6*/ TICK1_ADDR;    ST_MEM \
        TICK(7)

    



// 2.7 Read-only (indirect zero-page + Y) : 5 or 6 cycles

#define FN_RO_INDZY(TRANSFORM)                            \
 /*T0*/ TICK1;											            \
 /*T1*/ TICK1;     ial = readmem(cpu->pc); cpu->pc++;	    \
 /*T2*/ TICK1;     bal = readmem((uint16_t)ial);				\
 /*T3*/ TICK1;     bah = readmem((uint16_t)(ial+1));			\
                  addr = bal | (bah<<8);                   \
                  addr_c =  (addr+cpu->y);                 \
                  addr_nc = (addr&0xFF00) | (addr_c&0xFF); \
                  addr = addr_nc;                                   \
                  CHECK_INT										    \
 /*T4*/ TICK1_ADDR;                                                  \
                   if (addr_nc != addr_c) {                         \
                       addr = addr_c;                               \
                       CHECK_INT								    \
 /*T5*/ TICK1_ADDR; 											        \
                   }                                                \
                   LD_MEM											\
                   TRANSFORM \
        TICK(6)
    



// 3.6 Write-only (indirect zero-page + Y) : 6 cycles

#define FN_WO_INDZY(TRANSFORM)                            \
 /*T0*/ TICK1											            \
 /*T1*/ TICK1;       ial = readmem(cpu->pc); cpu->pc++;	    \
 /*T2*/ TICK1;       bal = readmem((uint16_t)ial);			\
 /*T3*/ TICK1;       bah = readmem((uint16_t)(ial+1));		\
                    addr = (bah<<8) | ((bal+cpu->y)&0xFF); \
 /*T4*/ TICK1_ADDR;  addr = ((bah<<8) | bal) + cpu->y;               \
                    CHECK_INT										\
 /*T5*/ TICK1_ADDR;  TRANSFORM                                       \
                    ST_MEM \
        TICK(6)



// 4.2 Read-modify-write (absolute address) : 6 cycles
    
#define FN_RMW_ABS(TRANSFORM)                             \
 /*T0*/ TICK1;                                                       \
 /*T1*/ TICK1;          addr = readmem(cpu->pc); cpu->pc++; \
 /*T2*/ TICK1;          addr |= (readmem(cpu->pc)<<8); cpu->pc++;    \
 /*T3*/ TICK1_ADDR;     val8 = readmem(addr);				\
 /*T4*/ TICK1_ADDR;     TRANSFORM                                    \
                       CHECK_INT                                    \
 /*T5*/ TICK1_ADDR;     writemem(addr, val8); \
        TICK(6)


// 5.1 Pushes : 3 cycles

#define FN_WO_PUSH(REG)                                   \
 /*T0*/ TICK1;   											        \
 /*T1*/ TICK1;        /* next opcode, discarded */                   \
                     CHECK_INT                                      \
 /*T2*/ TICK1;        push(REG); \
        TICK(3);
    
    
// 5.2 Pulls : 4 cycles
    
#define FN_RO_PULL(OP)                                    \
 /*T0*/ TICK1;    											        \
 /*T1*/ TICK1;        /* next opcode, discarded */                   \
 /*T2*/ TICK1;        /* an unused read from the stack */            \
                     CHECK_INT                                      \
 /*T3*/ TICK1;        OP; \
        TICK(4);


// 5.3 JSR: 6 cycles
#define FN_JSR \
    /*T0*/ TICK1; \
    /*T1*/ TICK1; addr = readmem(cpu->pc); cpu->pc++; \
    /*T2*/ TICK1; /*unused read from stack*/ \
    /*T3*/ TICK1; push(cpu->pc>>8); \
    /*T4*/ TICK1; push(cpu->pc); \
                    CHECK_INT \
    /*T5*/ TICK1; addr |= (readmem(cpu->pc)<<8); \
                    cpu->pc=addr; \
        TICK(6);
    
    

// 5.5 RTI: 6 cycles
#define FN_RTI \
    /*T0*/ TICK1; \
    /*T1*/ TICK1;    /* unused read from PC+1 */ \
    /*T2*/ TICK1;    /* unused read from stack */ \
    /*T3*/ TICK1;    temp=pull(); \
    /*T4*/ TICK1;    cpu->pc=pull(); \
                    cpu->p = (temp & ~0x30); \
    /*T5*/ TICK1;    cpu->pc|=(pull()<<8); \
                    CHECK_INT; \
        TICK(6);
    
    
// 5.6.1 JMP abs: 3 cycles
#define FN_JMP \
    /*T0*/ TICK1; \
    /*T1*/ TICK1;    addr = readmem(cpu->pc); cpu->pc++; \
                    CHECK_INT \
    /*T2*/ TICK1;    addr |= (readmem(cpu->pc)<<8); cpu->pc++; \
                    cpu->pc=addr; \
           TICK(3); \
                    mark_code_cov_byte(cpu->pc, CCB_JUMP);

// 5.6.1 JMP ind: 5 cycles
#define FN_JMP_IND \
    /*T0*/ TICK1; \
    /*T1*/ TICK1;        addr = readmem(cpu->pc); cpu->pc++; \
    /*T2*/ TICK1;        addr |= (readmem(cpu->pc)<<8); cpu->pc++; \
    /*T3*/ TICK1_ADDR;   cpu->pc = readmem(addr); \
                        CHECK_INT; \
    /*T4*/ TICK1_ADDR;   cpu->pc |= readmem((addr&0xff00)|((addr+1)&0xff)) << 8; \
           TICK(5); \
                        mark_code_cov_byte(cpu->pc, CCB_JUMP);

    
// 5.7 RTS : 6 cycles
#define FN_RTS \
    /*T0*/ TICK1; \
    /*T1*/ TICK1;      /* unused read from PC+1 */  \
    /*T2*/ TICK1;      /* unused read from stack */ \
    /*T3*/ TICK1;      addr = pull(); \
    /*T4*/ TICK1;      addr |= (pull()<<8); \
                      cpu->pc = addr+1; \
                      CHECK_INT; \
    /*T5*/ TICK1_ADDR; /* unused read from PC-1 */ \
            TICK(6);


	// Argh! RMW+INDZX isn't spec'd! Work it out via visual6502 and remove these macros
    //
#define ADDR_INDZX	\
    TICK1;           bal = readmem(cpu->pc); cpu->pc++;	\
    TICK1;           bal += cpu->x;								\
                    bah = bal + 1;							\
    TICK1;           addr = readmem((uint16_t)bal);      \
    TICK1;           addr |= (readmem((uint16_t)bah)<<8);
    
#define ADDR_INDZ	\
    TICK1;           bal = readmem(cpu->pc); cpu->pc++;	\
                    bah = bal + 1;							\
    TICK1;           addr = readmem((uint16_t)bal);      \
    TICK1;           addr |= (readmem((uint16_t)bah)<<8);
    
#define OFFSET(x)   if ((addr&0xFF00)^((addr+x)&0xFF00)) TICK1;			\
    readmem((addr&0xFF00)|((addr+x)&0xFF));                 \
    addr += x;
    
#define OFFSET_Y OFFSET(cpu->y)
    
    
#define FN(ADDR_FETCH, OFFSET, LOAD, TRANSFORM, STORE)    \
        TICK1														\
        ADDR_FETCH													\
        OFFSET														\
        LOAD														\
        TRANSFORM													\
        STORE													    \
        TICK_SYNC()													\
        cpu->takeint=(cpu->interrupt && !p_i);						\
        TICK1 \
        TICK(6)


/** LEGACY **/

#define FN_UNDEF \
        bbcLogI("Undefined opcode 0x%02X! pc=%04x", cpu->mem[cpu->pc-1], cpu->pc); \
        TICK(2); 	// Actually lasts forever, but the above code keeps executing HLT forever


// 0x8B: /*Undocumented - ANE*/
#define FN_ANE \
	temp=readmem(cpu->pc); cpu->pc++; \
	cpu->a=(cpu->a|0xEE)&cpu->x&temp; /*Internal parameter always 0xEE on BBC, always 0xFF on Electron*/ \
	setzn(cpu->a); \
	TICK(2); \
	cpu->takeint=(cpu->interrupt && !p_i);

// 0xAB: /*Undocumented - LAX*/
#define  FN_LAX \
	temp=readmem(cpu->pc); cpu->pc++; \
	cpu->a=cpu->x=((cpu->a|0xEE)&temp); /*WAAAAY more complicated than this, but it varies from machine to machine anyway*/ \
	setzn(cpu->a); \
	TICK(2); \
	cpu->takeint=(cpu->interrupt && !p_i);

// 0x00: BRK: 7 cycles
#define FN_BRK \
            bbcLogI("BRK! at %04X", cpu->pc); \
            /*T0*/ TICK1; \
            /*T1*/ TICK1;      cpu->pc++; /* + unused read from PC */ \
            /*T2*/ TICK1;      push(cpu->pc>>8); \
            /*T3*/ TICK1;      push(cpu->pc&0xFF); \
            /*T4*/ TICK1;      push(0x30 | cpu->p); \
                              addr = 0xfffe; \
            /*T5*/ TICK1_ADDR; bal = readmem(addr); addr++; \
            /*T6*/ TICK1_ADDR; bah = readmem(addr); \
                              cpu->pc = bal | (bah<<8); \
                              cpu->p |= FLAG_I; \
                              cpu->takeint=0; \
                   TICK(7);



#ifdef EMSCRIPTEN
#define FORCE_JUMPTABLE 1    // Actually a function table now
#else
#define FORCE_JUMPTABLE 0
#endif

#if FORCE_JUMPTABLE
    typedef void (*POPFN)(M6502*);
    extern POPFN jumpTable[];

    // Locals
    uint16_t addr, addr_c, addr_nc;
    uint8_t val8;
    uint8_t bal, bah, ial;
    uint8_t temp;

    void postOp(M6502* cpu);

#endif
    


void exec6502_c() {
	M6502* cpu = &the_beeb->cpu;
    
    // Ensure the cycle counter doesn't wrap during this burst
    if(cpu->cycles >= 1<<30) {
        cpu->cycles -= 1<<30;
    }

#if !FORCE_JUMPTABLE
        // Locals
        uint16_t addr, addr_c, addr_nc;
        uint8_t val8;
        uint8_t bal, bah, ial;
        uint8_t temp;
#endif

    // Loop for 40,000 cycles (i.e. 1/50th second of BBC Micro @ 2MHz)
    uint32_t cycleTarget = cpu->cycles + 40000;
	while (cpu->cycles < cycleTarget) {
        
#ifdef WANT_LOG_CPU
        TICK_SYNC();
		log_cpu(cpu);
#endif
		//mark_code_cov_byte(cpu->pc,CCB_EXEC);
		
		// Check if any trigger hit
		for (int i=0 ; i<cpu->num_triggers ; i++) {
			if (cpu->pc == cpu->pc_triggers[i].addr) {
				cpu->pc_triggers[i].hit++;
			}
		}


        // Test for successful completion of Kevin Edwards' decoding (see protection.ssd)
        /*if (cpu->pc == 0xe00) {
            bbcLogI("**** YAAAAY ****!");
            exit(0);
        }*/
		
		uint8_t opcode=readmem(cpu->pc);
		cpu->pc++;


#if FORCE_JUMPTABLE
        jumpTable[opcode](cpu);
        postOp(cpu);
        }
    }
#define OPFN(OPCODE, IMPL) void op##OPCODE(M6502* cpu) { IMPL; }
#else
		switch (opcode) {
#define OPFN(OPCODE, IMPL) case OPCODE: IMPL; break;
#endif


        OPFN(0x00, FN_BRK)                       // BRK: 7 cycles
        OPFN(0x01, FN_RO_INDZX(ORA));            // 0x01: ORA (,x)
        OPFN(0x02, FN_UNDEF)
        OPFN(0x03, FN(ADDR_INDZX, ,LD_MEM, SLO, ST_MEM))  // 0x03: SLO (,x)
        OPFN(0x04, FN_RO_ZP(NOP))                // 0x04: NOP zp
        OPFN(0x05, FN_RO_ZP(ORA))		         // 0x05: ORA zp
        OPFN(0x06, FN_RMW_ZP(ASL))               // 0x06: ASL zp
        OPFN(0x07, FN_RMW_ZP(SLO))               // 0x07: SLO zp
        OPFN(0x08, FN_WO_PUSH((cpu->p | 0x30)))  // 0x08: PHP
        OPFN(0x09, FN_RO_IMM(ORA))               // 0x09: ORA imm
        OPFN(0x0A, FN_2(LD_A, ASL, ST_A))        // 0x0A: ASL A
        OPFN(0x0B, FN_RO_IMM(ANC))               // 0x0B: ANC imm
        OPFN(0x0C, FN_RO_ABS(NOP))               // 0x0C: NOP abs
        OPFN(0x0D, FN_RO_ABS(ORA))               // 0x0D: ORA abs
        OPFN(0x0E, FN_RMW_ABS(ASL))              // 0x0E: ASL abs
        OPFN(0x0F, FN_RMW_ABS(SLO))              // 0x0F: SLO abs
        OPFN(0x10, FN_BRANCH(!p_n))               // 0x10: BPL
        OPFN(0x11, FN_RO_INDZY(ORA))             // 0x11: ORA (),y
        OPFN(0x12, FN_UNDEF)
        OPFN(0x13, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, SLO, ST_MEM))   // 0x13: SLO (),y
        OPFN(0x14, FN_RO_ZPXY(cpu->x, NOP))	     // 0x14: NOP zp,x
        OPFN(0x15, FN_RO_ZPXY(cpu->x, ORA))      // 0x15: ORA zp,x
        OPFN(0x16, FN_RMW_ZPXY(cpu->x, ASL))     // 0x16: ASL zp,x
        OPFN(0x17, FN_RMW_ZPXY(cpu->x, SLO))     // 0x17: SLO zp,x
        OPFN(0x18, FN_2(,cpu->p &= ~FLAG_C;,))   // 0x18: CLC
        OPFN(0x19, FN_RO_ABSXY(cpu->y, ORA))     // 0x19: ORA abs,y
        OPFN(0x1A, FN_2( ,    , ))               // 0x1A NOP
        OPFN(0x1B, FN_RMW_ABS_XY(cpu->y, SLO))   // 0x1B: SLO abs,y
        OPFN(0x1C, FN_RO_ABSXY(cpu->x, NOP))     // 0x1C: NOP abs,x
        OPFN(0x1D, FN_RO_ABSXY(cpu->x, ORA))     // 0x1D: ORA abs,x
        OPFN(0x1E, FN_RMW_ABS_XY(cpu->x, ASL))   // 0x1E: ASL abs,x
        OPFN(0x1F, FN_RMW_ABS_XY(cpu->x, SLO))   // 0x1F: SLO abs,x
        OPFN(0x20, FN_JSR)                        // 0x20: JSR
        OPFN(0x21, FN_RO_INDZX(AND))             // 0x21: AND (,x)
        OPFN(0x22, FN_UNDEF)
        OPFN(0x23, FN(ADDR_INDZX, , LD_MEM, RLA, ST_MEM))  // 0x23: RLA (,x)
        OPFN(0x24, FN_RO_ZP(BIT))                // 0x24: BIT zp
        OPFN(0x25, FN_RO_ZP(AND))                // 0x25: AND zp
        OPFN(0x26, FN_RMW_ZP(ROL))               // 0x26: ROL zp
        OPFN(0x27, FN_RMW_ZP(RLA))               // 0x27: RLA zp
        OPFN(0x28, FN_RO_PULL(PLP))              // 0x28: PLP
        OPFN(0x29, FN_RO_IMM(AND))               // 0x29: AND imm
        OPFN(0x2A, FN_2(LD_A, ROL, ST_A))        // 0x2A: ROL A
        OPFN(0x2B, FN_RO_IMM(ANC))               // 0x2B: ANC imm
        OPFN(0x2C, FN_RO_ABS(BIT))               // 0x2C: BIT abs
        OPFN(0x2D, FN_RO_ABS(AND))               // 0x2D: AND abs
        OPFN(0x2E, FN_RMW_ABS(ROL))              // 0x2E: ROL abs
        OPFN(0x2F, FN_RMW_ABS(RLA))              // 0x2F: RLA abs    Undocumented, found in The Hobbit & Dare Devil Denis
        OPFN(0x30, FN_BRANCH(p_n))                // 0x30: BMI
        OPFN(0x31, FN_RO_INDZY(AND))             // 0x31: AND (),y
        OPFN(0x32, FN_UNDEF)
        OPFN(0x33, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, RLA, ST_MEM))   // 0x33: RLA (),y
        OPFN(0x34, FN_RO_ZPXY(cpu->x, NOP))	     // 0x34: NOP zp,x
        OPFN(0x35, FN_RO_ZPXY(cpu->x, AND))      // 0x35: AND zp,x
        OPFN(0x36, FN_RMW_ZPXY(cpu->x, ROL))     // 0x36: ROL zp,x
        OPFN(0x37, FN_RMW_ZPXY(cpu->x, RLA))     // 0x37: RLA zp,x
        OPFN(0x38, FN_2( ,cpu->p |= FLAG_C;, ))  // 0x38: SEC
        OPFN(0x39, FN_RO_ABSXY(cpu->y, AND))     // 0x39: AND abs,y
        OPFN(0x3A, FN_2( ,    , ))               // 0x3A NOP
        OPFN(0x3B, FN_RMW_ABS_XY(cpu->y, RLA))   // 0x3B: RLA abs,y
        OPFN(0x3C, FN_RO_ABSXY(cpu->x, NOP))     // 0x3C: NOP abs,x
        OPFN(0x3D, FN_RO_ABSXY(cpu->x, AND))     // 0x3D: AND abs,x
        OPFN(0x3E, FN_RMW_ABS_XY(cpu->x, ROL))   // 0x3E: ROL abs,x
        OPFN(0x3F, FN_RMW_ABS_XY(cpu->x, RLA))   // 0x3F: RLA abs,x
        OPFN(0x40, FN_RTI)                        // 0x40: RTI
        OPFN(0x41, FN_RO_INDZX(EOR))             // 0x41: EOR (,x)
        OPFN(0x42, FN_UNDEF)
        OPFN(0x43, FN(ADDR_INDZX, ,LD_MEM, SRE, ST_MEM)) // 0x43: SRE (,x)
        OPFN(0x44, FN_RO_ZP(NOP))                // 0x44: NOP zp
        OPFN(0x45, FN_RO_ZP(EOR))                // 0x45: EOR zp
        OPFN(0x46, FN_RMW_ZP(LSR))               // 0x46: LSR zp
        OPFN(0x47, FN_RMW_ZP(SRE))               // 0x47: SRE zp
        OPFN(0x48, FN_WO_PUSH(cpu->a))           // 0x48: PHA
        OPFN(0x49, FN_RO_IMM(EOR))               // 0x49: EOR imm
        OPFN(0x4A, FN_2(LD_A, LSR, ST_A))        // 0x4A: LSR A
        OPFN(0x4B, FN_RO_IMM(ASR))               // 0x4B: ASR imm
        OPFN(0x4C, FN_JMP)                        // 0x4C: JMP
        OPFN(0x4D, FN_RO_ABS(EOR))               // 0x4D: EOR abs
        OPFN(0x4E, FN_RMW_ABS(LSR))              // 0x4E: LSR abs
        OPFN(0x4F, FN_RMW_ABS(SRE))              // 0x4F: SRE abs
        OPFN(0x50, FN_BRANCH(!p_v))               // 0x50: BVC
        OPFN(0x51, FN_RO_INDZY(EOR))             // 0x51: EOR (),y
        OPFN(0x52, FN_UNDEF)
        OPFN(0x53, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, SRE, ST_MEM)) // 0x53: SRE (),y
        OPFN(0x54, FN_RO_ZPXY(cpu->x, NOP))	     // 0x54: NOP zp,x
        OPFN(0x55, FN_RO_ZPXY(cpu->x, EOR))      // 0x55: EOR zp,x
        OPFN(0x56, FN_RMW_ZPXY(cpu->x, LSR))     // 0x56: LSR zp,x
        OPFN(0x57, FN_RMW_ZPXY(cpu->x, SRE))     // 0x57: SRE zp,x
        OPFN(0x58, FN_2(, cpu->p &= ~FLAG_I;,))  // 0x58: CLI
        OPFN(0x59, FN_RO_ABSXY(cpu->y, EOR))     // 0x59: EOR abs,y
        OPFN(0x5A, FN_2( ,    , ))               // 0x5A NOP
        OPFN(0x5B, FN_RMW_ABS_XY(cpu->y, SRE))   // 0x5B: SRE abs,y
        OPFN(0x5C, FN_RO_ABSXY(cpu->x, NOP))     // 0x5C: NOP abs,x
        OPFN(0x5D, FN_RO_ABSXY(cpu->x, EOR))     // 0x5D: EOR abs,x
        OPFN(0x5E, FN_RMW_ABS_XY(cpu->x, LSR))   // 0x5E: LSR abs,x
        OPFN(0x5F, FN_RMW_ABS_XY(cpu->x, SRE))   // 0x5F: SRE abs,x
        OPFN(0x60, FN_RTS)                        // 0x60: RTS
        OPFN(0x61, FN_RO_INDZX(ADC))             // 0x61: ADC (,x)
        OPFN(0x62, FN_UNDEF)
        OPFN(0x63, FN(ADDR_INDZX, ,LD_MEM, RRA, ST_MEM)) // 0x63: RRA (,x)
        OPFN(0x64, FN_RO_ZP(NOP))                // 0x64: NOP zp
        OPFN(0x65, FN_RO_ZP(ADC))                // 0x65: ADC zp
        OPFN(0x66, FN_RMW_ZP(ROR))               // 0x66: ROR zp
        OPFN(0x67, FN_RMW_ZP(RRA))               // 0x67: RRA zp
        OPFN(0x68, FN_RO_PULL(PLA))              // 0x68: PLA
        OPFN(0x69, FN_RO_IMM(ADC))               // 0x69: ADC imm
        OPFN(0x6A, FN_2(LD_A, ROR, ST_A))        // 0x6A: ROR A
        OPFN(0x6B, FN_RO_IMM(ARR))               // 0x6B: ARR imm
        OPFN(0x6C, FN_JMP_IND)                    // 0x6C: JMP ()
        OPFN(0x6D, FN_RO_ABS(ADC))               // 0x6D: ADC abs
        OPFN(0x6E, FN_RMW_ABS(ROR))              // 0x6E: ROR abs
        OPFN(0x6F, FN_RMW_ABS(RRA))              // 0x6F: RRA abs
        OPFN(0x70, FN_BRANCH(p_v))                // 0x70: BVS
        OPFN(0x71, FN_RO_INDZY(ADC))             // 0x71: ADC (),y
        OPFN(0x72, FN_UNDEF)
        OPFN(0x73, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, RRA, ST_MEM)) // 0x73: RRA (),y
        OPFN(0x74, FN_RO_ZPXY(cpu->x, NOP))	     // 0x74: NOP zp,x
        OPFN(0x75, FN_RO_ZPXY(cpu->x, ADC))      // 0x75: ADC zp,x
        OPFN(0x76, FN_RMW_ZPXY(cpu->x, ROR))     // 0x76: ROR zp,x
        OPFN(0x77, FN_RMW_ZPXY(cpu->x, RRA))     // 0x77: RRA zp,x
        OPFN(0x78, FN_2( , cpu->p |= FLAG_I;, )) // 0x78: SEI
        OPFN(0x79, FN_RO_ABSXY(cpu->y, ADC))     // 0x79: ADC abs,y
        OPFN(0x7A, FN_2( ,    , ))               // 0x7A NOP
        OPFN(0x7B, FN_RMW_ABS_XY(cpu->y, RRA))   // 0x7B: RRA abs,y
        OPFN(0x7C, FN_RO_ABSXY(cpu->x, NOP))     // 0x7C: NOP abs,x
        OPFN(0x7D, FN_RO_ABSXY(cpu->x, ADC))     // 0x7D: ADC abs,x
        OPFN(0x7E, FN_RMW_ABS_XY(cpu->x, ROR))   // 0x7E: ROR abs,x
        OPFN(0x7F, FN_RMW_ABS_XY(cpu->x, RRA))   // 0x7F: RRA abs,x
        OPFN(0x80, FN_RO_IMM(NOP))               // 0x80: NOP imm
        OPFN(0x81, FN_WO_INDZX(LD_A))            // 0x81: STA (,x)
        OPFN(0x82, FN_RO_IMM(NOP))               // 0x82: NOP imm
        OPFN(0x83, FN_WO_INDZX(LD_AX))           // 0x83: SAX (,x)
        OPFN(0x84, FN_WO_ZP(LD_Y))               // 0x84: STY zp
        OPFN(0x85, FN_WO_ZP(LD_A))               // 0x85: STA zp
        OPFN(0x86, FN_WO_ZP(LD_X))               // 0x86: STX zp
        OPFN(0x87, FN_WO_ZP(LD_AX))              // 0x87: SAX zp
        OPFN(0x88, FN_2(, cpu->y--; setzn(cpu->y);,)) // 0x88: DEY
        OPFN(0x89, FN_RO_IMM(NOP))               // 0x89: NOP imm
        OPFN(0x8A, FN_2(, cpu->a=cpu->x; setzn(cpu->a);, )) // 0x8A: TXA
        OPFN(0x8B, FN_ANE)                        // 0x8B: ANE
        OPFN(0x8C, FN_WO_ABS(LD_Y))              // 0x8C: STY abs
        OPFN(0x8D, FN_WO_ABS(LD_A))              // 0x8D: STA abs
        OPFN(0x8E, FN_WO_ABS(LD_X))              // 0x8E: STX abs
        OPFN(0x8F, FN_WO_ABS(LD_AX))             // 0x8F: SAX abs
        OPFN(0x90, FN_BRANCH(!p_c))               // 0x90: BCC
        OPFN(0x91, FN_WO_INDZY(LD_A))            // 0x91: STA (),y
        OPFN(0x92, FN_UNDEF)
        OPFN(0x93, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, SHA, ST_MEM))   // 0x93: SHA (),y
        OPFN(0x94, FN_WO_ZPXY(cpu->x, LD_Y))     // 0x94: STY zp,x
        OPFN(0x95, FN_WO_ZPXY(cpu->x, LD_A))	 // 0x95: STA zp,x
        OPFN(0x96, FN_WO_ZPXY(cpu->y, LD_X))	 // 0x96: STX zp,y
        OPFN(0x97, FN_WO_ZPXY(cpu->y, LD_AX))	 // 0x97: SAX zp,y
        OPFN(0x98, FN_2(, cpu->a=cpu->y; setzn(cpu->a);, )) // 0x98: TYA
        OPFN(0x99, FN_WO_ABS_XY(cpu->y, LD_A))	 // 0x99: STA abs,y
        OPFN(0x9A, FN_2(, cpu->s=cpu->x; , ))    // 0x9A: TXS
        OPFN(0x9B, FN_RMW_ABS_XY(cpu->y, SHS))	 // 0x9B: SHS abs,y
        OPFN(0x9C, FN_RMW_ABS_XY(cpu->x, SHY))   // 0x9C: SHY abs,x
        OPFN(0x9D, FN_WO_ABS_XY(cpu->x, LD_A))   // 0x9D: STA abs,x
        OPFN(0x9E, FN_RMW_ABS_XY(cpu->y, SHX))   // 0x9E: SHX abs,y
        OPFN(0x9F, FN_RMW_ABS_XY(cpu->y, SHA))   // 0x9F: SHA abs,y
        OPFN(0xA0, FN_RO_IMM(LDY))               // 0xA0: LDY imm
        OPFN(0xA1, FN_RO_INDZX(LDA))             // 0xA1: LDA (,x)
        OPFN(0xA2, FN_RO_IMM(LDX))               // 0xA2: LDX imm
        OPFN(0xA3, FN_RO_INDZX(LAX))             // 0xA3: LAX (,x)
        OPFN(0xA4, FN_RO_ZP(LDY))                // 0xA4: LDY zp
        OPFN(0xA5, FN_RO_ZP(LDA))                // 0xA5: LDA zp
        OPFN(0xA6, FN_RO_ZP(LDX))                // 0xA6: LDX zp
        OPFN(0xA7, FN_RO_ZP(LAX))                // 0xA7: LAX zp
        OPFN(0xA8, FN_2(, cpu->y=cpu->a; setzn(cpu->y);, )) // 0xA8: TAY
        OPFN(0xA9, FN_RO_IMM(LDA))               // 0xA9: LDA imm
        OPFN(0xAA, FN_2(, cpu->x=cpu->a; setzn(cpu->x);, )) // 0xAA: TAX
        OPFN(0xAB, FN_LAX)                        // 0xAB: LAXS
        OPFN(0xAC, FN_RO_ABS(LDY))               // 0xAC: LDY abs
        OPFN(0xAD, FN_RO_ABS(LDA))               // 0xAD: LDA abs
        OPFN(0xAE, FN_RO_ABS(LDX))               // 0xAE: LDX abs
        OPFN(0xAF, FN_RO_ABS(LAX))               // 0xAF: LAX abs
        OPFN(0xB0, FN_BRANCH(p_c))                // 0xB0: BCS
        OPFN(0xB1, FN_RO_INDZY(LDA))             // 0xB1: LDA (),y
        OPFN(0xB2, FN_UNDEF)
        OPFN(0xB3, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, LAX, ST_MEM)) // 0xB3: LAX (),y
        OPFN(0xB4, FN_RO_ZPXY(cpu->x, LDY))      // 0xB4: LDY zp,x
        OPFN(0xB5, FN_RO_ZPXY(cpu->x, LDA))      // 0xB5: LDA zp,x
        OPFN(0xB6, FN_RO_ZPXY(cpu->y, LDX))      // 0xB6: LDX zp,y
        OPFN(0xB7, FN_RO_ZPXY(cpu->y, LAX))      // 0xB7: LAX zp,y
        OPFN(0xB8, FN_2(, cpu->p &= ~FLAG_V;,))  // 0xB8: CLV
        OPFN(0xB9, FN_RO_ABSXY(cpu->y, LDA))	 // 0xB9: LDA abs,y
        OPFN(0xBA, FN_2(, cpu->x=cpu->s; setzn(cpu->x);, )) // 0xBA: TSX
        OPFN(0xBB, FN_RO_ABSXY(cpu->y, LAS))	 // 0xBB: LAS abs,y
        OPFN(0xBC, FN_RO_ABSXY(cpu->x, LDY))	 // 0xBC: LDY abs,x
        OPFN(0xBD, FN_RO_ABSXY(cpu->x, LDA))     // 0xBD: LDA abs,x
        OPFN(0xBE, FN_RO_ABSXY(cpu->y, LDX))     // 0xBE: LDX abs,y
        OPFN(0xBF, FN_RO_ABSXY(cpu->y, LAX))     // 0xBF: LAX abs,y
        OPFN(0xC0, FN_RO_IMM(CPY))               // 0xC0: CPY imm
        OPFN(0xC1, FN_RO_INDZX(CMP))             // 0xC1: CMP (,x)
        OPFN(0xC2, FN_RO_IMM(NOP))               // 0xC2: NOP imm
        OPFN(0xC3, FN(ADDR_INDZX, , LD_MEM, DCP, ST_MEM))   // 0xC3: DCP (,x)
        OPFN(0xC4, FN_RO_ZP(CPY))                // 0xC4: CPY zp
        OPFN(0xC5, FN_RO_ZP(CMP))                // 0xC5: CMP zp
        OPFN(0xC6, FN_RMW_ZP(DEC))               // 0xC6: DEC zp
        OPFN(0xC7, FN_RMW_ZP(DCP))               // 0xC7: DCP zp
        OPFN(0xC8, FN_2(, cpu->y++; setzn(cpu->y);,)) // 0xC8: INY
        OPFN(0xC9, FN_RO_IMM(CMP))               // 0xC9: CMP imm
        OPFN(0xCA, FN_2(, cpu->x--; setzn(cpu->x);,)) // 0xCA: DEX
        OPFN(0xCB, FN_RO_IMM(SBX))               // 0xCB: SBX imm
        OPFN(0xCC, FN_RO_ABS(CPY))               // 0xCC: CPY abs
        OPFN(0xCD, FN_RO_ABS(CMP))               // 0xCD: CMP abs
        OPFN(0xCE, FN_RMW_ABS(DEC))              // 0xCE: DEC abs
        OPFN(0xCF, FN_RMW_ABS(DCP))              // 0xCF: DCP abs
        OPFN(0xD0, FN_BRANCH(!p_z))               // 0xD0: BNE
	    OPFN(0xD1, FN_RO_INDZY(CMP))             // 0xD1: CMP (),y
	    OPFN(0xD2, FN_UNDEF)
	    OPFN(0xD3, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, DCP, ST_MEM)) // 0xD3: DCP (),y
        OPFN(0xD4, FN_RO_ZPXY(cpu->x, NOP))      // 0xD4: NOP zp,x
	    OPFN(0xD5, FN_RO_ZPXY(cpu->x, CMP))      // 0xD5: CMP zp,x
	    OPFN(0xD6, FN_RMW_ZPXY(cpu->x, DEC))	 // 0xD6: DEC zp,x
	    OPFN(0xD7, FN_RMW_ZPXY(cpu->x, DCP))	 // 0xD7: DCP zp,x
	    OPFN(0xD8, FN_2(, cpu->p &= ~FLAG_D;,))  // 0xD8: CLD
	    OPFN(0xD9, FN_RO_ABSXY(cpu->y, CMP))	 // 0xD9: CMP abs,y
        OPFN(0xDA, FN_2( ,    , ))               // 0xDA NOP
	    OPFN(0xDB, FN_RMW_ABS_XY(cpu->y, DCP))   // 0xDB: DCP abs,y
        OPFN(0xDC, FN_RO_ABSXY(cpu->x, NOP))     // 0xDC: NOP abs,x
	    OPFN(0xDD, FN_RO_ABSXY(cpu->x, CMP))	 // 0xDD: CMP abs,x
	    OPFN(0xDE, FN_RMW_ABS_XY(cpu->x, DEC))	 // 0xDE: DEC abs,x
	    OPFN(0xDF, FN_RMW_ABS_XY(cpu->x, DCP))	 // 0xDF: DCP abs,x
	    OPFN(0xE0, FN_RO_IMM(CPX))               // 0xE0: CPX imm
	    OPFN(0xE1, FN_RO_INDZX(SBC))             // 0xE1: SBC (,x)
        OPFN(0xE2, FN_RO_IMM(NOP))               // 0xE2: NOP imm
	    OPFN(0xE3, FN(ADDR_INDZX, , LD_MEM, ISB, ST_MEM)) // 0xE3: ISB (,x)
	    OPFN(0xE4, FN_RO_ZP(CPX))                // 0xE4: CPX zp
	    OPFN(0xE5, FN_RO_ZP(SBC))                // 0xE5: SBC zp
	    OPFN(0xE6, FN_RMW_ZP(INC))               // 0xE6: INC zp
	    OPFN(0xE7, FN_RMW_ZP(ISB))               // 0xE7: ISB zp
	    OPFN(0xE8, FN_2(, cpu->x++; setzn(cpu->x);,)) // 0xE8: INX
	    OPFN(0xE9, FN_RO_IMM(SBC))               // 0xE9: SBC imm
        OPFN(0xEA, FN_2( ,    , ))               // 0xEA NOP
	    OPFN(0xEB, FN_RO_IMM(SBC))               // 0xEB: SBC imm
	    OPFN(0xEC, FN_RO_ABS(CPX))               // 0xEC: CPX abs
	    OPFN(0xED, FN_RO_ABS(SBC))               // 0xED: SBC abs
	    OPFN(0xEE, FN_RMW_ABS(INC))              // 0xEE: INC abs
	    OPFN(0xEF, FN_RMW_ABS(ISB))              // 0xEF: ISB abs
	    OPFN(0xF0, FN_BRANCH(p_z))                // 0xF0: BEQ
	    OPFN(0xF1, FN_RO_INDZY(SBC))             // 0xF1: SBC (),y
	    OPFN(0xF2, FN_UNDEF)
	    OPFN(0xF3, FN(ADDR_INDZ, OFFSET_Y, LD_MEM, ISB, ST_MEM)) // 0xF3: ISB (),y
        OPFN(0xF4, FN_RO_ZPXY(cpu->x, NOP))      // 0xF4: NOP zp,x
        OPFN(0xF5, FN_RO_ZPXY(cpu->x, SBC))      // 0xF5: SBC zp,x
	    OPFN(0xF6, FN_RMW_ZPXY(cpu->x, INC))	 // 0xF6: INC zp,x
	    OPFN(0xF7, FN_RMW_ZPXY(cpu->x, ISB))	 // 0xF7: ISB zp,x
	    OPFN(0xF8, FN_2(, cpu->p |= FLAG_D;,))   // 0xF8: SED
	    OPFN(0xF9, FN_RO_ABSXY(cpu->y, SBC))     // 0xF9: SBC abs,y
        OPFN(0xFA, FN_2( ,    , ))               // 0xFA NOP
	    OPFN(0xFB, FN_RMW_ABS_XY(cpu->y, ISB))   // 0xFB: ISB abs,y
        OPFN(0xFC, FN_RO_ABSXY(cpu->x, NOP))     // 0xFC: NOP abs,x
	    OPFN(0xFD, FN_RO_ABSXY(cpu->x, SBC))	 // 0xFD: SBC abs,x
	    OPFN(0xFE, FN_RMW_ABS_XY(cpu->x, INC))   // 0xFE: INC abs,x
	    OPFN(0xFF, FN_RMW_ABS_XY(cpu->x, ISB)) 	 // 0xFF: ISB abs,x

#if FORCE_JUMPTABLE
    void postOp(M6502* cpu) {
#else
        } // closes 'switch'
#endif
		
		if (cpu->takeint) {
			cpu->takeint=0;
			push(cpu->pc>>8);
			push(cpu->pc&0xFF);
			push(cpu->p | 0x20);
			cpu->pc=readmem(0xFFFE)|(readmem(0xFFFF)<<8);
			cpu->p |= FLAG_I;
			cpu->pendingCycles += 7;
		}
		
		if (cpu->nmi) {
			cpu->nmi=0;
			push(cpu->pc>>8);
			push(cpu->pc&0xFF);
			push(cpu->p | 0x20);
			cpu->pc=readmem(0xFFFA)|(readmem(0xFFFB)<<8);
			cpu->p |= FLAG_I;
			cpu->pendingCycles += 7;
		}
	}

#if FORCE_JUMPTABLE
	
POPFN jumpTable[256] ={
    op0x00, op0x01, op0x02, op0x03, op0x04, op0x05, op0x06, op0x07,
    op0x08, op0x09, op0x0A, op0x0B, op0x0C, op0x0D, op0x0E, op0x0F,
    op0x10, op0x11, op0x12, op0x13, op0x14, op0x15, op0x16, op0x17,
    op0x18, op0x19, op0x1A, op0x1B, op0x1C, op0x1D, op0x1E, op0x1F,
    op0x20, op0x21, op0x22, op0x23, op0x24, op0x25, op0x26, op0x27,
    op0x28, op0x29, op0x2A, op0x2B, op0x2C, op0x2D, op0x2E, op0x2F,
    op0x30, op0x31, op0x32, op0x33, op0x34, op0x35, op0x36, op0x37,
    op0x38, op0x39, op0x3A, op0x3B, op0x3C, op0x3D, op0x3E, op0x3F,
    op0x40, op0x41, op0x42, op0x43, op0x44, op0x45, op0x46, op0x47,
    op0x48, op0x49, op0x4A, op0x4B, op0x4C, op0x4D, op0x4E, op0x4F,
    op0x50, op0x51, op0x52, op0x53, op0x54, op0x55, op0x56, op0x57,
    op0x58, op0x59, op0x5A, op0x5B, op0x5C, op0x5D, op0x5E, op0x5F,
    op0x60, op0x61, op0x62, op0x63, op0x64, op0x65, op0x66, op0x67,
    op0x68, op0x69, op0x6A, op0x6B, op0x6C, op0x6D, op0x6E, op0x7F,
    op0x70, op0x71, op0x72, op0x73, op0x74, op0x75, op0x76, op0x77,
    op0x78, op0x79, op0x7A, op0x7B, op0x7C, op0x7D, op0x7E, op0x7F,
    op0x80, op0x81, op0x82, op0x83, op0x84, op0x85, op0x86, op0x87,
    op0x88, op0x89, op0x8A, op0x8B, op0x8C, op0x8D, op0x8E, op0x8F,
    op0x90, op0x91, op0x92, op0x93, op0x94, op0x95, op0x96, op0x97,
    op0x98, op0x99, op0x9A, op0x9B, op0x9C, op0x9D, op0x9E, op0x9F,
    op0xA0, op0xA1, op0xA2, op0xA3, op0xA4, op0xA5, op0xA6, op0xA7,
    op0xA8, op0xA9, op0xAA, op0xAB, op0xAC, op0xAD, op0xAE, op0xAF,
    op0xB0, op0xB1, op0xB2, op0xB3, op0xB4, op0xB5, op0xB6, op0xB7,
    op0xB8, op0xB9, op0xBA, op0xBB, op0xBC, op0xBD, op0xBE, op0xBF,
    op0xC0, op0xC1, op0xC2, op0xC3, op0xC4, op0xC5, op0xC6, op0xC7,
    op0xC8, op0xC9, op0xCA, op0xCB, op0xCC, op0xCD, op0xCE, op0xCF,
    op0xD0, op0xD1, op0xD2, op0xD3, op0xD4, op0xD5, op0xD6, op0xD7,
    op0xD8, op0xD9, op0xDA, op0xDB, op0xDC, op0xDD, op0xDE, op0xDF,
    op0xE0, op0xE1, op0xE2, op0xE3, op0xE4, op0xE5, op0xE6, op0xE7,
    op0xE8, op0xE9, op0xEA, op0xEB, op0xEC, op0xED, op0xEE, op0xEF,
    op0xF0, op0xF1, op0xF2, op0xF3, op0xF4, op0xF5, op0xF6, op0xF7,
    op0xF8, op0xF9, op0xFA, op0xFB, op0xFC, op0xFD, op0xFE, op0xFF,
};
    
#else
}
#endif


}
