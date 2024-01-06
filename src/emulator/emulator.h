#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>

#include "../renderer/renderer.h"

#define code(a, b) ((a << 8) | b)
#define first(opcode) (opcode & 0xF000) // first e.g. '5'xy0
#define second(opcode) ((opcode >> 8) & 0x0F) // x e.g. 5'x'y0
#define third(opcode) ((opcode >> 4) & 0x0F) // y e.g. 5x'y'0
#define fourth(opcode) (opcode & 0x0F) // last e.g. 5xy'0'
#define nnn(opcode) (opcode & 0x0FFF) // nnn e.g. 1'nnn' 
#define kk(opcode) (opcode & 0x0FF) // kk e.g. 6x'kk'

typedef struct {

	uint16_t opcode;
	uint16_t first;
	uint16_t second;
	uint16_t third;
	uint16_t fourth;
	uint16_t nnn;
	uint16_t kk;
} Instruction;

typedef struct {

	uint8_t memory[4096]; // 4KB / 4096 bytes of RAM.
	uint8_t V[16];		  // 8-bit registers. V0-VF
	uint16_t I;			  // Index register.
	uint16_t pc;		  // Currently executing address.
	
	Instruction instr; // The currently executing instruction.
	
	uint16_t stack[16];
	uint16_t sp; // Points to top of stack.
	
	bool frameBuffer[64*32]; // 64x32-pixel monochrome display.
	
	bool keypad[16];
	
	uint8_t delayTimer; // Decrements when > 0
	uint8_t soundTimer; // Decrements and plays a tone when > 0

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

void opcodePrint(Instruction *instr, const char *msg);

void opcodePrefixZero(Emulator *em);
void opcodePrefixOne(Emulator *em);
void opcodePrefixTwo(Emulator *em);
void opcodePrefixThree(Emulator *em);
void opcodePrefixFour(Emulator *em);
void opcodePrefixFive(Emulator *em);
void opcodePrefixSix(Emulator *em);
void opcodePrefixSeven(Emulator *em);
void opcodePrefixEight(Emulator *em);
void opcodePrefixNine(Emulator *em);
void opcodePrefixA(Emulator *em);
void opcodePrefixB(Emulator *em);
void opcodePrefixC(Emulator *em);
void opcodePrefixD(Emulator *em);
void opcodePrefixE(Emulator *em);
void opcodePrefixF(Emulator *em);

void frameBufferClear(Emulator *em);
void frameBufferPut(Emulator *em, uint8_t x, uint8_t y);

#endif // !EMULATOR_H
