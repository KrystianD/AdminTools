package pl.edu.agh.zpi.admintools.connection;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Header implements IPacket{
	
	public static final byte PACKET_AUTH = 0;
	public static final byte PACKET_AGENTDATA = 1;
	public static final byte PACKET_START = 2;
	public static final byte PACKET_STOP = 3;
	
	public static final short PACKET_AUTH_SIZE = 1;
	public static final short PACKET_START_SIZE = 1;
	public static final short PACKET_STOP_SIZE = 1;
	
	
	private byte type;
	private short size;
	
	public Header(){
		type = PACKET_AUTH;
		size = 32; // FIXME ?!
	}
	
	public Header(byte type, byte size) {
		this.type = type;
		this.size = size;
	}
	
	public void setData(byte type, short size) {
		this.type = type;
		this.size = size;
	}

	public void setType(byte type){
		this.type = type;
	}
	
	public void setSize(short size){
		this.size = size;
	}
	
	public byte getType(){
		return type;
	}
	
	public short getSize(){
		return size;
	}
	
	public byte[] toByteArray(){
		ByteBuffer bArray = ByteBuffer.allocate(24);
		bArray.order(ByteOrder.BIG_ENDIAN);
		bArray.put(type);
		bArray.putShort(size);
		return bArray.array();
	}
}
