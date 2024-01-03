#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include <raylib.h>

#define MIN(a, b) ((a)<(b)? (a) : (b))

typedef struct {
	uint16_t width;
	uint16_t height;

	uint8_t gameWidth;
	uint8_t gameHeight;

	RenderTexture2D target;
} Renderer;

void rendererInit(Renderer *renderer);
void rendererUpdate(Renderer *renderer, unsigned char buffer[2048]);
void drawFrameBuffer(unsigned char buffer[2048]);

#endif // !RENDERER_H
