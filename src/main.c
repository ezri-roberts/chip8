#include "emulator/emulator.h"

int main(int argc, char *argv[]) {

	Vm vm;

	emulatorInit(&vm, false);
	emulatorLoad(&vm, argv[1]);
	emulatorUpate(&vm);

	return 0;
}
