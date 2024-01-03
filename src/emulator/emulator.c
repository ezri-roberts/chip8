#include "emulator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void emulatorInit(Emulator *em) {

	em->pc = 0x200;
	em->opcode = 0;
	em->I = 0;
	em->sp = 0;

	// Clear Display
	// Clear Stack
	// Clear registers V0-VF
	// Clear memory.
	
	// Load fontset
	// for (int i = 0; i < 80; ++i) {
	// 	emulator->memory[i] = fontset[i];
	// }
	
	// Reset timers.

}

void emulatorLoad(Emulator *em, const char *name) {

	const int bufferSize = 1000;

	FILE *file;
	char buffer[bufferSize];

	file = fopen(name, "rb");

	if (file == NULL) {
		perror("Error opening file.");
		//return 1;
	}

	size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
	if (bytesRead > 0) {
		printf("Read %zu bytes from the file.\n", bytesRead);
	}

	fclose(file);

	for (int i = 0; i < bufferSize; i++) {
		em->memory[i+512] = buffer[i];
	}

}

void opcodeClearOrReturn(Emulator *em) {

	uint16_t decoded = em->opcode & 0x000F;

	switch (decoded) {
		case 0x0000: // 0x00E0 Clears the screen.
			printf("%X, %X, '%s'\n", decoded, em->opcode, "Clears the screen.");
		break;
		case 0x000E: // 0x00EE Returns from subroutine.
			printf("%X, %X, '%s'\n", decoded, em->opcode, "Returns from a subroutine.");
		break;
	}
}

void showCode(Emulator *em, uint16_t decoded, const char *msg) {
	printf("%X, %X, '%s'\n", decoded, em->opcode, msg);
}

void emulatorCycle(Emulator *em) {

	em->opcode = em->memory[em->pc] << 8 | em->memory[em->pc + 1];

	uint16_t decoded = em->opcode & 0xF000;
	switch (decoded) {

		case 0x0000:
			opcodeClearOrReturn(em);
		break;
		case 0x1000:
			showCode(em, decoded, "Jumps to address NNN.");
		break;
		case 0x2000:
			showCode(em, decoded, "Calls subroutine at NNN.");
		break;
		case 0x3000:
			showCode(em, decoded, "Skips the next instruction if VX equals NN.");
		break;
		case 0x4000:
			showCode(em, decoded, "Skips the next instruction if VX does not equal NN.");
		break;
		case 0x5000:
			showCode(em, decoded, "Skips the next instruction if VX equals VY.");
		break;
		case 0x6000:
			showCode(em, decoded, "Sets VX to NN.");
		break;
		case 0x7000:
			showCode(em, decoded, "Adds NN to VX.");
		break;
		case 0xA000:
			showCode(em, decoded, "Sets I to the address NNN.");
		break;

		default:
			printf("UNKNOWN OPCODE: %X, %X\n", em->opcode, decoded);
	}

	em->pc += 2;

}

