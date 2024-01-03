#include "renderer.h"
#include <raylib.h>

void rendererInit(Renderer *renderer) {

	renderer->width = 800;
	renderer->height = 400;
	renderer->gameWidth = 64;
	renderer->gameHeight = 32;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(renderer->width, renderer->height, "CHIP8 Emulator");
	SetWindowMinSize(renderer->gameWidth, renderer->gameHeight);

	renderer->target = LoadRenderTexture(renderer->gameWidth, renderer->gameHeight);
	SetTextureFilter(renderer->target.texture, TEXTURE_FILTER_POINT);

	SetTargetFPS(60);
}

void rendererUpdate(Renderer *renderer, Emulator *emulator) {

	while (!WindowShouldClose()) {

		// Calculate framebuffer scaling.
		float scale = MIN(
			(float)GetScreenWidth()/renderer->gameWidth,
			(float)GetScreenHeight()/renderer->gameHeight
		);

		BeginTextureMode(renderer->target);
			ClearBackground(DARKGRAY);
			emulatorDraw(emulator);

		EndTextureMode();

		BeginDrawing();
			ClearBackground(BLACK);

			Rectangle source = (Rectangle){
				0.0f, 0.0f,
				(float)renderer->target.texture.width,
				(float)-renderer->target.texture.height
			};

			Rectangle dest = (Rectangle){
				(GetScreenWidth() - ((float)renderer->gameWidth*scale)) * 0.5f,	
				(GetScreenHeight() - ((float)renderer->gameHeight*scale)) * 0.5f,	
				(float)renderer->gameWidth*scale, (float)renderer->gameHeight*scale
			};

			DrawTexturePro(renderer->target.texture, source, dest, (Vector2){0,0}, 0.0f, WHITE);
		EndDrawing();
	}

	CloseWindow();
}
