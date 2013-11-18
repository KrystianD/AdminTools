package pl.edu.agh.zpi.admintools.connection;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 *	\class Header
 *	\brief Packet header configuration and utility functions.
 */
public class Header {
	//! Header size.
	public static final int HEADER_SIZE = 3;
	//! PacketAuthKey flag.
	public static final byte PACKET_AUTH = 0;
	//! PacketReply flag.
	public static final byte PACKET_REPLY = 1;
	//! Packet agent data flag.
	public static final byte PACKET_AGENT_DATA = 2; // unused
	//! PacketStart flag.
	public static final byte PACKET_START = 3;
	//! Packet stop flag.
	public static final byte PACKET_STOP = 4;
	//! Packet ping flag.
	public static final byte PACKET_PING = 5;
	//! PacketAgentsData flag.
	public static final byte PACKET_AGENTS_DATA = 6;
	//! PacketKeyRequest flag.
	public static final byte PACKET_KEY_REQUEST = 7;
	//! PacketKeyReply flag.
	public static final byte PACKET_KEY_REPLY = 8;
	//! PacketConfig flag.
	public static final byte PACKET_CONFIG = 9; // unused
	//! PacketConfigRequest flag.
	public static final byte PACKET_CONFIG_REQUEST = 10;
	//! Packet configuration reply flag.
	public static final byte PACKET_CONFIG_REPLY = 11;
	//! Packet change request flag.
	public static final byte PACKET_CHANGE_REQUEST = 12;
	//! Packet change reply flag.
	public static final byte PACKET_CHANGE_REPLY = 13;
	//! PacketStatsRequest flag.
	public static final byte PACKET_STATS_REQUEST = 14;
	//! PacketStatsReply flag.
	public static final byte PACKET_STATS_REPLY = 15;

	private byte type;
	private short size;

	/**
	 *	\fn public Header()
	 *	\brife Default constructor, do nothing.
	 */
	public Header() {
	}
	/**
	 *	\fn public Header(byte type, short size)
	 *	\brief Constructor, set header type and size.
	 *	\param type Header type.
	 *	\param size Header size.
	 */
	public Header(byte type, short size) {
		this.type = type;
		this.size = size;
	}
	/**
	 *	\fn public void setData(byte type, short size)
	 *	\brief Set basic header data.
	 *	\param type Header type.
	 *	\param size Header size.
	 *	\return None.
	 */
	public void setData(byte type, short size) {
		this.type = type;
		this.size = size;
	}
	/**
	 *	\fn public void setType(byte type)
	 *	\brief Set header type flag.
	 *	\param type Header type flag.
	 *	\return None.
	 */
	public void setType(byte type) {
		this.type = type;
	}
	/**
	 *	\fn public void setSize(short size)
	 *	\brief Set header size.
	 *	\param size Header size.
	 *	\return None.
	 */
	public void setSize(short size) {
		this.size = size;
	}
	/**
	 *	\fn public byte getType()
	 *	\brief Get header type flag.
	 *	\return Header type flag.
	 */
	public byte getType() {
		return type;
	}
	/**
	 *	\fn public short getSize()
	 *	\brief Get header size.
	 *	\return Header size.
	 */
	public short getSize() {
		return size;
	}
	/**
	 *	\fn public byte[] toByteArray()
	 *	\brief Get byte representation of Header object.
	 *	\return Byte array with Header data.
	 */
	public byte[] toByteArray() {
		ByteBuffer bArray = ByteBuffer.allocate(3);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.put(type);
		bArray.putShort(size);
		return bArray.array();
	}
	/**
	 *	\fn public static Header fromByteArray(byte[] array)
	 *	\brief Create new Header object from its byte representation.
	 *	\param array Array of bytes with Header data to reproduce.
	 *	\return Instance of new Header object.
	 */
	public static Header fromByteArray(byte[] array) {
		Header header = new Header();
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		header.type = bArray.get();
		header.size = bArray.getShort();
		return header;
	}
}
