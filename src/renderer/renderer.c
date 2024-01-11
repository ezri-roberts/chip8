#include "renderer.h"
#include <raylib.h>
#include <stdint.h>

void renderer_init(Renderer *renderer) {

	renderer->width = 640;
	renderer->height = 360;
	renderer->game_width = 64;
	renderer->game_height = 32;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(renderer->width, renderer->height, "CHIP8 VM");
	SetWindowMinSize(64*4, 32*4);

	renderer->target = LoadRenderTexture(renderer->game_width, renderer->game_height);
	SetTextureFilter(renderer->target.texture, TEXTURE_FILTER_POINT);

	renderer->source.x = 0;
	renderer->source.y = 0;
	renderer->source.width = (float)renderer->target.texture.width;
	renderer->source.height = (float)-renderer->target.texture.height;

	renderer->dest.x = 0;
	renderer->dest.y = 0;
	renderer->dest.width = (float)renderer->target.texture.width;
	renderer->dest.height = (float)-renderer->target.texture.height;

	SetTargetFPS(60);
}

void framebuffer_draw(bool buffer[]) {

	for (uint32_t i = 0; i < 2048; i++) {

		uint16_t x = i % 64;
		uint16_t y = i / 64;

		if (buffer[i]) {
			DrawRectangle(x, y, 1, 1, SKYBLUE);
		}
	}
}

void renderer_update(Renderer *renderer, bool buffer[]) {

	// Calculate framebuffer scaling.
	float scale = MIN(
		(float)GetScreenWidth()/renderer->game_width,
		(float)GetScreenHeight()/renderer->game_height
	);

	BeginTextureMode(renderer->target);

		ClearBackground(DARKBLUE);
		framebuffer_draw(buffer);
	EndTextureMode();

	renderer->source.x = 0.0f;
	renderer->source.y = 0.0f;
	renderer->source.width = (float)renderer->target.texture.width;
	renderer->source.height = (float)-renderer->target.texture.height;

	renderer->dest.x = (GetScreenWidth() - ((float)renderer->game_width*scale)) * 0.5f;
	renderer->dest.y = (GetScreenHeight() - ((float)renderer->game_height*scale)) * 0.5f;
	renderer->dest.width = (float)renderer->game_width*scale;
	renderer->dest.height = (float)renderer->game_height*scale;

	BeginDrawing();

		ClearBackground(BLACK);

		DrawTexturePro(
			renderer->target.texture, renderer->source, renderer->dest,
			(Vector2){0,0}, 0.0f, RAYWHITE
		);
	EndDrawing();
}
