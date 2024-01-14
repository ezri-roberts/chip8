#include <stdint.h>
#include <stdlib.h>
#include "vm.h"

// Execute opcodes starting with 0.
void instruction_zero(Vm *vm) {

	// 00E0 Clears the screen.
	if (vm->inst.kk == 0xE0) {

		vm_framebuffer_clear(vm);

	// 00EE Returns from a subroutine.
	} else if (vm->inst.kk == 0xEE) {

		vm->pc = vm->stack[vm->sp];
		vm->sp --;
	}
}

// Jump to location nnn.
// Interpreter sets the program counter to nnn.
void instruction_one(Vm *vm) {
	vm->pc = vm->inst.nnn;
}

// Call subroutine at nnn.
// Interpreter increments the stack pointer,
// then puts the current PC on the top of the stack.
// The PC is then set to nnn.
void instruction_two(Vm *vm) {
	vm->sp ++;
	vm->stack[vm->sp] = vm->pc;
	vm->pc = vm->inst.nnn;
}

// Skip next instruction if Vx is equal to kk.
// Compares register Vx to kk, and if they are equal, increments the program counter by 2.
void instruction_three(Vm *vm) {

	if (vm->V[vm->inst.x] == vm->inst.kk) {
		vm->pc += 2;
	}
}

// Skip next instruction if Vx is not equal to kk.
// Compares register Vx to kk, and if they are not equal, increments the program counter by 2.
void instruction_four(Vm *vm) {

	if (vm->V[vm->inst.x] != vm->inst.kk) {
		vm->pc += 2;
	}
}

// Skip next instruction if Vx is equal to Vy.
// compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
void instruction_five(Vm *vm) {

	if (vm->V[vm->inst.x] == vm->V[vm->inst.y]) {

		vm->pc += 2;
	}
}

// Set Vx to kk.
// Puts the value kk into register Vx.
void instruction_six(Vm *vm) {

	vm->V[vm->inst.x] = vm->inst.kk;
}

// Add kk to Vx.
// Adds the value kk to the value of register Vx, then stores the result in Vx.
void instruction_seven(Vm *vm) {

	vm->V[vm->inst.x] += vm->inst.kk;
}

void instruction_eight(Vm *vm) {

	switch (vm->inst.n) {
		// Stores the value of register Vy in register Vx.
		case 0x0000:
			vm->V[vm->inst.x] = vm->V[vm->inst.y];
		break;
		// Performs a bitwise OR on the values of Vx and Vy.
		// Result is stored in Vx.
		case 0x0001:
			vm->V[vm->inst.x] |= vm->V[vm->inst.y];
		break;
		// Performs a bitwise AND on the values of Vx and Vy.
		// Result is stored in Vx.
		case 0x0002:
			vm->V[vm->inst.x] &= vm->V[vm->inst.y];
		break;
		// Performs a bitwise exclusive OR on the values of Vx and Vy.
		// Result is stored in Vx.
		case 0x0003:
			vm->V[vm->inst.x] ^= vm->V[vm->inst.y];
		break;
		// The values of Vx and Vy are added together.
		// If the result is greater than 8 bits VF is set to 1, otherwise 0.
		case 0x0004:
			vm->V[vm->inst.x] += vm->V[vm->inst.y];
			vm->V[0xF] = vm->V[vm->inst.x] > 255;
		break;
		// If Vx > Vy, then VF is set to 1, otherwise 0.
		// Then Vy is subtracted from Vx, and the results stored in Vx.
		case 0x0005:
			vm->V[0xF] = vm->V[vm->inst.x] > vm->V[vm->inst.y];
			vm->V[vm->inst.x] -= vm->V[vm->inst.y];
		break;
		// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
		// Then Vx is divided by 2.
		case 0x0006:
			vm->V[0xF] = vm->V[vm->inst.x] & 1;
			vm->V[vm->inst.x] >>= 1;
		break;
		// If Vy > Vx, then VF is set to 1, otherwise 0.
		// Then Vx is subtracted from Vy, and the results stored in Vx.
		case 0x0007:
			vm->V[0xF] = vm->V[vm->inst.y] > vm->V[vm->inst.x];
			vm->V[vm->inst.x] = vm->V[vm->inst.y] - vm->V[vm->inst.x];
		break;
		// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
		// Then Vx is multiplied by 2.
		case 0x000E:
			vm->V[0xF] = (vm->V[vm->inst.x] & 0x80) >> 7;
			vm->V[vm->inst.x] <<= 1;
		break;
	}
}

