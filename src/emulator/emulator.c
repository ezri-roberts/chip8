#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulator.h"

void frameBufferClear(Vm *vm) {
	memset(vm->frameBuffer, 0, sizeof(vm->frameBuffer));
}

void frameBufferPut(Vm *vm, uint8_t x, uint8_t y) {
	vm->frameBuffer[y * 64 + x] = 1;
}

void emulatorInit(Vm *vm, bool stepMode) {

	rendererInit(&vm->renderer);

	vm->pc = 0x200;
	vm->I = 0;
	vm->sp = 0;

	vm->stepMode = stepMode;

	frameBufferClear(vm);

	// Clear Stack
	memset(vm->stack, 0, sizeof(vm->stack));
	// Clear registers V0-VF
	memset(vm->V, 0, sizeof(vm->V));
	// Clear memory.
	memset(vm->memory, 0, sizeof(vm->memory));

	const uint8_t fontset[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
		0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
		0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
		0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
		0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
		0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
		0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
		0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
		0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
		0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
		0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
		0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
		0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
		0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
		0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
		0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
	};
	
	// Load fontset into memory.
	for (int i = 0; i < 80; i++) {
		vm->memory[i] = fontset[i];
	}
	
	// Reset timers.
	vm->delayTimer = 0;
	vm->soundTimer = 0;

	vm->state = RUNNING;
}

void emulatorLoad(Vm *vm, const char *name) {

	FILE *rom;
	rom = fopen(name, "rb");

	if (rom == NULL) {
		printf("Error reading file '%s'", name);
		exit(1);
	}

	// Get and check rom size.
	fseek(rom, 0, SEEK_END);
	const size_t romSize = ftell(rom);
	const size_t maxSize = sizeof(vm->memory) - 0x200; 
	rewind(rom);

	if (romSize > maxSize) {
		printf("ROM '%s' is too big.\n", name);
		exit(1);
	}

	// Load rom into memory.
	fread(&vm->memory[0x200], romSize, 1, rom);

	// size_t bytesRead = fread(buffer, 1, sizeof(buffer), rom);
	printf("Read %zu bytes from file '%s'\n", romSize, name);

	fclose(rom);
}

void(*lookup[16])(Vm*) = {
	opcodeZero,
	opcodeOne,
	opcodeTwo,
	opcodeThree,
	opcodeFour,
	opcodeFive,
	opcodeSix,
	opcodeSeven,
	opcodeEight,
	opcodeNine,
	opcodeA,
	opcodeB,
	opcodeC,
	opcodeD,
	opcodeE,
	opcodeF,
};

void emulatorCycle(Vm *vm) {

	vm->inst.opcode = code(vm->memory[vm->pc], vm->memory[vm->pc+1]);

	vm->inst.first = first(vm->inst.opcode);
	vm->inst.second = second(vm->inst.opcode);
	vm->inst.third = third(vm->inst.opcode);
	vm->inst.fourth = fourth(vm->inst.opcode);
	vm->inst.nnn = nnn(vm->inst.opcode);
	vm->inst.kk = kk(vm->inst.opcode);

	printf("ADDR %X | ", vm->pc);
	vm->pc += 2; // Pre-increment the counter for the next opcode.

	// Execute the instruction.
	lookup[vm->inst.first](vm);

	// Timers.
	if (vm->delayTimer > 0) {
		vm->delayTimer--;
	}

	if (vm->soundTimer > 0) {
		vm->soundTimer--;
	}
}

void emulatorUpate(Vm *vm) {

	while (!WindowShouldClose()) {

		if (IsKeyPressed(KEY_SPACE)) {
			vm->state = !vm->state;
		}

		// Handle input.

		if (vm->state == RUNNING) {

			vm->keypad[0] = IsKeyPressed(KEY_X);
			vm->keypad[1] = IsKeyPressed(KEY_ONE);
			vm->keypad[2] = IsKeyPressed(KEY_TWO);
			vm->keypad[3] = IsKeyPressed(KEY_THREE);
			vm->keypad[4] = IsKeyPressed(KEY_Q);
			vm->keypad[5] = IsKeyPressed(KEY_W);
			vm->keypad[6] = IsKeyPressed(KEY_W);
			vm->keypad[7] = IsKeyPressed(KEY_A);
			vm->keypad[8] = IsKeyPressed(KEY_S);
			vm->keypad[9] = IsKeyPressed(KEY_D);
			vm->keypad[10] = IsKeyPressed(KEY_Z);
			vm->keypad[11] = IsKeyPressed(KEY_C);
			vm->keypad[12] = IsKeyPressed(KEY_FOUR);
			vm->keypad[13] = IsKeyPressed(KEY_R);
			vm->keypad[14] = IsKeyPressed(KEY_F);
			vm->keypad[15] = IsKeyPressed(KEY_V);

			emulatorCycle(vm);

		}

		rendererUpdate(&vm->renderer, vm->frameBuffer);
	}

	CloseWindow();
}

