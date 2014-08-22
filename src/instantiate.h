/*

Bit Crusher
instantiate()

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

static INSTANCE_TYPE instantiate_mono (
	const DESCRIPTOR_TYPE *descriptor,
	double rate
	LV2_INSTANTIATE_ADDIT_ARGS)
{
	Plugin_MONO *plugin = (Plugin_MONO *)malloc(sizeof(Plugin_MONO));
	plugin->state = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	bit_crusher_init_state(plugin->state);
	return (INSTANCE_TYPE)plugin;
}

static INSTANCE_TYPE instantiate_stereo (
	const DESCRIPTOR_TYPE *descriptor,
	double rate
	LV2_INSTANTIATE_ADDIT_ARGS)
{
	Plugin_STEREO *plugin = (Plugin_STEREO *)malloc(sizeof(Plugin_STEREO));
	plugin->state_l = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	plugin->state_r = (Bit_crusher_state *)malloc(sizeof(Bit_crusher_state));
	bit_crusher_init_state(plugin->state_l);
	bit_crusher_init_state(plugin->state_r);
	return (INSTANCE_TYPE)plugin;
}
