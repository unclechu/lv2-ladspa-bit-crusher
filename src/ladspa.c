/*

Bit Crusher LADSPA

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "ladspa.h"

#include "bit-crusher.h"

#define MIN_DRIVE 0.0f
#define MAX_DRIVE 90.0f
#define MIN_BIT_DEPTH 1
#define MAX_BIT_DEPTH 32
#define MIN_DOWNSAMPLING 1
#define MAX_DOWNSAMPLING 128
#define MIN_OUTPUT_GAIN -90.0f
#define MAX_OUTPUT_GAIN 12.0f

// ports types {{{1

typedef enum { // PortIndex_MONO {{{2
	m_input = 0,
	m_output = 1,
	m_drive = 2,
	m_bit_depth = 3,
	m_downsampling = 4,
	m_output_gain = 5
} PortIndex_MONO; // }}}2

typedef enum { // PortIndex_STEREO {{{2
	s_input_l = 0,
	s_input_r = 1,
	s_output_l = 2,
	s_output_r = 3,
	s_drive = 4,
	s_bit_depth = 5,
	s_downsampling = 6,
	s_output_gain = 7
} PortIndex_STEREO; // }}}2

typedef struct { // Plugin_MONO {{{2
	// audio ports
	LADSPA_Data* input;
	LADSPA_Data* output;

	// knobs
	LADSPA_Data* drive;
	LADSPA_Data* bit_depth;
	LADSPA_Data* downsampling;
	LADSPA_Data* output_gain;

	// inner
	Bit_crusher_state *state;
} Plugin_MONO; // }}}2

typedef struct { // Plugin_STEREO {{{2
	// audio ports
	LADSPA_Data* input_l;
	LADSPA_Data* input_r;
	LADSPA_Data* output_l;
	LADSPA_Data* output_r;

	// knobs
	LADSPA_Data* drive;
	LADSPA_Data* bit_depth;
	LADSPA_Data* downsampling;
	LADSPA_Data* output_gain;

	// inner
	Bit_crusher_state *state_l;
	Bit_crusher_state *state_r;
} Plugin_STEREO; // }}}2

// ports types }}}1

// instantiate() {{{1

static LADSPA_Handle instantiate_mono (
	const LADSPA_Descriptor *descriptor,
	double rate)
{
	Plugin_MONO *plugin = (Plugin_MONO *)malloc(sizeof(Plugin_MONO));
	plugin->state = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	bit_crusher_init_state(plugin->state);
	return (LADSPA_Handle)plugin;
}

static LADSPA_Handle instantiate_stereo (
	const LADSPA_Descriptor *descriptor,
	double rate)
{
	Plugin_STEREO *plugin = (Plugin_STEREO *)malloc(sizeof(Plugin_STEREO));
	plugin->state_l = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	plugin->state_r = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	bit_crusher_init_state(plugin->state_l);
	bit_crusher_init_state(plugin->state_r);
	return (LADSPA_Handle)plugin;
}

// instantiate() }}}1

// connect_port() {{{1

#define portConnect(pname, pname_i) case pname_i: plugin->pname = data; break;

static void connect_port_mono (
	LADSPA_Handle instance,
	unsigned long port,
	LADSPA_Data* data)
{
	Plugin_MONO *plugin = (Plugin_MONO *)instance;

	switch((PortIndex_MONO)port) {
		portConnect(input, m_input);
		portConnect(output, m_output);
		portConnect(drive, m_drive);
		portConnect(bit_depth, m_bit_depth);
		portConnect(downsampling, m_downsampling);
		portConnect(output_gain, m_output_gain);
	}
}

static void connect_port_stereo (
	LADSPA_Handle instance,
	unsigned long port,
	LADSPA_Data* data)
{
	Plugin_STEREO *plugin = (Plugin_STEREO *)instance;

	switch((PortIndex_STEREO)port) {
		portConnect(input_l, s_input_l);
		portConnect(input_r, s_input_r);
		portConnect(output_l, s_output_l);
		portConnect(output_r, s_output_r);
		portConnect(drive, s_drive);
		portConnect(bit_depth, s_bit_depth);
		portConnect(downsampling, s_downsampling);
		portConnect(output_gain, s_output_gain);
	}
}

// connect_port() }}}1

// run {{{1

// helpers {{{2

	inline float prepare_drive_knob(LADSPA_Data val)
	{
		if (val < MIN_DRIVE) return MIN_DRIVE;
		else if (val > MAX_DRIVE) return MAX_DRIVE;
		else return val;
	}

	inline uint8_t prepare_bit_depth_knob(LADSPA_Data val)
	{
		if ((uint8_t)val < MIN_BIT_DEPTH) return MIN_BIT_DEPTH;
		else if ((uint8_t)val > MAX_BIT_DEPTH) return MAX_BIT_DEPTH;
		else return (uint8_t)val;
	}

	inline uint8_t prepare_downsampling_knob(LADSPA_Data val)
	{
		if ((uint8_t)val < MIN_DOWNSAMPLING) return MIN_DOWNSAMPLING;
		else if ((uint8_t)val > MAX_DOWNSAMPLING) return MAX_DOWNSAMPLING;
		else return (uint8_t)val;
	}

	inline float prepare_output_gain_knob(LADSPA_Data val)
	{
		if (val < MIN_OUTPUT_GAIN) return MIN_OUTPUT_GAIN;
		else if (val > MAX_OUTPUT_GAIN) return MAX_OUTPUT_GAIN;
		else return val;
	}

// helpers }}}2

static void run_mono (
	LADSPA_Handle instance,
	unsigned long n_samples)
{
	const Plugin_MONO *plugin = (const Plugin_MONO *)instance;

	// audio ports
	LADSPA_Data* input = plugin->input;
	LADSPA_Data* output = plugin->output;

	// knobs
	const float drive = prepare_drive_knob( *(plugin->drive) );
	const uint8_t bit_depth = prepare_bit_depth_knob( *(plugin->bit_depth) );
	const uint8_t downsampling = prepare_downsampling_knob( *(plugin->downsampling) );
	const float output_gain = prepare_output_gain_knob( *(plugin->output_gain) );

	bit_crusher_process(
		plugin->state,

		n_samples,

		input,
		output,

		drive,
		bit_depth,
		downsampling,
		output_gain
	);
}

static void run_stereo (
	LADSPA_Handle instance,
	unsigned long n_samples)
{
	const Plugin_STEREO *plugin = (const Plugin_STEREO *)instance;

	// audio ports
	LADSPA_Data* input_l = plugin->input_l;
	LADSPA_Data* input_r = plugin->input_r;
	LADSPA_Data* output_l = plugin->output_l;
	LADSPA_Data* output_r = plugin->output_r;

	// knobs
	const float drive = prepare_drive_knob( *(plugin->drive) );
	const uint8_t bit_depth = prepare_bit_depth_knob( *(plugin->bit_depth) );
	const uint8_t downsampling = prepare_downsampling_knob( *(plugin->downsampling) );
	const float output_gain = prepare_output_gain_knob( *(plugin->output_gain) );

	bit_crusher_process(
		plugin->state_l,

		n_samples,

		input_l,
		output_l,

		drive,
		bit_depth,
		downsampling,
		output_gain
	);

	bit_crusher_process(
		plugin->state_r,

		n_samples,

		input_r,
		output_r,

		drive,
		bit_depth,
		downsampling,
		output_gain
	);
}

// run() }}}1

// cleanup() {{{1

static void cleanup_mono ( LADSPA_Handle instance )
{
	const Plugin_MONO *plugin = (const Plugin_MONO *)instance;
	free( plugin->state );
	free( instance );
}

static void cleanup_stereo ( LADSPA_Handle instance )
{
	const Plugin_STEREO *plugin = (const Plugin_STEREO *)instance;
	free( plugin->state_l );
	free( plugin->state_r );
	free( instance );
}

// cleanup() }}}1

LADSPA_Descriptor* descriptor_mono = NULL;
LADSPA_Descriptor* descriptor_stereo = NULL;

// init and destroy ladspa descriptors {{{1

void __attribute__ ((constructor)) my_init(void)
{ // {{{2
	char** portNames;
	LADSPA_PortDescriptor* portDescriptors;
	LADSPA_PortRangeHint* portRangeHints;

	// mono {{{3
	descriptor_mono = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
	if (descriptor_mono) {
		descriptor_mono->UniqueID = 2014080401;
		descriptor_mono->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		descriptor_mono->Label = strdup("bit_crusher_mono");
		descriptor_mono->Name = strdup("Bit Crusher (Mono)");
		descriptor_mono->Maker = strdup("Viacheslav Lotsmanov");
		descriptor_mono->Copyright = strdup("License: GNU/GPLv3 by Free Software Foundation");

		descriptor_mono->PortCount = 6;

		// ports descriptors
		portDescriptors = (LADSPA_PortDescriptor *)calloc(descriptor_mono->PortCount, sizeof(LADSPA_PortDescriptor));
		descriptor_mono->PortDescriptors = (const LADSPA_PortDescriptor *)portDescriptors;
		portDescriptors[m_input] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		portDescriptors[m_output] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		portDescriptors[m_drive] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		portDescriptors[m_bit_depth] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		portDescriptors[m_downsampling] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		portDescriptors[m_output_gain] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

		// ports names
		portNames = (char **)calloc(descriptor_mono->PortCount, sizeof(char *));
		descriptor_mono->PortNames = (const char **)portNames;
		portNames[m_input] = strdup("Input");
		portNames[m_output] = strdup("Output");
		portNames[m_drive] = strdup("Drive (dB)");
		portNames[m_bit_depth] = strdup("Bit-depth");
		portNames[m_downsampling] = strdup("Downsampling (x)");
		portNames[m_output_gain] = strdup("Output gain (dB)");

		// ports range hints
		portRangeHints = ((LADSPA_PortRangeHint *) calloc(descriptor_mono->PortCount, sizeof(LADSPA_PortRangeHint)));
		descriptor_mono->PortRangeHints = (const LADSPA_PortRangeHint *)portRangeHints;
		portRangeHints[m_input].HintDescriptor = 0;
		portRangeHints[m_output].HintDescriptor = 0;
		portRangeHints[m_drive].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[m_drive].LowerBound = MIN_DRIVE;
		portRangeHints[m_drive].UpperBound = MAX_DRIVE;
		portRangeHints[m_bit_depth].HintDescriptor = (LADSPA_HINT_INTEGER | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[m_bit_depth].LowerBound = MIN_BIT_DEPTH;
		portRangeHints[m_bit_depth].UpperBound = MAX_BIT_DEPTH;
		portRangeHints[m_downsampling].HintDescriptor = (LADSPA_HINT_INTEGER | LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[m_downsampling].LowerBound = MIN_DOWNSAMPLING;
		portRangeHints[m_downsampling].UpperBound = MAX_DOWNSAMPLING;
		portRangeHints[m_output_gain].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[m_output_gain].LowerBound = MIN_OUTPUT_GAIN;
		portRangeHints[m_output_gain].UpperBound = MAX_OUTPUT_GAIN;

		// binds
		descriptor_mono->instantiate = instantiate_mono;
		descriptor_mono->connect_port = connect_port_mono;
		descriptor_mono->activate = NULL;
		descriptor_mono->run = run_mono;
		descriptor_mono->run_adding = NULL;
		descriptor_mono->set_run_adding_gain = NULL;
		descriptor_mono->deactivate = NULL;
		descriptor_mono->cleanup = cleanup_mono;
	} // mono }}}3

	// stereo {{{3
	descriptor_stereo = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
	if (descriptor_stereo) {
		descriptor_stereo->UniqueID = 2014080402;
		descriptor_stereo->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		descriptor_stereo->Label = strdup("bit_crusher_stereo");
		descriptor_stereo->Name = strdup("Bit Crusher (Stereo)");
		descriptor_stereo->Maker = strdup("Viacheslav Lotsmanov (unclechu)");
		descriptor_stereo->Copyright = strdup("License: GNU/GPLv3 by Free Software Foundation");

		descriptor_stereo->PortCount = 8;

		// ports descriptors
		portDescriptors = (LADSPA_PortDescriptor *)calloc(descriptor_stereo->PortCount, sizeof(LADSPA_PortDescriptor));
		descriptor_stereo->PortDescriptors = (const LADSPA_PortDescriptor *)portDescriptors;
		portDescriptors[s_input_l] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		portDescriptors[s_input_r] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		portDescriptors[s_output_l] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		portDescriptors[s_output_r] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		portDescriptors[s_drive] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		portDescriptors[s_bit_depth] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		portDescriptors[s_downsampling] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		portDescriptors[s_output_gain] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

		// ports names
		portNames = (char **)calloc(descriptor_stereo->PortCount, sizeof(char *));
		descriptor_stereo->PortNames = (const char **)portNames;
		portNames[s_input_l] = strdup("Left input");
		portNames[s_input_r] = strdup("Right input");
		portNames[s_output_l] = strdup("Left output");
		portNames[s_output_r] = strdup("Right output");
		portNames[s_drive] = strdup("Drive (dB)");
		portNames[s_bit_depth] = strdup("Bit-depth");
		portNames[s_downsampling] = strdup("Downsampling (x)");
		portNames[s_output_gain] = strdup("Output gain (dB)");

		// ports range hints
		portRangeHints = ((LADSPA_PortRangeHint *) calloc(descriptor_stereo->PortCount, sizeof(LADSPA_PortRangeHint)));
		descriptor_stereo->PortRangeHints = (const LADSPA_PortRangeHint *)portRangeHints;
		portRangeHints[s_input_l].HintDescriptor = 0;
		portRangeHints[s_input_r].HintDescriptor = 0;
		portRangeHints[s_output_l].HintDescriptor = 0;
		portRangeHints[s_output_r].HintDescriptor = 0;
		portRangeHints[s_drive].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[s_drive].LowerBound = MIN_DRIVE;
		portRangeHints[s_drive].UpperBound = MAX_DRIVE;
		portRangeHints[s_bit_depth].HintDescriptor = (LADSPA_HINT_INTEGER | LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[s_bit_depth].LowerBound = MIN_BIT_DEPTH;
		portRangeHints[s_bit_depth].UpperBound = MAX_BIT_DEPTH;
		portRangeHints[s_downsampling].HintDescriptor = (LADSPA_HINT_INTEGER | LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[s_downsampling].LowerBound = MIN_DOWNSAMPLING;
		portRangeHints[s_downsampling].UpperBound = MAX_DOWNSAMPLING;
		portRangeHints[s_output_gain].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
		portRangeHints[s_output_gain].LowerBound = MIN_OUTPUT_GAIN;
		portRangeHints[s_output_gain].UpperBound = MAX_OUTPUT_GAIN;

		// binds
		descriptor_stereo->instantiate = instantiate_stereo;
		descriptor_stereo->connect_port = connect_port_stereo;
		descriptor_stereo->activate = NULL;
		descriptor_stereo->run = run_stereo;
		descriptor_stereo->run_adding = NULL;
		descriptor_stereo->set_run_adding_gain = NULL;
		descriptor_stereo->deactivate = NULL;
		descriptor_stereo->cleanup = cleanup_stereo;
	} // stereo }}}3
} // my_init() }}}2

void __attribute__ ((destructor)) my_fini(void)
{
	// TODO: deleting descriptors
}

// init and destroy ladspa descriptors }}}1

const LADSPA_Descriptor* ladspa_descriptor(unsigned long Index) {
	switch (Index) {
	case 0:
		return descriptor_mono;
	case 1:
		return descriptor_stereo;
	default:
		return NULL;
	}
}
