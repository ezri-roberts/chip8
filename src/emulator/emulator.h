#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>

#include "../renderer/renderer.h"

typedef struct {

	unsigned char memory[4096]; // 4KB / 4096 bytes of RAM.
	uint8_t V[16]; // 8-bit registers. V0-VF
	uint16_t I;
	uint16_t pc; // Currently executing address.
	
	uint16_t opcode; // The current opcode;
	
	uint16_t stack[16];
	uint8_t sp; // Points to top of stack.
	
	unsigned char frameBuffer[64*32]; // 64x32-pixel monochrome display.
	
	uint16_t delayTimer;
	uint16_t soundTimer;

	Renderer renderer;
	bool stepMode;
	
} Emulator;

/*
	* All instructions are 2 bytes long and are stored most-significant-byte first.
	* In memory, the first byte of each instruction should be located at an even addresses.
	* If a program includes sprite data,
	* it should be padded so any instructions following it will be properly situated in RAM.
*/

void emulatorInit(Emulator *em, bool stepMode);
void emulatorLoad(Emulator *em, const char *name);
void emulatorCycle(Emulator *em);
void emulatorUpate(Emulator *em);

void opcodePrint(Emulator *em, const char *msg);

void opcodePrefixZero(Emulator *em, const uint16_t parts[6]);
void opcodePrefixOne(Emulator *em, const uint16_t parts[6]);
void opcodePrefixTwo(Emulator *em, const uint16_t parts[6]);
void opcodePrefixThree(Emulator *em, const uint16_t parts[6]);
void opcodePrefixFour(Emulator *em, const uint16_t parts[6]);
void opcodePrefixFive(Emulator *em, const uint16_t parts[6]);
void opcodePrefixSix(Emulator *em, const uint16_t parts[6]);
void opcodePrefixSeven(Emulator *em, const uint16_t parts[6]);
void opcodePrefixEight(Emulator *em, const uint16_t parts[6]);
void opcodePrefixNine(Emulator *em, const uint16_t parts[6]);
void opcodePrefixA(Emulator *em, const uint16_t parts[6]);
void opcodePrefixB(Emulator *em, const uint16_t parts[6]);
void opcodePrefixC(Emulator *em, const uint16_t parts[6]);
void opcodePrefixD(Emulator *em, const uint16_t parts[6]);
void opcodePrefixE(Emulator *em, const uint16_t parts[6]);
void opcodePrefixF(Emulator *em, const uint16_t parts[6]);

void frameBufferClear(Emulator *em);
void frameBufferPut(Emulator *em, uint8_t x, uint8_t y);

#endif // !EMULATOR_H
