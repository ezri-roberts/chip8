#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

void vm_framebuffer_clear(Vm *vm) {
	memset(vm->framebuffer, 0, sizeof(vm->framebuffer));
}

void vm_init(Vm *vm) {

	renderer_init(&vm->renderer);

	vm->pc = 0x200;
	vm->I = 0;
	vm->sp = 0;

	vm_framebuffer_clear(vm);

	// Clear Stack
	memset(vm->stack, 0, sizeof(vm->stack));
	// Clear registers V0-VF
	memset(vm->V, 0, sizeof(vm->V));
	// Clear memory.
	memset(vm->memory, 0, sizeof(vm->memory));
	memset(vm->keypad, 0, sizeof(vm->keypad));

	const uint8_t fontset[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
		0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
		0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
		0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
		0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
		0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
		0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
		0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
		0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
		0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
		0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
		0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
		0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
		0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
		0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
		0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
	};
	
	// Load fontset into memory.
	for (int i = 0; i < 80; i++) {
		vm->memory[i] = fontset[i];
	}
	
	// Reset timers.
	vm->delay_timer = 0;
	vm->sound_timer = 0;
	vm->clock_rate = 700;

	// Audio.
	// Freq is 440hz for middle A
	
	InitAudioDevice();

	SetAudioStreamBufferSizeDefault(512);
	vm->audio_stream = LoadAudioStream(44100, 16, 1);
	// SetAudioStreamVolume(vm->audio_stream, 3000.0f);
	SetAudioStreamCallback(vm->audio_stream, vm_audio_callback);

	// PlayAudioStream(vm->audio_stream);
	PlayAudioStream(vm->audio_stream);

	vm->state = RUNNING;
}

void vm_audio_callback(void *buffer_data, uint32_t frames) {

	short *data = (short *)buffer_data;

	// sample rate
	// wave freq
	const int samples_per_second = 44100;
	const int tone_hz = 440;
	const int16_t tone_volume = 4000;

	static uint32_t running_index = 0;
	const int32_t wave_period = samples_per_second / tone_hz;
	const int32_t half_wave_period = wave_period / 2;

	const int bytes_per_sample = sizeof(int16_t) * 2;

	int bytes_to_write = 800 * bytes_per_sample;
	int sample_count = bytes_to_write / bytes_per_sample;

	for (int i = 0; i < sample_count; i++) {
	
		int16_t sample_value = ((running_index++ / half_wave_period) % 2) ?
			tone_volume : -tone_volume;
		*data++ = sample_value;
		*data++ = sample_value;
	}

	// for (int i = 0; i < frames/2; i++) {
	//
	// 	data[i] = ((running_index++ / half_wave_period) % 2) ?
	// 			  8000 : -8000;
	// }

}

void vm_load_rom(Vm *vm, const char *name) {

	FILE *rom;
	rom = fopen(name, "rb");

	if (rom == NULL) {
		printf("Error reading file '%s'", name);
		exit(1);
	}

	// Get rom size.
	fseek(rom, 0, SEEK_END);
	const size_t rom_size = ftell(rom);
	rewind(rom);

	const size_t max_size = sizeof(vm->memory) - 0x200; 

	// Make sure rom is not too big.
	if (rom_size > max_size) {
		printf("ROM '%s' is too big.\n", name);
		printf("ROM can be no bigger than %zu bytes.", max_size);
		exit(1);
	}

	// Load rom into memory.
	// All programs are loaded in starting at address 0x200.
	fread(&vm->memory[0x200], rom_size, 1, rom);
	printf("Read %zu bytes from file '%s'\n", rom_size, name);

	fclose(rom);
}

// Lookup table for instruction execution functions.
// Indexed by the first nibble of an opcode.
void(*lookup[16])(Vm*) = {
	opcodeZero,
	opcodeOne,
	opcodeTwo,
	opcodeThree,
	opcodeFour,
	opcodeFive,
	opcodeSix,
	opcodeSeven,
	opcodeEight,
	opcodeNine,
	opcodeA,
	opcodeB,
	opcodeC,
	opcodeD,
	opcodeE,
	opcodeF,
};

void vm_cycle(Vm *vm) {

	vm->inst.opcode = OPCODE(vm->memory[vm->pc], vm->memory[vm->pc+1]);

	// Mask off parts of the instruction.
	vm->inst.first = FIRST(vm->inst.opcode);
	vm->inst.x = X(vm->inst.opcode);
	vm->inst.y = Y(vm->inst.opcode);
	vm->inst.n = N(vm->inst.opcode);
	vm->inst.nnn = NNN(vm->inst.opcode);
	vm->inst.kk = KK(vm->inst.opcode);

	// printf("ADDR %X | ", vm->pc);
	vm->pc += 2; // Pre-increment the counter for the next opcode.

	// Execute the instruction.
	lookup[vm->inst.first](vm);

	// Decrement timers.
	if (vm->delay_timer > 0) {
		vm->delay_timer--;
	}

	if (vm->sound_timer > 0) {
		// PlayAudioStream(vm->audio_stream);
		ResumeAudioStream(vm->audio_stream);
		vm->sound_timer--;
	} else {
		// StopAudioStream(vm->audio_stream);
		PauseAudioStream(vm->audio_stream);
	}
}

void vm_input_update(Vm *vm) {

	const uint8_t lookup[] = {
		88, 49, 50, 51,
		81, 87, 69, 65,
		83, 68, 90, 67,
		52, 82, 70, 86,
	};
	
	uint8_t key = GetKeyPressed();

	for (uint8_t i; i < 16; i++) {

		if (IsKeyPressed(lookup[i])) {
			vm->keypad[i] = true;
		} else if (IsKeyReleased(lookup[i])) {
			vm->keypad[i] = false;
		}
	}
}

void vm_update(Vm *vm) {

	while (!WindowShouldClose()) {

		if (IsKeyPressed(KEY_SPACE)) {
			vm->state = !vm->state;
		}

		// Handle input.

		if (vm->state == RUNNING) {

			for (uint32_t i = 0; i < vm->clock_rate/60; i++) {
				vm_input_update(vm);
				vm_cycle(vm);
			}
		}

		renderer_update(&vm->renderer, vm->framebuffer);
	}

	StopAudioStream(vm->audio_stream);
	UnloadAudioStream(vm->audio_stream);
	CloseAudioDevice();
	CloseWindow();
}

