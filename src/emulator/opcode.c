#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"

void opcodePrint(Emulator *em, const char *msg) {

	const uint16_t first = (em->opcode & 0xF000); // e.g. '5'xy0
	const uint16_t x     = (em->opcode & 0x0F00); // e.g. 5'x'y0
	const uint16_t y     = (em->opcode & 0x00F0); // e.g. 5x'y'0
	const uint16_t last  = (em->opcode & 0x000F); // e.g. 5xy'0'
	const uint16_t nnn   = (em->opcode & 0x0FFF); // e.g. 1'nnn' 
	const uint16_t kk    = (em->opcode & 0x00FF); // e.g. 6x'kk'
	
	printf("%X | %X, %X, %X, %X | nnn: %X, kk: %X | '%s'\n",
		em->opcode, first, x, y, last, nnn, kk, msg);
}

/*
* Increment the program counter by two after every executed opcode.
* This is true unless you jump to a certain address in the memory or if you call a subroutine.
* (in which case you need to store the program counter in the stack).
* If the next opcode should be skipped, increase the program counter by four.
*/

void opcodePrefixZero(Emulator *em, const uint16_t parts[6]) {

	switch (parts[3]) {
		case 0x0000:
			opcodePrint(em, "CLS.");
			frameBufferClear(em);
			em->pc += 2;
		break;
		case 0x000E:
			opcodePrint(em, "RET.");
			em->pc = em->stack[em->sp];
			em->sp --;
		break;
	}
}

void opcodePrefixOne(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "JP addr.");
	em->pc = parts[4];
}

void opcodePrefixTwo(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "CALL addr.");
	em->sp ++;
	em->stack[em->sp] = em->pc;
	em->pc = parts[4];
}

void opcodePrefixThree(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "SE Vx, byte.");

	if (em->V[parts[1]] == parts[5]) {
		em->pc += 4;	
	}
}

void opcodePrefixFour(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "SNE Vx, byte.");

	if (em->V[parts[1]] != parts[5]) {
		em->pc += 4;	
	}
}

void opcodePrefixFive(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "SE Vx, Vy.");

	if (em->V[parts[1]] == em->V[parts[2]]) {
		em->pc += 4;
	}
}

void opcodePrefixSix(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "LD Vx, byte.");

	em->V[parts[1]] = parts[5];
	em->pc += 2;
}

void opcodePrefixSeven(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "ADD Vx, byte.");

	em->V[parts[1]] = em->V[parts[1]] + parts[5];
	em->pc += 2;
}

void opcodePrefixEight(Emulator *em, const uint16_t parts[6]) {

	switch (parts[3]) {
		case 0x0000:
			opcodePrint(em, "LD Vx, Vy.");
			em->V[parts[1]] = em->V[parts[2]];
			em->pc += 2;
		break;
		case 0x0001:
			opcodePrint(em, "OR Vx, Vy.");
			em->V[parts[1]] = em->V[parts[1]] | em->V[parts[2]];
			em->pc += 2;
		break;
		case 0x0002:
			opcodePrint(em, "AND Vx, Vy.");
			em->V[parts[1]] = em->V[parts[1]] & em->V[parts[2]];
			em->pc += 2;
		break;
		case 0x0003:
			opcodePrint(em, "XOR Vx, Vy.");
			em->V[parts[1]] = em->V[parts[1]] ^ em->V[parts[2]];
			em->pc += 2;
		break;
		case 0x0004:
			opcodePrint(em, "ADD Vx, Vy.");
		break;
		case 0x0005:
			opcodePrint(em, "SUB Vx, Vy.");
		break;
		case 0x0006:
			opcodePrint(em, "SHR Vx {, Vy}.");
		break;
		case 0x0007:
			opcodePrint(em, "SUBN Vx, Vy.");
		break;
		case 0x000E:
			opcodePrint(em, "SHL Vx {, Vy}.");
		break;
	}
}

void opcodePrefixNine(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "SNE Vx, Vy.");

	if (em->V[parts[1]] != em->V[parts[2]]) {
		em->pc += 4;
	}
}

void opcodePrefixA(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "LD I, addr.");
	em->I = parts[4];
	em->pc += 2;
}

void opcodePrefixB(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "JP V0, addr.");
	em->pc = parts[4] + em->V[0];
}

void opcodePrefixC(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "RND Vx, byte.");
	int val = rand() % (256); // Random number from 0 to 255.
	em->V[parts[1]] = val & parts[5];
	em->pc += 2;
}

void opcodePrefixD(Emulator *em, const uint16_t parts[6]) {
	opcodePrint(em, "DRW Vx, Vy, nibble.");
}

void opcodePrefixE(Emulator *em, const uint16_t parts[6]) {

	switch (parts[5]) {
		case 0x009E:
			opcodePrint(em, "SKP Vx.");
		break;
		case 0x0A1:
			opcodePrint(em, "SKNP Vx.");
		break;
	}
}

void opcodePrefixF(Emulator *em, const uint16_t parts[6]) {

	switch (parts[5]) {
		case 0x0007:
			opcodePrint(em, "LD Vx, DT.");
		break;
		case 0x000A:
			opcodePrint(em, "LD VX, K.");
		break;
		case 0x0015:
			opcodePrint(em, "LD DT, Vx.");
		break;
		case 0x0018:
			opcodePrint(em, "LD ST, Vx.");
		break;
		case 0x001E:
			opcodePrint(em, "ADD I, Vx.");
		break;
		case 0x0029:
			opcodePrint(em, "LD F, Vx.");
		break;
		case 0x0033:
			opcodePrint(em, "LD B, Vx.");
		break;
		case 0x0055:
			opcodePrint(em, "LD [I], Vx.");
		break;
		case 0x0065:
			opcodePrint(em, "LD Vx, [I].");
		break;
	}
}
