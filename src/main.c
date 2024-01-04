#include "emulator/emulator.h"

int main(int argc, char *argv[]) {

	Emulator emulator;

	emulatorInit(&emulator, false);
	emulatorLoad(&emulator, "TETRIS.ch8");

	emulatorUpate(&emulator);

	return 0;
}
