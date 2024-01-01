#include "raylib.h"

int main(int argc, char *argv[]) {

	const int width = 800;
	const int height = 450;

	InitWindow(width, height, "Window");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();	
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
