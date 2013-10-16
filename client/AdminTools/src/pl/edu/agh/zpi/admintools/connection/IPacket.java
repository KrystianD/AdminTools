package pl.edu.agh.zpi.admintools.connection;

public interface IPacket {
	public byte[] toByteArray();
	public void fromByteArray(byte[] array);
	public byte getType();
}
