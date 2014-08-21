/*

Bit Crusher DSP module

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

#include "bit-crusher.h"

#include <tgmath.h>
#include <math.h>

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)

void bit_crusher_init_state(Bit_crusher_state *state)
{
	state->sample_counter = 0;
	state->last_sample = 0.0f;
}

inline float bit_crusher_limiter(const float val) {
	if (val > 1.0f) return 1.0f;
	else if (val < -1.0f) return -1.0f;
	else return val;
}

inline float bit_crusher_drive(const float val, const float drive)
{
	return bit_crusher_limiter( val * DB_CO(drive) );
}

inline int16_t bit_crusher_to_N_bit(const float val, const uint8_t bit_depth)
{
	if (val >= 1.0) return powf(2, bit_depth-1) - 1;
	else if (val <= -1.0) return powf(-2, bit_depth-1);
	else return floorf( val * -powf(-2, bit_depth-1) );
}

inline float bit_crusher_N_bit_to_float(const int16_t val, const uint8_t bit_depth)
{
	return val / -powf(-2, bit_depth-1);
}

inline float bit_crusher_crush_bit(const float val, const uint8_t bit_depth)
{
	return bit_crusher_N_bit_to_float(
		bit_crusher_to_N_bit(val, bit_depth),
		bit_depth);
}

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
	const float output_gain)
{
	uint32_t i;

	for (i=0; i<n_samples; i++) {
		state->sample_counter++;

		if (state->sample_counter < downsampling) {
			output[i] = state->last_sample;
			continue;
		} else state->sample_counter = 0;

		float sample =
			bit_crusher_crush_bit(
				bit_crusher_drive( input[i], drive ),
				bit_depth
			) * DB_CO(output_gain);
		state->last_sample = sample;
		output[i] = sample;
	}
}
