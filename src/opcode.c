// #include <stdint.h>
#include <stdint.h>
// #include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

void instruction_print(Instruction *instr, const char *msg) {
	
	// printf("%X | [%X, %X, %X, %X] | nnn: %X, kk: %X | '%s'\n",
	// 	instr->opcode,
	// 	instr->first, instr->x, instr->y, instr->n,
	// 	instr->nnn, instr->kk, msg
	// );
}

// Execute opcodes starting with 0.
// 00E0 Clears the screen.
// 00EE Returns from a subroutine.
void opcodeZero(Vm *vm) {

	if (vm->inst.kk == 0xE0) {

		instruction_print(&vm->inst, "CLS.");
		vm_framebuffer_clear(vm);
	} else if (vm->inst.kk == 0xEE) {

		instruction_print(&vm->inst, "RET.");
		vm->pc = vm->stack[vm->sp];
		vm->sp --;
	}
}

// Jump to location nnn.
void opcodeOne(Vm *vm) {
	instruction_print(&vm->inst, "JP addr.");
	vm->pc = vm->inst.nnn;
}

// Call subroutine at nnn.
void opcodeTwo(Vm *vm) {
	instruction_print(&vm->inst, "CALL addr.");
	vm->sp ++;
	vm->stack[vm->sp] = vm->pc;
	vm->pc = vm->inst.nnn;
}

// Skip next instruction if Vx is equal to kk.
void opcodeThree(Vm *vm) {
	instruction_print(&vm->inst, "SE Vx, byte.");

	if (vm->V[vm->inst.x] == vm->inst.kk) {
		vm->pc += 2;
	}
}

// Skip next instruction if Vx is not equal to kk.
void opcodeFour(Vm *vm) {
	instruction_print(&vm->inst, "SNE Vx, byte.");

	if (vm->V[vm->inst.x] != vm->inst.kk) {
		vm->pc += 2;
	}
}

// Skip next instruction if Vx is equal to Vy.
void opcodeFive(Vm *vm) {
	instruction_print(&vm->inst, "SE Vx, Vy.");

	if (vm->V[vm->inst.x] == vm->V[vm->inst.y]) {

		vm->pc += 2;
	}
}

// Set Vx to kk.
void opcodeSix(Vm *vm) {
	instruction_print(&vm->inst, "LD Vx, byte.");

	vm->V[vm->inst.x] = vm->inst.kk;
}

// Add kk to Vx.
void opcodeSeven(Vm *vm) {
	instruction_print(&vm->inst, "ADD Vx, byte.");

	vm->V[vm->inst.x] += vm->inst.kk;
}

void opcodeEight(Vm *vm) {

	switch (vm->inst.n) {
		// Store value of Vy in Vx.
		case 0x0000:
			instruction_print(&vm->inst, "LD Vx, Vy.");
			vm->V[vm->inst.x] = vm->V[vm->inst.y];
		break;
		case 0x0001:
			instruction_print(&vm->inst, "OR Vx, Vy.");
			vm->V[vm->inst.x] |= vm->V[vm->inst.y];
		break;
		case 0x0002:
			instruction_print(&vm->inst, "AND Vx, Vy.");
			vm->V[vm->inst.x] &= vm->V[vm->inst.y];
		break;
		case 0x0003:
			instruction_print(&vm->inst, "XOR Vx, Vy.");
			vm->V[vm->inst.x] ^= vm->V[vm->inst.y];
		break;
		case 0x0004:
			instruction_print(&vm->inst, "ADD Vx, Vy.");

			// uint16_t val = vm->V[vm->inst.x] + vm->V[vm->inst.y];
			vm->V[vm->inst.x] += vm->V[vm->inst.y];
			vm->V[0xF] = vm->V[vm->inst.x] > 255;
		break;
		case 0x0005:
			instruction_print(&vm->inst, "SUB Vx, Vy.");

			vm->V[0xF] = vm->V[vm->inst.x] > vm->V[vm->inst.y];
			vm->V[vm->inst.x] -= vm->V[vm->inst.y];
		break;
		case 0x0006:
			instruction_print(&vm->inst, "SHR Vx {, Vy}.");

			vm->V[0xF] = vm->V[vm->inst.x] & 1;
			vm->V[vm->inst.x] >>= 1;
		break;
		case 0x0007:
			instruction_print(&vm->inst, "SUBN Vx, Vy.");

			vm->V[0xF] = vm->V[vm->inst.y] > vm->V[vm->inst.x];
			vm->V[vm->inst.x] = vm->V[vm->inst.y] - vm->V[vm->inst.x];
		break;
		case 0x000E:
			instruction_print(&vm->inst, "SHL Vx {, Vy}.");

			vm->V[0xF] = (vm->V[vm->inst.x] & 0x80) >> 7;
			vm->V[vm->inst.x] <<= 1;
		break;
	}
}

