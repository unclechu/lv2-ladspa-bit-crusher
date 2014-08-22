/*

Bit Crusher
Port indexes types

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

typedef enum {
	m_input = 0,
	m_output = 1,
	m_drive = 2,
	m_bit_depth = 3,
	m_downsampling = 4,
	m_dry = 5,
	m_wet = 6,
	m_invert_wet_phase = 7
} PortIndex_MONO;

typedef enum {
	s_input_l = 0,
	s_input_r = 1,
	s_output_l = 2,
	s_output_r = 3,
	s_drive = 4,
	s_bit_depth = 5,
	s_downsampling = 6,
	s_dry = 7,
	s_wet = 8,
	s_invert_wet_phase = 9
} PortIndex_STEREO;
