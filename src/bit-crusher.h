/*

Bit Crusher DSP module

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

#ifndef BIT_CRUSHER_H
#define BIT_CRUSHER_H

#include <inttypes.h>
#include <tgmath.h>

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)

typedef struct {
	uint32_t sample_counter;
	float last_sample;
} Bit_crusher_state;

void bit_crusher_init_state(Bit_crusher_state *state);

void bit_crusher_process(
	Bit_crusher_state *state,

	uint32_t n_samples,

	// audio ports
	const float *input,
	float *output,

	// knobs
	const float drive,
	const uint8_t bit_depth,
	const uint8_t downsampling,
	const float output_gain);

#endif
