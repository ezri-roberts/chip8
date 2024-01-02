#include "emulator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void emulatorInit(Emulator *em) {

	em->pc = 0x200;
	em->opcode = 0;
	em->I = 0;
	em->stackPtr = 0;

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

	for (int i = 0; i < bufferSize; ++i) {
		em->memory[i+512] = buffer[i];
		printf("%X\n", buffer[i]);
	}

}

