package pl.edu.agh.zpi.admintools.connection.packets;

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketKeyRequest
 *	\brief Packet request for authentication key.
 */
public class PacketKeyRequest implements IPacket {
	private final byte type = Header.PACKET_KEY_REQUEST;
	/**
	 *	\fn public PacketKeyRequest()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketKeyRequest() {
	}

	@Override
	public byte[] toByteArray() {
		return new byte[0];
	}

	@Override
	public void fromByteArray(byte[] array) {
	}

	@Override
	public byte getType() {
		return type;
	}

}
