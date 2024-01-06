#include "emulator/emulator.h"

int main(int argc, char *argv[]) {

	Emulator emulator;

	emulatorInit(&emulator, false);
	emulatorLoad(&emulator, "IBM.ch8");
	emulatorUpate(&emulator);

	return 0;
}
