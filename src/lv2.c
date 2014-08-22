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

#define INSTANCE_TYPE LV2_Handle
#define DESCRIPTOR_TYPE LV2_Descriptor
#define KNOB_VAL_TYPE float
#define NSAMPLES_TYPE uint32_t

#include "ranges.h"

// ports types {{{1

#include "port_indexes.h"

#define PORT_TYPE_F float*
#define PORT_TYPE_CF const PORT_TYPE_F

#include "port_types.h"

// ports types }}}1

// instantiate() {{{1

#define LV2_INSTANTIATE_ADDIT_ARGS , const char *bundle_path, const LV2_Feature* const* features

#include "instantiate.h"

// instantiate() }}}1

// connect_port() {{{1

#define portConnect(pname, pname_i) case pname_i: plugin->pname = (float *)data; break;
#define portConnectConst(pname, pname_i) case pname_i: plugin->pname = (const float *)data; break;

#define PORT_CONNECT_PORT_TYPE uint32_t
#define PORT_CONNECT_DATA_TYPE void*

#include "port_connect.h"

// connect_port() }}}1

// run {{{1

#include "run_helpers.h"

#define RUN_INPUT_TYPE const float* const
#define RUN_OUTPUT_TYPE float* const

#include "run.h"

// run() }}}1

#include "cleanup.h"

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