// Skip next instruction if Vx != Vy.
// The values of Vx and Vy are compared, and if they are not equal,
// the program counter is increased by 2.
void instruction_nine(Vm *vm) {

	if (vm->V[vm->inst.x] != vm->V[vm->inst.y]) {
		vm->pc += 2;
	}
}

// The value of register I is set to nnn.
void instruction_a(Vm *vm) {
	vm->I = vm->inst.nnn;
}

// Jump to location nnn + V0.
// The program counter is set to nnn plus the value of V0.
void instruction_b(Vm *vm) {
	vm->pc = vm->inst.nnn + vm->V[0];
}

// Generates a random number from 0 to 255, which is then ANDed with the value kk.
// The results are stored in Vx.
void instruction_c(Vm *vm) {
	vm->V[vm->inst.x] = (rand() % 256) & vm->inst.kk;
}

// Reads n bytes from memory, starting at the address stored in I.
// These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
// Sprites are XORed onto the existing screen.
// If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
// If the sprite is positioned so part of it is outside the coordinates of the display,
// it wraps around to the opposite side of the screen.
void instruction_d(Vm *vm) {

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

void instruction_e(Vm *vm) {

	switch (vm->inst.kk) {
		// Skip next instruction if key with the value of Vx is pressed.
		// Checks the keyboard, and if the key corresponding to the value of Vx is pressed down,
		// PC is increased by 2.
		case 0x009E:

			if (vm->keypad[vm->V[vm->inst.x]]) {
				vm->pc += 2;
			}
		break;
		// Skip next instruction if key with the value of Vx is not pressed.
		// Checks the keyboard, and if the key corresponding to the value of Vx is not pressed down,
		// PC is increased by 2.
		case 0x0A1:

			if (!vm->keypad[vm->V[vm->inst.x]]) {
				vm->pc += 2;
			}
		break;
	}
}

void instruction_f(Vm *vm) {

	switch (vm->inst.kk) {
		// The value of delay_timer is placed into Vx.
		case 0x0007:
			vm->V[vm->inst.x] = vm->delay_timer;
		break;
		// All execution stops until a key is pressed,
		// then the value of that key is stored in Vx.
		case 0x000A: {

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
		}
		// delay_timer is set to the value of Vx.
		case 0x0015:
			vm->delay_timer = vm->V[vm->inst.x];
		break;
		// sound_timer is set to the value of Vx.
		case 0x0018:
			vm->sound_timer = vm->V[vm->inst.x];
		break;
		// The values of I and Vx are added, and the results are stored in I.
		case 0x001E:
			vm->I += vm->V[vm->inst.x];
		break;
		// The value of I is set to the location for the hexadecimal sprite
		// corresponding to the value of Vx.
		case 0x0029:
			vm->I = vm->V[vm->inst.x] * 5;
		break;
		// Store BCD representation of Vx in memory locations I, I+1, and I+2.
		// Takes the decimal value of Vx,
		// and places the hundreds digit in memory at location in I,
		// the tens digit at location I+1,
		// and the ones digit at location I+2.
		case 0x0033: {

			uint8_t bcd = vm->V[vm->inst.x];

			vm->memory[vm->I+2] = bcd % 10;
			bcd /= 10;
			vm->memory[vm->I+1] = bcd % 10;
			bcd /= 10;
			vm->memory[vm->I] = bcd;

			break;
		}
		// Store registers V0 through Vx in memory starting at location I.
		case 0x0055:

			for (uint8_t i = 0; i <= vm->inst.x; i++) {
				vm->memory[vm->I++] = vm->V[i];
			}
		break;
		// Read registers V0 through Vx from memory starting at location I.
		case 0x0065:

			for (uint8_t i = 0; i <= vm->inst.x; i++) {
				vm->V[i] = vm->memory[vm->I++];
			}
		break;
	}
}
