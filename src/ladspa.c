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

#define INSTANCE_TYPE LADSPA_Handle
#define DESCRIPTOR_TYPE LADSPA_Descriptor
#define KNOB_VAL_TYPE LADSPA_Data
#define NSAMPLES_TYPE unsigned long

#include "ranges.h"

// ports types {{{1

#include "port_indexes.h"

#define PORT_TYPE_F LADSPA_Data*
#define PORT_TYPE_CF PORT_TYPE_F

#include "port_types.h"

// ports types }}}1

// instantiate() {{{1

#define LV2_INSTANTIATE_ADDIT_ARGS

#include "instantiate.h"

// instantiate() }}}1

// connect_port() {{{1

#define portConnect(pname, pname_i) case pname_i: plugin->pname = data; break;
#define portConnectConst(pname, pname_i) portConnect(pname, pname_i);

#define PORT_CONNECT_PORT_TYPE unsigned long
#define PORT_CONNECT_DATA_TYPE LADSPA_Data*

#include "port_connect.h"

// connect_port() }}}1

// run {{{1

#include "run_helpers.h"

#define RUN_INPUT_TYPE LADSPA_Data*
#define RUN_OUTPUT_TYPE LADSPA_Data*

#include "run.h"

// run() }}}1

#include "cleanup.h"

LADSPA_Descriptor* descriptor_mono = NULL;
LADSPA_Descriptor* descriptor_stereo = NULL;

void __attribute__ ((destructor)) my_fini(void)
{ // {{{1
	unsigned long i;

	// mono {{{2
	if (descriptor_mono) {
		free((char *)descriptor_mono->Label);
		free((char *)descriptor_mono->Name);
		free((char *)descriptor_mono->Maker);
		free((LADSPA_PortDescriptor *)descriptor_mono->PortDescriptors);
		for (i=0; i<descriptor_mono->PortCount; i++) {
			free((char *)(descriptor_mono->PortNames[i]));
		}
		free((char **)descriptor_mono->PortNames);
		free((LADSPA_PortRangeHint *)descriptor_mono->PortRangeHints);
		free(descriptor_mono);
	} // mono }}}2

	// stereo {{{2
	if (descriptor_stereo) {
		free((char *)descriptor_stereo->Label);
		free((char *)descriptor_stereo->Name);
		free((char *)descriptor_stereo->Maker);
		free((LADSPA_PortDescriptor *)descriptor_stereo->PortDescriptors);
		for (i=0; i<descriptor_stereo->PortCount; i++) {
			free((char *)(descriptor_stereo->PortNames[i]));
		}
		free((char **)descriptor_stereo->PortNames);
		free((LADSPA_PortRangeHint *)descriptor_stereo->PortRangeHints);
		free(descriptor_stereo);
	} // stereo }}}2
} // my_fini() destroying descriptors }}}1

