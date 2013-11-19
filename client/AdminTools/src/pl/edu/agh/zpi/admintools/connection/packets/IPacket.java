/**
 *	\namespace pl.edu.agh.zpi.admintools.connection.packets
 *	\brief Packets submodule.
 */
package pl.edu.agh.zpi.admintools.connection.packets;

/**
 *	\interface IPacket
 *	\brief Packet data base interface.
 */
public interface IPacket {
	/**
	 *	\fn public byte[] toByteArray()
	 *	\brief Get byte array representation of packet.
	 *	\return Byte representation.
	 */
	public byte[] toByteArray();
	/**
	 *	\fn public void fromByteArray(byte[] array)
	 *	\brief Fill object with byte data.
	 *	\param[in] array Byte data as array.
	 *	\return None.
	 */
	public void fromByteArray(byte[] array);
	/**
	 *	\fn public byte getType()
	 *	\brief Get packet type.
	 *	\return Packet type.
	 */
	public byte getType();
}
