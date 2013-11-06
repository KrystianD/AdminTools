package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;

public class PacketStatsRequest implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	private final byte type = Header.PACKET_STATS_REQUEST;
	public static final String PACKET_STATS_REQUEST = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_CONFIG";
	
	public static final byte CPU = 0;
	public static final byte RAM = 1;
	public static final byte TEMP = 2;
	public static final byte DISK = 3;
	
	private short agentId;
	private int startDate;
	private int endDate;
	private short points;
	private byte dataType;
	private String diskName;
	
	public PacketStatsRequest() {
	}

	@Override
	public byte[] toByteArray() {
		short stringSize = (short)diskName.getBytes().length;
		ByteBuffer bArray = ByteBuffer.allocate(2+4+4+2+1+2+stringSize);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putShort(agentId);
		bArray.putInt(startDate);
		bArray.putInt(endDate);
		bArray.putShort(points);
		bArray.put(dataType);
		bArray.putShort(stringSize);
		bArray.put(diskName.getBytes());
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
	}
	
	public void setAgentId(short agentId) {
		this.agentId = agentId;
	}

	public void setStartDate(int startDate) {
		this.startDate = startDate;
	}

	public void setEndDate(int endDate) {
		this.endDate = endDate;
	}

	public void setPoints(short points) {
		this.points = points;
	}

	public void setDataType(byte dataType) {
		this.dataType = dataType;
	}

	public void setDiskName(String diskName) {
		this.diskName = diskName;
	}

	public short getAgentId() {
		return agentId;
	}

	public int getStartDate() {
		return startDate;
	}

	public int getEndDate() {
		return endDate;
	}

	public short getPoints() {
		return points;
	}

	public byte getDataType() {
		return dataType;
	}

	public String getDiskName() {
		return diskName;
	}

	@Override
	public byte getType() {
		return type;
	}

}
