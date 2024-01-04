#include <stdio.h>
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

void opcodePrefixZero(Emulator *em, const uint16_t last) {

	switch (last) {
		case 0x0000:
			opcodePrint(em, "CLS.");
		break;
		case 0x000E:
			opcodePrint(em, "RET.");
		break;
	}
}

void opcodePrefixEight(Emulator *em, const uint16_t last) {

	switch (last) {
		case 0x0000:
			opcodePrint(em, "LD Vx, Vy.");
		break;
		case 0x0001:
			opcodePrint(em, "OR Vx, Vy.");
		break;
		case 0x0002:
			opcodePrint(em, "AND Vx, Vy.");
		break;
		case 0x0003:
			opcodePrint(em, "XOR Vx, Vy.");
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

void opcodePrefixE(Emulator *em, const uint16_t kk) {

	switch (kk) {
		case 0x009E:
			opcodePrint(em, "SKP Vx.");
		break;
		case 0x0A1:
			opcodePrint(em, "SKNP Vx.");
		break;
	}
}

void opcodePrefixF(Emulator *em, const uint16_t kk) {

	switch (kk) {
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
