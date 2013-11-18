package pl.edu.agh.zpi.admintools.connection.packets;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketConfigRequest
 *	\brief Requet for agent configuration.
 */
public class PacketConfigRequest implements IPacket {
	private final byte type = Header.PACKET_CONFIG_REQUEST;
	//! Packet full class type.
	public static final String ID = "pl.edu.agh.zpi.admintools.connection.packets.PacketConfigRequest.id";
	private short agentId;

	/**
	 *	\fn public PacketConfigRequest()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketConfigRequest() {
	}
	/**
	 *	\fn public PacketConfigRequest(short agentId)
	 *	\brief Constructor, set agent id.
	 *	\param agentId Agent id value.
	 */
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
	/**
	 *	\fn public short getAgentId()
	 *	\brief Get agent id.
	 *	\return Agent id.
	 */
	public short getAgentId() {
		return agentId;
	}

	@Override
	public byte getType() {
		return type;
	}

}
