/*

Bit Crusher LV2

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

#include <inttypes.h>

#include "bit-crusher.h"

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"

#define URI_MONO "https://github.com/unclechu/lv2-ladspa-bit-crusher/mono"
#define URI_STEREO "https://github.com/unclechu/lv2-ladspa-bit-crusher/stereo"

#define MIN_DRIVE 0.0f
#define MAX_DRIVE 90.0f
#define MIN_BIT_DEPTH 1
#define MAX_BIT_DEPTH 32
#define MIN_DOWNSAMPLING 1
#define MAX_DOWNSAMPLING 128
#define MIN_DRY -90.0f
#define MAX_DRY 12.0f
#define MIN_WET -90.0f
#define MAX_WET 12.0f
#define MIN_INVERT_WET_PHASE 0.0f
#define MAX_INVERT_WET_PHASE 1.0f

// ports types {{{1

typedef enum { // PortIndex_MONO {{{2
	m_input = 0,
	m_output = 1,
	m_drive = 2,
	m_bit_depth = 3,
	m_downsampling = 4,
	m_dry = 5,
	m_wet = 6,
	m_invert_wet_phase = 7
} PortIndex_MONO; // }}}2

typedef enum { // PortIndex_STEREO {{{2
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
} PortIndex_STEREO; // }}}2

typedef struct { // Plugin_MONO {{{2
	// audio ports
	const float* input;
	float* output;

	// knobs
	const float* drive;
	const float* bit_depth;
	const float* downsampling;
	const float* dry;
	const float* wet;
	const float* invert_wet_phase;

	// inner
	Bit_crusher_state *state;
} Plugin_MONO; // }}}2

typedef struct { // Plugin_STEREO {{{2
	// audio ports
	const float* input_l;
	const float* input_r;
	float* output_l;
	float* output_r;

	// knobs
	const float* drive;
	const float* bit_depth;
	const float* downsampling;
	const float* dry;
	const float* wet;
	const float* invert_wet_phase;

	// inner
	Bit_crusher_state *state_l;
	Bit_crusher_state *state_r;
} Plugin_STEREO; // }}}2

// ports types }}}1

// instantiate() {{{1

static LV2_Handle instantiate_mono (
	const LV2_Descriptor *descriptor,
	double rate,
	const char *bundle_path,
	const LV2_Feature* const* features)
{
	Plugin_MONO *plugin = (Plugin_MONO *)malloc(sizeof(Plugin_MONO));
	plugin->state = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	bit_crusher_init_state(plugin->state);
	return (LV2_Handle)plugin;
}

static LV2_Handle instantiate_stereo (
	const LV2_Descriptor *descriptor,
	double rate,
	const char *bundle_path,
	const LV2_Feature* const* features)
{
	Plugin_STEREO *plugin = (Plugin_STEREO *)malloc(sizeof(Plugin_STEREO));
	plugin->state_l = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	plugin->state_r = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	bit_crusher_init_state(plugin->state_l);
	bit_crusher_init_state(plugin->state_r);
	return (LV2_Handle)plugin;
}

// instantiate() }}}1

// connect_port() {{{1

#define portConnect(pname, pname_i) case pname_i: plugin->pname = (float *)data; break;
#define portConnectConst(pname, pname_i) case pname_i: plugin->pname = (const float *)data; break;

static void connect_port_mono (
	LV2_Handle instance,
	uint32_t port,
	void* data)
{
	Plugin_MONO *plugin = (Plugin_MONO *)instance;

	switch((PortIndex_MONO)port) {
		portConnectConst(input, m_input);
		portConnect(output, m_output);
		portConnectConst(drive, m_drive);
		portConnectConst(bit_depth, m_bit_depth);
		portConnectConst(downsampling, m_downsampling);
		portConnectConst(dry, m_dry);
		portConnectConst(wet, m_wet);
		portConnectConst(invert_wet_phase, m_invert_wet_phase);
	}
}

static void connect_port_stereo (
	LV2_Handle instance,
	uint32_t port,
	void* data)
{
	Plugin_STEREO *plugin = (Plugin_STEREO *)instance;

	switch((PortIndex_STEREO)port) {
		portConnectConst(input_l, s_input_l);
		portConnectConst(input_r, s_input_r);
		portConnect(output_l, s_output_l);
		portConnect(output_r, s_output_r);
		portConnectConst(drive, s_drive);
		portConnectConst(bit_depth, s_bit_depth);
		portConnectConst(downsampling, s_downsampling);
		portConnectConst(dry, s_dry);
		portConnectConst(wet, s_wet);
		portConnectConst(invert_wet_phase, s_invert_wet_phase);
	}
}

// connect_port() }}}1

// run {{{1

// helpers {{{2

	inline float prepare_drive_knob(float val)
	{
		if (val < MIN_DRIVE) return MIN_DRIVE;
		else if (val > MAX_DRIVE) return MAX_DRIVE;
		else return val;
	}

	inline uint8_t prepare_bit_depth_knob(float val)
	{
		if ((uint8_t)val < MIN_BIT_DEPTH) return MIN_BIT_DEPTH;
		else if ((uint8_t)val > MAX_BIT_DEPTH) return MAX_BIT_DEPTH;
		else return (uint8_t)val;
	}

	inline uint8_t prepare_downsampling_knob(float val)
	{
		if ((uint8_t)val < MIN_DOWNSAMPLING) return MIN_DOWNSAMPLING;
		else if ((uint8_t)val > MAX_DOWNSAMPLING) return MAX_DOWNSAMPLING;
		else return (uint8_t)val;
	}

	inline float prepare_dry_knob(float val)
	{
		if (val < MIN_DRY) return MIN_DRY;
		else if (val > MAX_DRY) return MAX_DRY;
		else return val;
	}

	inline float prepare_wet_knob(float val)
	{
		if (val < MIN_WET) return MIN_WET;
		else if (val > MAX_WET) return MAX_WET;
		else return val;
	}

	inline uint8_t prepare_invert_wet_phase_knob(float val)
	{
		if ((uint8_t)val < MIN_INVERT_WET_PHASE) return MIN_INVERT_WET_PHASE;
		else if ((uint8_t)val > MAX_INVERT_WET_PHASE) return MAX_INVERT_WET_PHASE;
		else return (uint8_t)val;
	}

// helpers }}}2

static void run_mono (
	LV2_Handle instance,
	uint32_t n_samples)
{
	const Plugin_MONO *plugin = (const Plugin_MONO *)instance;

	// audio ports
	const float* const input = plugin->input;
	float* const output = plugin->output;

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
	LV2_Handle instance,
	uint32_t n_samples)
{
	const Plugin_STEREO *plugin = (const Plugin_STEREO *)instance;

	// audio ports
	const float* const input_l = plugin->input_l;
	const float* const input_r = plugin->input_r;
	float* const output_l = plugin->output_l;
	float* const output_r = plugin->output_r;

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

// run() }}}1

// cleanup() {{{1

static void cleanup_mono ( LV2_Handle instance )
{
	const Plugin_MONO *plugin = (const Plugin_MONO *)instance;
	free( plugin->state );
	free( instance );
}

static void cleanup_stereo ( LV2_Handle instance )
{
	const Plugin_STEREO *plugin = (const Plugin_STEREO *)instance;
	free( plugin->state_l );
	free( plugin->state_r );
	free( instance );
}

// cleanup() }}}1

// descriptors {{{1

static const LV2_Descriptor descriptor_mono = {
	URI_MONO,
	instantiate_mono,
	connect_port_mono,
	NULL,
	run_mono,
	NULL,
	cleanup_mono
};

static const LV2_Descriptor descriptor_stereo = {
	URI_STEREO,
	instantiate_stereo,
	connect_port_stereo,
	NULL,
	run_stereo,
	NULL,
	cleanup_stereo
};

// descriptors }}}1

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor ( uint32_t index ) {
	switch (index) {
	case 0:
		return &descriptor_mono;
	case 1:
		return &descriptor_stereo;
	default:
		return NULL;
	}
}
