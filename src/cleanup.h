/*

Bit Crusher
cleanup()

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

static void cleanup_mono ( INSTANCE_TYPE instance )
{
	const Plugin_MONO *plugin = (const Plugin_MONO *)instance;
	free( plugin->state );
	free( instance );
}

static void cleanup_stereo ( INSTANCE_TYPE instance )
{
	const Plugin_STEREO *plugin = (const Plugin_STEREO *)instance;
	free( plugin->state_l );
	free( plugin->state_r );
	free( instance );
}
