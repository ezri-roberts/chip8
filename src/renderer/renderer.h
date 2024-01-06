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
	Rectangle source;
	Rectangle dest;
} Renderer;

void rendererInit(Renderer *renderer);
void rendererUpdate(Renderer *renderer, bool buffer[]);
void drawFrameBuffer(bool buffer[]);

#endif // !RENDERER_H
