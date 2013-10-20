package pl.edu.agh.zpi.admintools.connection.packets;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;

public class PacketStart implements IPacket {
	private final byte type = Header.PACKET_START;
	private short interval;
	
	public PacketStart() {
	}
	
	public PacketStart(short interval){
		this.interval = interval;
	}

	@Override
	public byte[] toByteArray() {
		ByteBuffer bArray = ByteBuffer.allocate(2);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putShort(interval);
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		interval = bArray.getShort();
	}

	@Override
	public byte getType() {
		return type;
	}

}
