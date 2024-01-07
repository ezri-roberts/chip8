// #include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"

void opcodePrint(Instruction *instr, const char *msg) {
	
	printf("%X | [%X, %X, %X, %X] | nnn: %X, kk: %X | '%s'\n",
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

void opcodeZero(Vm *vm) {

	switch (vm->inst.kk) {
		case 0xE0:
			opcodePrint(&vm->inst, "CLS.");
			frameBufferClear(vm);
		break;
		case 0xEE:
			opcodePrint(&vm->inst, "RET.");
			vm->pc = vm->stack[vm->sp];
			vm->sp --;
		break;
	}
}

void opcodeOne(Vm *vm) {
	opcodePrint(&vm->inst, "JP addr.");
	vm->pc = vm->inst.nnn;
}

void opcodeTwo(Vm *vm) {
	opcodePrint(&vm->inst, "CALL addr.");
	vm->sp ++;
	vm->stack[vm->sp] = vm->pc;
	vm->pc = vm->inst.nnn;
}

void opcodeThree(Vm *vm) {
	opcodePrint(&vm->inst, "SE Vx, byte.");

	if (vm->V[vm->inst.second] == vm->inst.kk) {
		vm->pc += 2;
	}
}

void opcodeFour(Vm *vm) {
	opcodePrint(&vm->inst, "SNE Vx, byte.");

	if (vm->V[vm->inst.second] != vm->inst.kk) {
		vm->pc += 2;
	}
}

void opcodeFive(Vm *vm) {
	opcodePrint(&vm->inst, "SE Vx, Vy.");

	if (vm->V[vm->inst.second] == vm->V[vm->inst.third]) {

		vm->pc += 2;
	}
}

void opcodeSix(Vm *vm) {
	opcodePrint(&vm->inst, "LD Vx, byte.");

	vm->V[vm->inst.second] = vm->inst.kk;
}

void opcodeSeven(Vm *vm) {
	opcodePrint(&vm->inst, "ADD Vx, byte.");

	vm->V[vm->inst.second] = vm->V[vm->inst.second] + vm->inst.kk;
}

void opcodeEight(Vm *vm) {

	switch (vm->inst.fourth) {
		case 0x0000:
			opcodePrint(&vm->inst, "LD Vx, Vy.");
			vm->V[vm->inst.second] = vm->V[vm->inst.third];
		break;
		case 0x0001:
			opcodePrint(&vm->inst, "OR Vx, Vy.");
			vm->V[vm->inst.second] = vm->V[vm->inst.second] | vm->V[vm->inst.third];
		break;
		case 0x0002:
			opcodePrint(&vm->inst, "AND Vx, Vy.");
			vm->V[vm->inst.second] = vm->V[vm->inst.second] & vm->V[vm->inst.third];
		break;
		case 0x0003:
			opcodePrint(&vm->inst, "XOR Vx, Vy.");
			vm->V[vm->inst.second] = vm->V[vm->inst.second] ^ vm->V[vm->inst.third];
		break;
		case 0x0004:
			opcodePrint(&vm->inst, "ADD Vx, Vy.");

			uint16_t val = vm->V[vm->inst.second] + vm->V[vm->inst.third];
			vm->V[vm->inst.second] += vm->V[vm->inst.third];
			vm->V[0xF] = val > 255;
		break;
		case 0x0005:
			opcodePrint(&vm->inst, "SUB Vx, Vy.");

			vm->V[0xF] = vm->V[vm->inst.second] > vm->V[vm->inst.third];
			vm->V[vm->inst.second] -= vm->V[vm->inst.third];
		break;
		case 0x0006:
			opcodePrint(&vm->inst, "SHR Vx {, Vy}.");

			vm->V[0xF] = vm->V[vm->inst.second] & 1;
			vm->V[vm->inst.second] >>= 1;
		break;
		case 0x0007:
			opcodePrint(&vm->inst, "SUBN Vx, Vy.");

			vm->V[0xF] = vm->V[vm->inst.third] > vm->V[vm->inst.second];
			vm->V[vm->inst.second] = vm->V[vm->inst.third] - vm->V[vm->inst.second];
		break;
		case 0x000E:
			opcodePrint(&vm->inst, "SHL Vx {, Vy}.");

			vm->V[0xF] = (vm->V[vm->inst.second] & 0x80) >> 7;
			vm->V[vm->inst.second] <<= 1;
		break;
	}
}

void opcodeNine(Vm *vm) {
	opcodePrint(&vm->inst, "SNE Vx, Vy.");

	if (vm->V[vm->inst.second] != vm->V[vm->inst.third]) {
		vm->pc += 2;
	}
}

void opcodeA(Vm *vm) {
	opcodePrint(&vm->inst, "LD I, addr.");
	vm->I = vm->inst.nnn;
}

void opcodeB(Vm *vm) {
	opcodePrint(&vm->inst, "JP V0, addr.");
	vm->pc = vm->inst.nnn + vm->V[0];
}

void opcodeC(Vm *vm) {
	opcodePrint(&vm->inst, "RND Vx, byte.");
	vm->V[vm->inst.second] = (rand() % 256) & vm->inst.kk;
}

void opcodeD(Vm *vm) {
	opcodePrint(&vm->inst, "DRW Vx, Vy, nibble.");

	uint16_t xCoord = vm->V[vm->inst.second];
	uint16_t yCoord = vm->V[vm->inst.third];
	uint16_t height = vm->inst.fourth;

	uint16_t pixel;

	vm->V[0xF] = 0;

	for (int yLine = 0; yLine < height; yLine++) {
	
		pixel = vm->memory[vm->I + yLine];

		for (int xLine = 0; xLine < 8; xLine++) {
		
			if ((pixel & (0x80 >> xLine)) != 0) {

				if (vm->frameBuffer[(xCoord+xLine + ((yCoord+yLine) * 64))] == 1) {

					vm->V[0xF] = 1;
				}
				vm->frameBuffer[xCoord+xLine + ((yCoord+yLine) * 64)] ^= 1;
			}
		}
	}
}

void opcodeE(Vm *vm) {

	switch (vm->inst.kk) {
		case 0x009E:
			opcodePrint(&vm->inst, "SKP Vx.");
			if (vm->keypad[vm->V[vm->inst.second]]) {

				vm->pc += 2;
			}
		break;
		case 0x0A1:
			opcodePrint(&vm->inst, "SKNP Vx.");
			if (!vm->keypad[vm->V[vm->inst.second]]) {

				vm->pc += 2;
			}
		break;
	}
}

void opcodeF(Vm *vm) {

	switch (vm->inst.kk) {
		case 0x0007:
			opcodePrint(&vm->inst, "LD Vx, DT.");
			vm->V[vm->inst.second] = vm->delayTimer;
		break;
		case 0x000A:
			opcodePrint(&vm->inst, "LD VX, K.");

		break;
		case 0x0015:
			opcodePrint(&vm->inst, "LD DT, Vx.");
			vm->delayTimer = vm->V[vm->inst.second];
		break;
		case 0x0018:
			// opcodePrint(&em->instr, "LD ST, Vx.");
		break;
		case 0x001E:
			opcodePrint(&vm->inst, "ADD I, Vx.");
			vm->I += vm->V[vm->inst.second];
		break;
		case 0x0029:
			// opcodePrint(&em->instr, "LD F, Vx.");
		break;
		case 0x0033:
			// opcodePrint(&em->instr, "LD B, Vx.");
		break;
		case 0x0055:
			// opcodePrint(&em->instr, "LD [I], Vx.");
		break;
		case 0x0065:
			// opcodePrint(&em->instr, "LD Vx, [I].");
		break;
	}
}
