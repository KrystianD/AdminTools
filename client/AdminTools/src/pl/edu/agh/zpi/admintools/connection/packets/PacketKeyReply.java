package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;

import pl.edu.agh.zpi.admintools.connection.Header;
import android.util.Log;

/**
 *	\class PacketKeyReply
 *	\brief Packet with authentication key.
 */
public class PacketKeyReply implements IPacket,Serializable {
	private static final long serialVersionUID = 1L;
	private static final byte type = Header.PACKET_KEY_REPLY;
	private static final int SIZE = 16;
	//! Packet full class type.
	public static final String PACKET_KEY_REPLY = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_KEY_REPLY";

	private byte[] key = new byte[SIZE];
	/**
	 *	\fn public PacketKeyReply()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketKeyReply() {
	}

	@Override
	public byte[] toByteArray() {
		return key;
	}

	@Override
	public void fromByteArray(byte[] array) {
		key = array;
		Log.d("qwe", this.toString());
	}

	@Override
	public byte getType() {
		return type;
	}
	/**
	 *	\fn public String toString()
	 *	\brief Human readable string representation of packet.
	 *	\return Packet as string.
	 */
	@Override
	public String toString() {
		String s = "";
		for(int i = 0 ;  i < SIZE ; i++){
			s += (char)key[i];
		}
		return s;
	}
}
