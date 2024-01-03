#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {

	unsigned char memory[4096]; // 4KB / 4096 bytes of RAM.
	uint8_t V[16]; // 8-bit registers. V0-VF
	uint16_t I;
	uint16_t pc; // Currently executing address.
	
	uint16_t opcode; // The current opcode;
	
	uint16_t stack[16];
	uint8_t sp; // Points to top of stack.
	
	unsigned char display[64 * 32]; // 64x32-pixel monochrome display.
	
	uint16_t delayTimer;
	uint16_t soundTimer;
	
} Emulator;

/*
	* All instructions are 2 bytes long and are stored most-significant-byte first.
	* In memory, the first byte of each instruction should be located at an even addresses.
	* If a program includes sprite data,
	* it should be padded so any instructions following it will be properly situated in RAM.
*/

void emulatorInit(Emulator *em);
void emulatorCycle(Emulator *em);
void emulatorLoad(Emulator *em, const char *name);

void opcodeClearOrReturn(Emulator *em);

#endif // !EMULATOR_H
