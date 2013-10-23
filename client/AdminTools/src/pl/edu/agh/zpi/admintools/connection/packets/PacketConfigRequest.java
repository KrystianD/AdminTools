package pl.edu.agh.zpi.admintools.connection.packets;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;

public class PacketConfigRequest implements IPacket {
	private final byte type = Header.PACKET_CONFIG_REQUEST;
	public static final String ID = "pl.edu.agh.zpi.admintools.connection.packets.PacketConfigRequest.id";
	private short agentId;

	public PacketConfigRequest() {
	}

	public PacketConfigRequest(short agentId) {
		this.agentId = agentId;
	}

	@Override
	public byte[] toByteArray() {
		ByteBuffer bArray = ByteBuffer.allocate(2);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putShort(agentId);
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		agentId = bArray.getShort();
	}

	public short getAgentId() {
		return agentId;
	}
	
	@Override
	public byte getType() {
		return type;
	}

}
