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
	// static uint8_t font[80] = {
	// 	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	// 	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	// 	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	// 	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	// 	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	// 	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	// 	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	// 	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	// 	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	// 	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	// 	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	// 	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	// 	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	// 	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	// 	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	// 	0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
	// };
	
	// for (int i = 0; i < 80; ++i) {
	// 	emulator->memory[i] = fontset[i];
	// }
	
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

void emulatorCycle(Emulator *em) {

	em->opcode = em->memory[em->pc] << 8 | em->memory[em->pc + 1];
	
	// Extract the different components of the opcode.
	uint16_t first = (em->opcode & 0xF000); // e.g. '5'xy0
	// uint16_t x     = (em->opcode & 0x0F00); // e.g. 5'x'y0
	// uint16_t y     = (em->opcode & 0x00F0); // e.g. 5x'y'0
	uint16_t last  = (em->opcode & 0x000F); // e.g. 5xy'0'
	// uint16_t nnn   = (em->opcode & 0x0FFF); // e.g. 1'nnn' 
	uint16_t kk    = (em->opcode & 0x00FF); // e.g. 6x'kk'

	switch (first) {

		case 0x0000:
			opcodePrefixZero(em, last);
		break;
		case 0x1000:
			opcodePrint(em, "JP addr.");
		break;
		case 0x2000:
			opcodePrint(em, "CALL addr.");
		break;
		case 0x3000:
			opcodePrint(em, "SE Vx, byte.");
		break;
		case 0x4000:
			opcodePrint(em, "SNE Vx, byte.");
		break;
		case 0x5000:
			opcodePrint(em, "SE Vx, Vy.");
		break;
		case 0x6000:
			opcodePrint(em, "LD Vx, byte.");
		break;
		case 0x7000:
			opcodePrint(em, "ADD Vx, byte.");
		break;
		case 0x8000:
			opcodePrefixEight(em, last);
		break;
		case 0x9000:
			opcodePrint(em, "SNE Vx, Vy.");
		break;
		case 0xA000:
			opcodePrint(em, "LD I, addr.");
		break;
		case 0xB000:
			opcodePrint(em, "JP V0, addr.");
		break;
		case 0xC000:
			opcodePrint(em, "RND Vx, byte.");
		break;
		case 0xD000:
			opcodePrint(em, "DRW Vx, Vy, nibble.");
		break;
		case 0xE000:
			opcodePrefixE(em, kk);
		break;
		case 0xF000:
			opcodePrefixF(em, kk);
		break;

		default:
			opcodePrint(em, "Unknown opcode.");
	}

	em->pc += 2;

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