void opcodeNine(Vm *vm) {
	instruction_print(&vm->inst, "SNE Vx, Vy.");

	if (vm->V[vm->inst.x] != vm->V[vm->inst.y]) {
		vm->pc += 2;
	}
}

void opcodeA(Vm *vm) {
	instruction_print(&vm->inst, "LD I, addr.");
	vm->I = vm->inst.nnn;
}

void opcodeB(Vm *vm) {
	instruction_print(&vm->inst, "JP V0, addr.");
	vm->pc = vm->inst.nnn + vm->V[0];
}

void opcodeC(Vm *vm) {
	instruction_print(&vm->inst, "RND Vx, byte.");
	vm->V[vm->inst.x] = (rand() % 256) & vm->inst.kk;
}

void opcodeD(Vm *vm) {

	instruction_print(&vm->inst, "DRW Vx, Vy, nibble.");

	const uint8_t x_coord = vm->V[vm->inst.x];
	const uint8_t y_coord = vm->V[vm->inst.y];
	const uint8_t height = vm->inst.n;

	uint16_t spr_data;
	bool *pixel;

	vm->V[0xF] = 0;

	for (int y_line = 0; y_line < height; y_line++) {
	
		spr_data = vm->memory[vm->I + y_line];

		for (int x_line = 0; x_line < 8; x_line++) {
		
			if ((spr_data & (0x80 >> x_line)) == 0) continue;

			const uint16_t index = (x_coord+x_line) + (y_coord+y_line) * 64;
			pixel = &vm->framebuffer[index];

			if (*pixel == 1) vm->V[0xF] = 1;
			*pixel ^= 1;
		}
	}
}

void opcodeE(Vm *vm) {

	switch (vm->inst.kk) {
		case 0x009E:
			instruction_print(&vm->inst, "SKP Vx.");

			if (vm->keypad[vm->V[vm->inst.x]]) {
				vm->pc += 2;
			}
		break;
		case 0x0A1:
			instruction_print(&vm->inst, "SKNP Vx.");

			if (!vm->keypad[vm->V[vm->inst.x]]) {
				vm->pc += 2;
			}
		break;
	}
}

void opcodeF(Vm *vm) {

	switch (vm->inst.kk) {
		case 0x0007:
			instruction_print(&vm->inst, "LD Vx, DT.");

			vm->V[vm->inst.x] = vm->delay_timer;
		break;
		case 0x000A:
			instruction_print(&vm->inst, "LD VX, K.");

			bool key_pressed = false;
			uint8_t key;

			for (uint8_t i = 0; i < sizeof(vm->keypad); i++) {
			
				if (vm->keypad[i]) {
					key_pressed = true;
					key = i;
				}
			}

			if (key_pressed) {
				vm->V[vm->inst.x] = key;
			} else {
				vm->pc -= 2;
			}
		break;
		case 0x0015:
			instruction_print(&vm->inst, "LD DT, Vx.");

			vm->delay_timer = vm->V[vm->inst.x];
		break;
		case 0x0018:
			instruction_print(&vm->inst, "LD ST, Vx.");

			vm->sound_timer = vm->V[vm->inst.x];
		break;
		case 0x001E:
			instruction_print(&vm->inst, "ADD I, Vx.");

			vm->I += vm->V[vm->inst.x];
		break;
		case 0x0029:
			instruction_print(&vm->inst, "LD F, Vx.");

			vm->I = vm->V[vm->inst.x] * 5;
		break;
		case 0x0033:
			instruction_print(&vm->inst, "LD B, Vx.");

			uint8_t bcd = vm->V[vm->inst.x];

			vm->memory[vm->I+2] = bcd % 10;
			bcd /= 10;
			vm->memory[vm->I+1] = bcd % 10;
			bcd /= 10;
			vm->memory[vm->I] = bcd;

		break;
		case 0x0055:
			instruction_print(&vm->inst, "LD [I], Vx.");

			for (uint8_t i = 0; i <= vm->inst.x; i++) {
				vm->memory[vm->I++] = vm->V[i];
			}
		break;
		case 0x0065:
			instruction_print(&vm->inst, "LD Vx, [I].");

			for (uint8_t i = 0; i <= vm->inst.x; i++) {
				vm->V[i] = vm->memory[vm->I++];
			}
		break;
	}
}
