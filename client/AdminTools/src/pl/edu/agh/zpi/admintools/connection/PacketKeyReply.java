package pl.edu.agh.zpi.admintools.connection;

import java.util.Arrays;

import android.util.Log;


public class PacketKeyReply implements IPacket {
	private final byte type = Header.PACKET_KEY_REPLY;
	
	private byte[] key = new byte[16];
	
	public PacketKeyReply() {
	}

	@Override
	public byte[] toByteArray() {
		return key;
	}

	@Override
	public void fromByteArray(byte[] array) {
		key = array;
		Log.d("qwe", Arrays.toString(key));
	}

	@Override
	public byte getType() {
		return type;
	}

}