const LADSPA_Descriptor* ladspa_descriptor(unsigned long Index)
{ // {{{1
	char** portNames;
	LADSPA_PortDescriptor* portDescriptors;
	LADSPA_PortRangeHint* portRangeHints;

	switch (Index) {

	case 0: // mono {{{2
		if (!descriptor_mono) {
			descriptor_mono = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
			if (!descriptor_mono) return NULL;

			descriptor_mono->UniqueID = 2014080401;
			descriptor_mono->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
			descriptor_mono->Label = strdup("bit_crusher_mono");
			descriptor_mono->Name = strdup("Bit Crusher (Mono)");
			descriptor_mono->Maker = strdup("Viacheslav Lotsmanov");

			descriptor_mono->PortCount = 8;

			// ports descriptors {{{3
			portDescriptors = (LADSPA_PortDescriptor *)calloc(descriptor_mono->PortCount, sizeof(LADSPA_PortDescriptor));
			descriptor_mono->PortDescriptors = (const LADSPA_PortDescriptor *)portDescriptors;
			portDescriptors[m_input] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
			portDescriptors[m_output] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
			portDescriptors[m_drive] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[m_bit_depth] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[m_downsampling] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[m_dry] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[m_wet] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[m_invert_wet_phase] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			// ports descriptors }}}3

			// ports names {{{3
			portNames = (char **)calloc(descriptor_mono->PortCount, sizeof(char *));
			descriptor_mono->PortNames = (const char **)portNames;
			portNames[m_input] = strdup("Input");
			portNames[m_output] = strdup("Output");
			portNames[m_drive] = strdup("Drive (dB)");
			portNames[m_bit_depth] = strdup("Bit-depth");
			portNames[m_downsampling] = strdup("Downsampling (x)");
			portNames[m_dry] = strdup("Dry (dB)");
			portNames[m_wet] = strdup("Wet (dB)");
			portNames[m_invert_wet_phase] = strdup("Invert wet phase");
			// ports names }}}3

			// ports range hints {{{3
			portRangeHints = ((LADSPA_PortRangeHint *) calloc(descriptor_mono->PortCount, sizeof(LADSPA_PortRangeHint)));
			descriptor_mono->PortRangeHints = (const LADSPA_PortRangeHint *)portRangeHints;
			portRangeHints[m_input].HintDescriptor = 0;
			portRangeHints[m_output].HintDescriptor = 0;
			portRangeHints[m_drive].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[m_drive].LowerBound = MIN_DRIVE;
			portRangeHints[m_drive].UpperBound = MAX_DRIVE;
			portRangeHints[m_bit_depth].HintDescriptor = (LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[m_bit_depth].LowerBound = MIN_BIT_DEPTH;
			portRangeHints[m_bit_depth].UpperBound = MAX_BIT_DEPTH;
			portRangeHints[m_downsampling].HintDescriptor = (LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[m_downsampling].LowerBound = MIN_DOWNSAMPLING;
			portRangeHints[m_downsampling].UpperBound = MAX_DOWNSAMPLING;
			portRangeHints[m_dry].HintDescriptor = (LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[m_dry].LowerBound = MIN_DRY;
			portRangeHints[m_dry].UpperBound = MAX_DRY;
			portRangeHints[m_wet].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[m_wet].LowerBound = MIN_WET;
			portRangeHints[m_wet].UpperBound = MAX_WET;
			portRangeHints[m_invert_wet_phase].HintDescriptor = (LADSPA_HINT_TOGGLED | LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[m_invert_wet_phase].LowerBound = MIN_INVERT_WET_PHASE;
			portRangeHints[m_invert_wet_phase].UpperBound = MAX_INVERT_WET_PHASE;
			// ports range hints }}}3

			// binds {{{3
			descriptor_mono->instantiate = instantiate_mono;
			descriptor_mono->connect_port = connect_port_mono;
			descriptor_mono->activate = NULL;
			descriptor_mono->run = run_mono;
			descriptor_mono->run_adding = NULL;
			descriptor_mono->set_run_adding_gain = NULL;
			descriptor_mono->deactivate = NULL;
			descriptor_mono->cleanup = cleanup_mono;
			// binds }}}3
		}

		return descriptor_mono;
		// mono }}}2

	case 1:
		// stereo {{{2
		if (!descriptor_stereo) {
			descriptor_stereo = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
			if (!descriptor_stereo) return NULL;

			descriptor_stereo->UniqueID = 2014080402;
			descriptor_stereo->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
			descriptor_stereo->Label = strdup("bit_crusher_stereo");
			descriptor_stereo->Name = strdup("Bit Crusher (Stereo)");
			descriptor_stereo->Maker = strdup("Viacheslav Lotsmanov (unclechu)");

			descriptor_stereo->PortCount = 10;

			// ports descriptors {{{3
			portDescriptors = (LADSPA_PortDescriptor *)calloc(descriptor_stereo->PortCount, sizeof(LADSPA_PortDescriptor));
			descriptor_stereo->PortDescriptors = (const LADSPA_PortDescriptor *)portDescriptors;
			portDescriptors[s_input_l] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
			portDescriptors[s_input_r] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
			portDescriptors[s_output_l] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
			portDescriptors[s_output_r] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
			portDescriptors[s_drive] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[s_bit_depth] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[s_downsampling] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[s_dry] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[s_wet] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			portDescriptors[s_invert_wet_phase] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
			// ports descriptors }}}3

			// ports names {{{3
			portNames = (char **)calloc(descriptor_stereo->PortCount, sizeof(char *));
			descriptor_stereo->PortNames = (const char **)portNames;
			portNames[s_input_l] = strdup("Left input");
			portNames[s_input_r] = strdup("Right input");
			portNames[s_output_l] = strdup("Left output");
			portNames[s_output_r] = strdup("Right output");
			portNames[s_drive] = strdup("Drive (dB)");
			portNames[s_bit_depth] = strdup("Bit-depth");
			portNames[s_downsampling] = strdup("Downsampling (x)");
			portNames[s_dry] = strdup("Dry (dB)");
			portNames[s_wet] = strdup("Wet (dB)");
			portNames[s_invert_wet_phase] = strdup("Invert wet phase");
			// ports names }}}3

			// ports range hints {{{3
			portRangeHints = ((LADSPA_PortRangeHint *) calloc(descriptor_stereo->PortCount, sizeof(LADSPA_PortRangeHint)));
			descriptor_stereo->PortRangeHints = (const LADSPA_PortRangeHint *)portRangeHints;
			portRangeHints[s_input_l].HintDescriptor = 0;
			portRangeHints[s_input_r].HintDescriptor = 0;
			portRangeHints[s_output_l].HintDescriptor = 0;
			portRangeHints[s_output_r].HintDescriptor = 0;
			portRangeHints[s_drive].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[s_drive].LowerBound = MIN_DRIVE;
			portRangeHints[s_drive].UpperBound = MAX_DRIVE;
			portRangeHints[s_bit_depth].HintDescriptor = (LADSPA_HINT_DEFAULT_MIDDLE | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[s_bit_depth].LowerBound = MIN_BIT_DEPTH;
			portRangeHints[s_bit_depth].UpperBound = MAX_BIT_DEPTH;
			portRangeHints[s_downsampling].HintDescriptor = (LADSPA_HINT_DEFAULT_1 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[s_downsampling].LowerBound = MIN_DOWNSAMPLING;
			portRangeHints[s_downsampling].UpperBound = MAX_DOWNSAMPLING;
			portRangeHints[s_dry].HintDescriptor = (LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[s_dry].LowerBound = MIN_DRY;
			portRangeHints[s_dry].UpperBound = MAX_DRY;
			portRangeHints[s_wet].HintDescriptor = (LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[s_wet].LowerBound = MIN_WET;
			portRangeHints[s_wet].UpperBound = MAX_WET;
			portRangeHints[s_invert_wet_phase].HintDescriptor = (LADSPA_HINT_TOGGLED | LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE);
			portRangeHints[s_invert_wet_phase].LowerBound = MIN_INVERT_WET_PHASE;
			portRangeHints[s_invert_wet_phase].UpperBound = MAX_INVERT_WET_PHASE;
			// ports range hints }}}3

			// binds {{{3
			descriptor_stereo->instantiate = instantiate_stereo;
			descriptor_stereo->connect_port = connect_port_stereo;
			descriptor_stereo->activate = NULL;
			descriptor_stereo->run = run_stereo;
			descriptor_stereo->run_adding = NULL;
			descriptor_stereo->set_run_adding_gain = NULL;
			descriptor_stereo->deactivate = NULL;
			descriptor_stereo->cleanup = cleanup_stereo;
			// binds }}}3
		}

		return descriptor_stereo;
		// stereo }}}2

	default:
		return NULL;

	} // switch
} // ladspa_descriptor() }}}1
