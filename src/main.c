#include "vm.h"

int main(int argc, char *argv[]) {

	Vm vm;

	vm_init(&vm);
	// Get file path from first arg to load a ROM.
	vm_load_rom(&vm, argv[1]);
	vm_update(&vm);

	return 0;
}
