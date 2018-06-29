//
//  dis.h
//  libbeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef dis_h
#define dis_h

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
void disassemble(uint16_t *pc, char *output, uint8_t *buffer);
#ifdef __cplusplus
	}
#endif

#endif /* dis_h */
