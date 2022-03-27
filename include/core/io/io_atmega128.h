#ifndef _IO_ATMEGA128_H_
#define _IO_ATMEGA128_H_

#define INPUT							0x00
#define OUTPUT							0xFF

#define ALL_ZERO						0x00
#define ALL_ONE							0xFF

#define SET_INPUT(x,y)						(y&=(~(1<<x)))
#define SET_OUTPUT(x,y)						(y|=(1<<x))

#endif
