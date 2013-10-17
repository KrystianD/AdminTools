package pl.edu.agh.zpi.admintools.connection;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Header {
	public static final int HEADER_SIZE = 3;

	public static final byte PACKET_AUTH = 0;
	public static final byte PACKET_REPLY = 1;
	public static final byte PACKET_AGENT_DATA = 2; // unused
	public static final byte PACKET_START = 3;
	public static final byte PACKET_STOP = 4;
	public static final byte PACKET_PING = 5;
	public static final byte PACKET_AGENTS_DATA = 6;
	public static final byte PACKET_KEY_REQUEST = 7;
	public static final byte PACKET_KEY_REPLY = 8;

	private byte type;
	private short size;

	public Header() {
	}

	public Header(byte type, short size) {
		this.type = type;
		this.size = size;
	}

	public void setData(byte type, short size) {
		this.type = type;
		this.size = size;
	}

	public void setType(byte type) {
		this.type = type;
	}

	public void setSize(short size) {
		this.size = size;
	}

	public byte getType() {
		return type;
	}

	public short getSize() {
		return size;
	}

	public byte[] toByteArray() {
		ByteBuffer bArray = ByteBuffer.allocate(3);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.put(type);
		bArray.putShort(size);
		return bArray.array();
	}

	public static Header fromByteArray(byte[] array) {
		Header header = new Header();
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		header.type = bArray.get();
		header.size = bArray.getShort();
		return header;
	}
}
