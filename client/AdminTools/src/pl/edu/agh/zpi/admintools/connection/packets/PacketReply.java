package pl.edu.agh.zpi.admintools.connection.packets;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;
/**
 * 
 * @author jacek
 *
 * Zwraca 
 */
public class PacketReply implements IPacket {
	private final byte type = Header.PACKET_REPLY;
	
	private int value;
	
	public PacketReply() {
	}

	public int getValue(){
		return value;
	}
	
	@Override
	public byte[] toByteArray(){
		ByteBuffer bArray = ByteBuffer.allocate(4);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putInt(value);
		return bArray.array();
	}
	
	@Override
	public void fromByteArray(byte[] array){
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		this.value = bArray.getInt();
	}

	@Override
	public byte getType() {
		return type;
	}

}