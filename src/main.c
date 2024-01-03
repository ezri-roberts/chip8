#include "raylib.h"
#include "emulator/emulator.h"

int main(int argc, char *argv[]) {

	Emulator emulator;

	// Emulator init.
	emulatorInit(&emulator);
	// Load game.
	emulatorLoad(&emulator, "TETRIS.ch8");
	//
	// Emulation loop.
	// Emulation cycle.
	// Draw.
	// Keys.
	// End loop.
	
	while(emulator.pc < 4096) {
		emulatorCycle(&emulator);
	}

	// const int width = 800;
	// const int height = 450;
	//
	// InitWindow(width, height, "Window");
	// SetTargetFPS(60);
	//
	// while (!WindowShouldClose()) {
	// 	BeginDrawing();	
	// 	ClearBackground(BLACK);
	// 	EndDrawing();
	//
	// 	emulatorCycle(&emulator);
	// }
	//
	// CloseWindow();

	return 0;
}
