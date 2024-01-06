// #include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include "emulator.h"

void opcodePrint(Instruction *instr, const char *msg) {
	
	printf("%X | %X, %X, %X, %X | nnn: %X, kk: %X | '%s'\n",
		instr->opcode,
		instr->first, instr->second, instr->third, instr->fourth,
		instr->nnn, instr->kk, msg
	);
}

/*
* Increment the program counter by two after every executed opcode.
* This is true unless you jump to a certain address in the memory or if you call a subroutine.
* (in which case you need to store the program counter in the stack).
* If the next opcode should be skipped, increase the program counter by four.
*/

void opcodePrefixZero(Emulator *em) {

	switch (em->instr.kk) {
		case 0x00E0:
			opcodePrint(&em->instr, "CLS.");
			frameBufferClear(em);
		break;
		case 0x000E:
			opcodePrint(&em->instr, "RET.");
			em->pc = em->stack[em->sp];
			em->sp --;
		break;
	}
}

void opcodePrefixOne(Emulator *em) {
	opcodePrint(&em->instr, "JP addr.");
	em->pc = em->instr.nnn;
}

void opcodePrefixTwo(Emulator *em) {
	opcodePrint(&em->instr, "CALL addr.");
	em->sp ++;
	em->stack[em->sp] = em->pc;
	em->pc = em->instr.nnn;
}

void opcodePrefixThree(Emulator *em) {
	opcodePrint(&em->instr, "SE Vx, byte.");

	if (em->V[em->instr.second] == em->instr.kk) {
		em->pc += 2;
	}
}

void opcodePrefixFour(Emulator *em) {
	opcodePrint(&em->instr, "SNE Vx, byte.");

	if (em->V[em->instr.second] != em->instr.kk) {
		em->pc += 2;
	}
}

void opcodePrefixFive(Emulator *em) {
	// opcodePrint(&em->instr, "SE Vx, Vy.");

}

void opcodePrefixSix(Emulator *em) {
	opcodePrint(&em->instr, "LD Vx, byte.");

	em->V[em->instr.second] = em->instr.kk;
}

void opcodePrefixSeven(Emulator *em) {
	opcodePrint(&em->instr, "ADD Vx, byte.");

	em->V[em->instr.second] = em->V[em->instr.second] + em->instr.kk;
}

void opcodePrefixEight(Emulator *em) {

	switch (em->instr.fourth) {
		case 0x0000:
			opcodePrint(&em->instr, "LD Vx, Vy.");
			em->V[em->instr.second] = em->V[em->instr.third];
		break;
		case 0x0001:
			// opcodePrint(&em->instr, "OR Vx, Vy.");
		break;
		case 0x0002:
			// opcodePrint(&em->instr, "AND Vx, Vy.");
		break;
		case 0x0003:
			// opcodePrint(&em->instr, "XOR Vx, Vy.");
		break;
		case 0x0004:
			// opcodePrint(&em->instr, "ADD Vx, Vy.");
		break;
		case 0x0005:
			// opcodePrint(&em->instr, "SUB Vx, Vy.");
		break;
		case 0x0006:
			// opcodePrint(&em->instr, "SHR Vx {, Vy}.");
		break;
		case 0x0007:
			// opcodePrint(&em->instr, "SUBN Vx, Vy.");
		break;
		case 0x000E:
			// opcodePrint(&em->instr, "SHL Vx {, Vy}.");
		break;
	}
}

void opcodePrefixNine(Emulator *em) {
	// opcodePrint(&em->instr, "SNE Vx, Vy.");

}

void opcodePrefixA(Emulator *em) {
	opcodePrint(&em->instr, "LD I, addr.");
	em->I = em->instr.nnn;
}

void opcodePrefixB(Emulator *em) {
	opcodePrint(&em->instr, "JP V0, addr.");
	em->pc = em->instr.nnn + em->V[0];
}

void opcodePrefixC(Emulator *em) {
	// opcodePrint(&em->instr, "RND Vx, byte.");
}

void opcodePrefixD(Emulator *em) {
	opcodePrint(&em->instr, "DRW Vx, Vy, nibble.");

	uint16_t xCoord = em->V[em->instr.second];
	uint16_t yCoord = em->V[em->instr.third];
	uint16_t height = em->instr.fourth;

	printf("x: %d, y: %d, h: %d\n", xCoord, yCoord, height);

	uint16_t pixel;

	em->V[0xF] = 0;

	for (int yLine = 0; yLine < height; yLine++) {
	
		pixel = em->memory[em->I + yLine];
		for (int xLine = 0; xLine < 8; xLine++) {
		
			if ((pixel & (0x80 >> xLine)) != 0) {

				if (em->frameBuffer[(xCoord+xLine + ((yCoord+yLine) * 64))] == 1) {

					em->V[0xF] = 1;
				}
				em->frameBuffer[xCoord+xLine + ((yCoord+yLine) * 64)] ^= 1;
			}
		}
	}


	// for (int i = 0; i < em->instr.fourth; i++) {
	//
	// 	// uint16_t data = em->memory[em->I + i];
	// 	frameBufferPut(em, xCoord, yCoord);
	// 
	// }
}

void opcodePrefixE(Emulator *em) {

	switch (em->instr.kk) {
		case 0x009E:
			// opcodePrint(&em->instr, "SKP Vx.");
		break;
		case 0x0A1:
			// opcodePrint(&em->instr, "SKNP Vx.");
		break;
	}
}

void opcodePrefixF(Emulator *em) {

	switch (em->instr.kk) {
		case 0x0007:
			opcodePrint(&em->instr, "LD Vx, DT.");
		break;
		case 0x000A:
			opcodePrint(&em->instr, "LD VX, K.");
		break;
		case 0x0015:
			opcodePrint(&em->instr, "LD DT, Vx.");
		break;
		case 0x0018:
			opcodePrint(&em->instr, "LD ST, Vx.");
		break;
		case 0x001E:
			opcodePrint(&em->instr, "ADD I, Vx.");
		break;
		case 0x0029:
			opcodePrint(&em->instr, "LD F, Vx.");
		break;
		case 0x0033:
			opcodePrint(&em->instr, "LD B, Vx.");
		break;
		case 0x0055:
			opcodePrint(&em->instr, "LD [I], Vx.");
		break;
		case 0x0065:
			opcodePrint(&em->instr, "LD Vx, [I].");
		break;
	}
}
