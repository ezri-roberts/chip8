#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include <raylib.h>

#define MIN(a, b) ((a)<(b)? (a) : (b))

typedef struct Renderer {
	uint16_t width;
	uint16_t height;

	uint8_t game_width;
	uint8_t game_height;

	RenderTexture2D target;
	Rectangle source;
	Rectangle dest;
} Renderer;

void renderer_init(Renderer *renderer);
void renderer_update(Renderer *renderer, bool buffer[]);
void framebuffer_draw(bool buffer[]);

#endif // !RENDERER_H
