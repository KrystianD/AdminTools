package pl.edu.agh.zpi.admintools.connection.packets;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketAuthKey
 *	\brief Authentication packet.
 */
public class PacketAuthKey implements IPacket {
	private final byte type = Header.PACKET_AUTH;

	private byte[] key = new byte[16];
	private boolean sendConfig = false;

	/**
	 *	\fn public PacketAuthKey()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketAuthKey() {
	}

	/**
	 *	\fn public PacketAuthKey(byte[] key, boolean sendConfig)
	 *	\brief Constructor, sets key and config flag.
	 *	\param key Authentication key.
	 *	\param sendConfig Send config flag.
	 */
	public PacketAuthKey(byte[] key, boolean sendConfig) {
		if(key.length == 16){
			this.key = key;
		}
		this.sendConfig = sendConfig;
	}
	/**
	 *	\fn public byte[] getKey()
	 *	\brief Get current authentication key.
	 *	\return Auth key.
	 */
	public byte[] getKey() {
		return key;
	}
	/**
	 *	\fn public void setKey(byte[] key)
	 *	\brief Set current authentication key.
	 *	\param key Auth key.
	 *	\return None.
	 */
	public void setKey(byte[] key) {
		if(key.length == 16)
			this.key = key;
	}

	public byte[] toByteArray(){
		ByteBuffer buffer = ByteBuffer.allocate(17);
		buffer.put(key);
		buffer.put(sendConfig == true ? (byte)1 : (byte)0);
		return buffer.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
		ByteBuffer buffer = ByteBuffer.wrap(array);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		buffer.get(key);
		sendConfig = (buffer.get() == 1);
	}

	@Override
	public byte getType() {
		return type;
	}
}
