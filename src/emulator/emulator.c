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
	int index = y * 64 + x;
	em->frameBuffer[index] = 1;
}

void emulatorInit(Emulator *em, bool stepMode) {

	rendererInit(&em->renderer);

	em->pc = 0x200;
	em->opcode = 0;
	em->I = 0;
	em->sp = 0;

	frameBufferClear(em);

	// Test grid.
	for (int x=0; x<64; x+=2) {
		for (int y=0; y<32; y+=2) {
			frameBufferPut(em, x, y);
		}
	}

	// Clear Stack
	memset(em->stack, 0, sizeof(em->stack));
	// Clear registers V0-VF
	memset(em->V, 0, sizeof(em->V));
	// Clear memory.
	memset(em->memory, 0, sizeof(em->memory));

	// Load fontset
	static uint8_t fontset[80] = {
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
	
	for (int i = 0; i < 80; i++) {
		em->memory[i] = fontset[i];
	}
	
	// Reset timers.
	em->delayTimer = 0;
	em->soundTimer = 0;

	em->stepMode = stepMode;
}

void emulatorLoad(Emulator *em, const char *name) {

	const int bufferSize = 1000;

	FILE *file;
	unsigned char buffer[bufferSize];

	file = fopen(name, "rb");

	if (file == NULL) {
		printf("Error reading file '%s'", name);
		exit(1);
	}

	size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
	printf("Read %zu bytes from file '%s'\n", bytesRead, name);

	fclose(file);

	// Load into memory.
	for (int i = 0; i < (4096-512); i++) {
		em->memory[i+512] = buffer[i];
	}

}

void(*lookup[16])(Emulator*, const uint16_t[6]) = {
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

	em->opcode = em->memory[em->pc] << 8 | em->memory[em->pc + 1];
	
	const uint16_t parts[6] = {
		(em->opcode & 0xF000), // first e.g. '5'xy0
		(em->opcode & 0x0F00), // x e.g. 5'x'y0
		(em->opcode & 0x00F0), // y e.g. 5x'y'0
		(em->opcode & 0x000F), // last e.g. 5xy'0'
		(em->opcode & 0x0FFF), // nnn e.g. 1'nnn' 
		(em->opcode & 0x00FF), // kk e.g. 6x'kk'
	};

	const uint8_t index = parts[0] >> 12;
	lookup[index](em, parts);

	// Timers.
	if (em->delayTimer > 0) {
		em->delayTimer--;
	}

	if (em->soundTimer > 0) {
		em->soundTimer--;
	}
}

bool shouldCycle(Emulator *em) {

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

		if (shouldCycle(em)) {
			emulatorCycle(em);
		}

		rendererUpdate(&em->renderer, em->frameBuffer);
	}

	CloseWindow();
}

