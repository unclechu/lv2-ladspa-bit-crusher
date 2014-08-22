/*

Bit Crusher
run() helpers

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

inline float prepare_drive_knob(KNOB_VAL_TYPE val)
{
	if (val < MIN_DRIVE) return MIN_DRIVE;
	else if (val > MAX_DRIVE) return MAX_DRIVE;
	else return val;
}

inline uint8_t prepare_bit_depth_knob(KNOB_VAL_TYPE val)
{
	if ((uint8_t)val < MIN_BIT_DEPTH) return MIN_BIT_DEPTH;
	else if ((uint8_t)val > MAX_BIT_DEPTH) return MAX_BIT_DEPTH;
	else return (uint8_t)val;
}

inline uint8_t prepare_downsampling_knob(KNOB_VAL_TYPE val)
{
	if ((uint8_t)val < MIN_DOWNSAMPLING) return MIN_DOWNSAMPLING;
	else if ((uint8_t)val > MAX_DOWNSAMPLING) return MAX_DOWNSAMPLING;
	else return (uint8_t)val;
}

inline float prepare_dry_knob(KNOB_VAL_TYPE val)
{
	if (val < MIN_DRY) return MIN_DRY;
	else if (val > MAX_DRY) return MAX_DRY;
	else return val;
}

inline float prepare_wet_knob(KNOB_VAL_TYPE val)
{
	if (val < MIN_WET) return MIN_WET;
	else if (val > MAX_WET) return MAX_WET;
	else return val;
}

inline uint8_t prepare_invert_wet_phase_knob(KNOB_VAL_TYPE val)
{
	if ((uint8_t)val < MIN_INVERT_WET_PHASE) return MIN_INVERT_WET_PHASE;
	else if ((uint8_t)val > MAX_INVERT_WET_PHASE) return MAX_INVERT_WET_PHASE;
	else return (uint8_t)val;
}
