#include "renderer.h"
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>

void rendererInit(Renderer *renderer) {

	renderer->width = 640;
	renderer->height = 360;
	renderer->gameWidth = 64;
	renderer->gameHeight = 32;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(renderer->width, renderer->height, "CHIP8 VM");
	SetWindowMinSize(64*4, 32*4);

	renderer->target = LoadRenderTexture(renderer->gameWidth, renderer->gameHeight);
	SetTextureFilter(renderer->target.texture, TEXTURE_FILTER_POINT);

	renderer->source.x = 0;
	renderer->source.y = 0;
	renderer->source.width = (float)renderer->target.texture.width;
	renderer->source.height = (float)-renderer->target.texture.height;

	renderer->dest.x = 0;
	renderer->dest.y = 0;
	renderer->dest.width = (float)renderer->target.texture.width;
	renderer->dest.height = (float)-renderer->target.texture.height;

	SetTargetFPS(500);
}

void drawFrameBuffer(bool buffer[]) {

	for (uint32_t i = 0; i < 2048; i++) {

		uint16_t x = i % 64;
		uint16_t y = i / 64;

		if (buffer[i]) {
			DrawRectangle(x, y, 1, 1, SKYBLUE);
		}
	}
}

void rendererUpdate(Renderer *renderer, bool buffer[]) {

	// Calculate framebuffer scaling.
	float scale = MIN(
		(float)GetScreenWidth()/renderer->gameWidth,
		(float)GetScreenHeight()/renderer->gameHeight
	);

	BeginTextureMode(renderer->target);

		ClearBackground(DARKBLUE);
		drawFrameBuffer(buffer);
	EndTextureMode();

	renderer->source.x = 0.0f;
	renderer->source.y = 0.0f;
	renderer->source.width = (float)renderer->target.texture.width;
	renderer->source.height = (float)-renderer->target.texture.height;

	renderer->dest.x = (GetScreenWidth() - ((float)renderer->gameWidth*scale)) * 0.5f;
	renderer->dest.y = (GetScreenHeight() - ((float)renderer->gameHeight*scale)) * 0.5f;
	renderer->dest.width = (float)renderer->gameWidth*scale;
	renderer->dest.height = (float)renderer->gameHeight*scale;

	BeginDrawing();

		ClearBackground(BLACK);

		DrawTexturePro(
			renderer->target.texture, renderer->source, renderer->dest,
			(Vector2){0,0}, 0.0f, RAYWHITE
		);
	EndDrawing();
}
