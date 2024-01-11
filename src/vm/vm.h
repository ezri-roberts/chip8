#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdbool.h>

#include "../renderer/renderer.h"

// Combine two bytes into a full opcode.
#define OPCODE(a, b) ((a << 8) | b)
// Mask off the first nibble of the opcode. e.g. '5'xy0
#define FIRST(opcode) ((opcode >> 12) & 0x0F)
// Mask off the second nibble of the opcode. e.g. 5'x'y0
#define X(opcode) ((opcode >> 8) & 0x0F)
// Mask off the third nibble of the opcode. e.g. 5x'y'0
#define Y(opcode) ((opcode >> 4) & 0x0F)
// Mask off the last nibble of the opcode. e.g. 5xy'0'
#define N(opcode) (opcode & 0x0F)
// Mask off everything except the first nibble of the opcode. e.g. 1'nnn' 
#define NNN(opcode) (opcode & 0x0FFF)
// Mask off the last byte of the opcode. e.g. 6x'kk'
#define KK(opcode) (opcode & 0x0FF)

typedef struct Instruction {
	uint16_t opcode; // The full opcode.
	uint16_t first;  // The first nibble.
	uint16_t x;		 // The second nibble.
	uint16_t y;		 // The third nibble.
	uint16_t n;		 // The last nibble.
	uint16_t nnn;	 // All but the first nibble.
	uint16_t kk;	 // The last byte.
} Instruction;

typedef enum {
	RUNNING,
	PAUSED,
} State;

typedef struct Vm {

	uint8_t memory[4096]; // 4KB/4096 bytes of RAM.
	uint8_t V[16];		  // 8-bit registers. V0-VF
	uint16_t I;			  // Index register.
	uint16_t pc;		  // Address of currently executing instruction.
	
	Instruction inst; // The currently executing instruction.
	
	// The stack stores the address to return to when finished with a subroutine.
	// Allows 16 levels of nested subroutines.
	uint16_t stack[16];
	uint16_t sp; // Points to top of stack.
	
	bool framebuffer[64*32]; // 64x32-pixel monochrome display.
	/*
	 * ---------------------
	 * |(0, 0)		(63, 0)|
	 * |				   |
	 * |				   |
	 * |(0, 31)	   (63, 31)|
	 * ---------------------
	*/
	
	bool keypad[16];
	
	uint8_t delay_timer; // Decrements when > 0
	uint8_t sound_timer; // Decrements and plays a tone when > 0
	
	uint32_t clock_rate;

	Renderer renderer;

	State state;
} Vm;

void vm_init(Vm *vm);
void vm_load_rom(Vm *vm, const char *name);
void vm_cycle(Vm *vm);
void vm_update(Vm *vm);
void vm_input_update(Vm *vm);
bool vm_input_any(Vm *vm);
void vm_framebuffer_clear(Vm *vm);

void instruction_print(Instruction *instr, const char *msg);

void opcodeZero(Vm *vm);
void opcodeOne(Vm *vm);
void opcodeTwo(Vm *vm);
void opcodeThree(Vm *vm);
void opcodeFour(Vm *vm);
void opcodeFive(Vm *vm);
void opcodeSix(Vm *vm);
void opcodeSeven(Vm *vm);
void opcodeEight(Vm *vm);
void opcodeNine(Vm *vm);
void opcodeA(Vm *vm);
void opcodeB(Vm *vm);
void opcodeC(Vm *vm);
void opcodeD(Vm *vm);
void opcodeE(Vm *vm);
void opcodeF(Vm *vm);

void inputCheck(Vm *vm);
bool inputAny(Vm *vm);

#endif // !VM_H
