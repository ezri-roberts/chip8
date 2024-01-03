#include "emulator/emulator.h"
#include "renderer/renderer.h"

int main(int argc, char *argv[]) {

	Renderer renderer;
	Emulator emulator;

	rendererInit(&renderer);

	emulatorInit(&emulator);
	emulatorLoad(&emulator, "TETRIS.ch8");
	//
	// Emulation loop.
	// Emulation cycle.
	// Draw.
	// Keys.
	// End loop.
	
	// while(emulator.pc < 4096) {
	// 	emulatorCycle(&emulator);
	// }
	
	rendererUpdate(&renderer, &emulator);

	return 0;
}
