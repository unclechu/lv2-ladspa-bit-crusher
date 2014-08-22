/*

Bit Crusher
Port types

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

typedef struct {
	// audio ports
	PORT_TYPE_CF input;
	PORT_TYPE_F output;

	// knobs
	PORT_TYPE_CF drive;
	PORT_TYPE_CF bit_depth;
	PORT_TYPE_CF downsampling;
	PORT_TYPE_CF dry;
	PORT_TYPE_CF wet;
	PORT_TYPE_CF invert_wet_phase;

	// inner
	Bit_crusher_state *state;
} Plugin_MONO;

typedef struct {
	// audio ports
	PORT_TYPE_CF input_l;
	PORT_TYPE_CF input_r;
	PORT_TYPE_F output_l;
	PORT_TYPE_F output_r;

	// knobs
	PORT_TYPE_CF drive;
	PORT_TYPE_CF bit_depth;
	PORT_TYPE_CF downsampling;
	PORT_TYPE_CF dry;
	PORT_TYPE_CF wet;
	PORT_TYPE_CF invert_wet_phase;

	// inner
	Bit_crusher_state *state_l;
	Bit_crusher_state *state_r;
} Plugin_STEREO;
