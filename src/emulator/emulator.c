#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulator.h"

void frameBufferClear(Emulator *em) {
	memset(em->frameBuffer, 0, sizeof(em->frameBuffer));
}

void frameBufferPut(Emulator *em, uint8_t x, uint8_t y) {
	em->frameBuffer[y * 64 + x] = 1;
}

void emulatorInit(Emulator *em, bool stepMode) {

	rendererInit(&em->renderer);

	em->pc = 0x200;
	em->I = 0;
	em->sp = 0;

	em->stepMode = stepMode;

	frameBufferClear(em);

	// Clear Stack
	memset(em->stack, 0, sizeof(em->stack));
	// Clear registers V0-VF
	memset(em->V, 0, sizeof(em->V));
	// Clear memory.
	memset(em->memory, 0, sizeof(em->memory));

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
		em->memory[i] = fontset[i];
	}
	
	// Reset timers.
	em->delayTimer = 0;
	em->soundTimer = 0;
}

void emulatorLoad(Emulator *em, const char *name) {

	FILE *rom;
	rom = fopen(name, "rb");

	if (rom == NULL) {
		printf("Error reading file '%s'", name);
		exit(1);
	}

	// Get and check rom size.
	fseek(rom, 0, SEEK_END);
	const size_t romSize = ftell(rom);
	const size_t maxSize = sizeof(em->memory) - 0x200; 
	rewind(rom);

	if (romSize > maxSize) {
		printf("ROM '%s' is too big.\n", name);
		exit(1);
	}

	// Load rom into memory.
	fread(&em->memory[0x200], romSize, 1, rom);

	// size_t bytesRead = fread(buffer, 1, sizeof(buffer), rom);
	printf("Read %zu bytes from file '%s'\n", romSize, name);

	fclose(rom);
}

void(*lookup[16])(Emulator*) = {
	opcodePrefixZero,
	opcodePrefixOne,
	opcodePrefixTwo,
	opcodePrefixThree,
	opcodePrefixFour,
	opcodePrefixFive,
	opcodePrefixSix,
	opcodePrefixSeven,
	opcodePrefixEight,
	opcodePrefixNine,
	opcodePrefixA,
	opcodePrefixB,
	opcodePrefixC,
	opcodePrefixD,
	opcodePrefixE,
	opcodePrefixF,
};

void emulatorCycle(Emulator *em) {

	em->instr.opcode = code(em->memory[em->pc], em->memory[em->pc+1]);

	em->instr.first = first(em->instr.opcode);
	em->instr.second = second(em->instr.opcode);
	em->instr.third = third(em->instr.opcode);
	em->instr.fourth = fourth(em->instr.opcode);
	em->instr.nnn = nnn(em->instr.opcode);
	em->instr.kk = kk(em->instr.opcode);

	em->pc += 2; // Pre-increment the counter for the next opcode.

	// Execute the instruction.
	lookup[em->instr.first >> 12](em);

	// Timers.
	if (em->delayTimer > 0) {
		em->delayTimer--;
	}

	if (em->soundTimer > 0) {
		em->soundTimer--;
	}
}

bool shouldCycle(Emulator *em) {

	// const size_t maxSize = sizeof(em->memory) - 0x200; 
	// if(em->pc >= maxSize) {
	// 	return false;
	// }

	if (!em->stepMode) {
		return true;
	}

	if (IsKeyPressed(KEY_ENTER)) {
		return true;
	}

	return false;
}

void emulatorUpate(Emulator *em) {

	while (!WindowShouldClose()) {

		// Handle input.

		if (shouldCycle(em)) {
			emulatorCycle(em);
		}

		rendererUpdate(&em->renderer, em->frameBuffer);
	}

	CloseWindow();
}

