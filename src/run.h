/*

Bit Crusher
run()

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

static void run_mono (
	INSTANCE_TYPE instance,
	NSAMPLES_TYPE n_samples)
{
	const Plugin_MONO *plugin = (const Plugin_MONO *)instance;

	// audio ports
	RUN_INPUT_TYPE input = plugin->input;
	RUN_OUTPUT_TYPE output = plugin->output;

	// knobs
	const float drive = prepare_drive_knob( *(plugin->drive) );
	const uint8_t bit_depth = prepare_bit_depth_knob( *(plugin->bit_depth) );
	const uint8_t downsampling = prepare_downsampling_knob( *(plugin->downsampling) );
	const float dry = prepare_dry_knob( *(plugin->dry) );
	const float wet = prepare_wet_knob( *(plugin->wet) );
	const uint8_t invert_wet_phase = prepare_invert_wet_phase_knob( *(plugin->invert_wet_phase) );

	bit_crusher_process(
		plugin->state,

		n_samples,

		input,
		output,

		drive,
		bit_depth,
		downsampling,
		dry,
		wet,
		invert_wet_phase
	);
}

static void run_stereo (
	INSTANCE_TYPE instance,
	NSAMPLES_TYPE n_samples)
{
	const Plugin_STEREO *plugin = (const Plugin_STEREO *)instance;

	// audio ports
	RUN_INPUT_TYPE input_l = plugin->input_l;
	RUN_INPUT_TYPE input_r = plugin->input_r;
	RUN_OUTPUT_TYPE output_l = plugin->output_l;
	RUN_OUTPUT_TYPE output_r = plugin->output_r;

	// knobs
	const float drive = prepare_drive_knob( *(plugin->drive) );
	const uint8_t bit_depth = prepare_bit_depth_knob( *(plugin->bit_depth) );
	const uint8_t downsampling = prepare_downsampling_knob( *(plugin->downsampling) );
	const float dry = prepare_dry_knob( *(plugin->dry) );
	const float wet = prepare_wet_knob( *(plugin->wet) );
	const uint8_t invert_wet_phase = prepare_invert_wet_phase_knob( *(plugin->invert_wet_phase) );

	bit_crusher_process(
		plugin->state_l,

		n_samples,

		input_l,
		output_l,

		drive,
		bit_depth,
		downsampling,
		dry,
		wet,
		invert_wet_phase
	);

	bit_crusher_process(
		plugin->state_r,

		n_samples,

		input_r,
		output_r,

		drive,
		bit_depth,
		downsampling,
		dry,
		wet,
		invert_wet_phase
	);
}
