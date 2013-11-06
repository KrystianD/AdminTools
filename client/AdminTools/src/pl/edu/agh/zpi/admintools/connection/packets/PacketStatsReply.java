package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.connection.Header;

public class PacketStatsReply implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	public static final String PACKET_STATS_REPLY = "pl.edu.agh.zpi.admintools.connection.packets.PacketStatsReply.PACKET_STATS_REPLY";
	private final byte type = Header.PACKET_STATS_REPLY;

	public static int CPU = 0;
	public static int RAM = 1;
	public static int TEMP = 2;
	public static int DISK = 3;

	private ArrayList<Float> points = new ArrayList<Float>();

	public PacketStatsReply() {
	}

	public void fromByteArray(byte[] array) {
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		for (short i = bArray.getShort(); i > 0; i--) {
			points.add(bArray.getFloat());
		}
	}

	public ArrayList<Float> getPoints() {
		return points;
	}

	public byte getType() {
		return type;
	}

	@Override
	public byte[] toByteArray() {
		return null;
	}
}
