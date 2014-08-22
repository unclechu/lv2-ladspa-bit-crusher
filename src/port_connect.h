/*

Bit Crusher
connect_port()

License: GPLv3 (https://github.com/unclechu/lv2-ladspa-bit-crusher/blob/master/LICENSE)
Author: Viacheslav Lotsmanov

*/

static void connect_port_mono (
	INSTANCE_TYPE instance,
	PORT_CONNECT_PORT_TYPE port,
	PORT_CONNECT_DATA_TYPE data)
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
	INSTANCE_TYPE instance,
	PORT_CONNECT_PORT_TYPE port,
	PORT_CONNECT_DATA_TYPE data)
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
