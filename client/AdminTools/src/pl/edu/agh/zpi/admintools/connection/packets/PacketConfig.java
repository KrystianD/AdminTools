package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.connection.Header;

public class PacketConfig implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	public static final String PACKET_CONFIG = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_CONFIG";
	private final byte type = Header.PACKET_CHANGE_REQUEST;
	private short agentId;
	private String tempPath = "";
	private short tempDivider;
	private ArrayList<ServiceConfig> servicesConfig = new ArrayList<ServiceConfig>();
	private short interval;
	private String name = "";

	public PacketConfig() {
	}

	public PacketConfig(short agentId, String tempPath, short tempDivider,
			ArrayList<ServiceConfig> servicesConfig, short interval, String name) {
		this.agentId = agentId;
		this.tempPath = tempPath;
		this.tempDivider = tempDivider;
		this.servicesConfig = servicesConfig;
		this.interval = interval;
		this.name = name;
	}

	@Override
	public byte[] toByteArray() {
		byte[] tempPath = this.tempPath.getBytes();
		byte[] name = this.name.getBytes();
		short servicesConfigAmount = (short) servicesConfig.size();
		short servicesConfigSize = 0;

		for (ServiceConfig sc : servicesConfig) {
			servicesConfigSize += sc.getSize();
		}

		ByteBuffer bArray = ByteBuffer.allocate(2 + 2 + tempPath.length + 2 + 2
				+ servicesConfigSize + 2 + 2 + name.length);
		bArray.order(ByteOrder.LITTLE_ENDIAN);

		bArray.putShort(agentId);
		bArray.putShort((short) tempPath.length);
		bArray.put(tempPath);
		bArray.putShort(tempDivider);
		bArray.putShort(servicesConfigAmount);
		for (ServiceConfig sc : servicesConfig) {
			bArray.put(sc.toByteArray());
		}
		bArray.putShort(interval);
		bArray.putShort((short) name.length);
		bArray.put(name);
		
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);

		agentId = bArray.getShort();
		for (short i = bArray.getShort(); i > 0; i--) {
			char c = (char) bArray.get();
			tempPath += c;
		}
		tempDivider = bArray.getShort();
		for(short i = bArray.getShort(); i > 0 ; i--){
			servicesConfig.add(ServiceConfig.fromByteBuffer(bArray));
		}
		interval = bArray.getShort();
		for(short i = bArray.getShort(); i > 0 ; i--){
			char c = (char)bArray.get();
			name += c;
		}
	}

	public void addServiceConfig(ServiceConfig sc){
		servicesConfig.add(sc);
	}
	
	@Override
	public byte getType() {
		return type;
	}

	public short getAgentId() {
		return agentId;
	}

	public String getTempPath() {
		return tempPath;
	}

	public short getTempDivider() {
		return tempDivider;
	}

	public ArrayList<ServiceConfig> getServicesConfig() {
		return servicesConfig;
	}

	public short getInterval() {
		return interval;
	}

	public String getName(){
		return name;
	}
	
	@Override
	public String toString() {
		return "PacketConfig [agentId=" + agentId + ", tempPath=" + tempPath
				+ ", tempDivider=" + tempDivider + ", servicesConfig="
				+ servicesConfig + ", interval=" + interval + "]";
	}

}
