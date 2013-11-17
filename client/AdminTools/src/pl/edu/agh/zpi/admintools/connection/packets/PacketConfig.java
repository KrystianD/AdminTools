package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketConfig
 *	\brief Agent configuration.
 */
public class PacketConfig implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	//! Packet full class type.
	public static final String PACKET_CONFIG = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_CONFIG";
	private final byte type = Header.PACKET_CHANGE_REQUEST;
	private short agentId;
	private String tempPath = "";
	private short tempDivider;
	private ArrayList<ServiceConfig> servicesConfig = new ArrayList<ServiceConfig>();
	private short interval;
	private String name = "";

	/**
	 *	\fn public PacketConfig()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketConfig() {
	}

	/**
	 *	\fn public PacketConfig(short agentId, String tempPath, short tempDivider, ArrayList<ServiceConfig> servicesConfig, short interval, String name)
	 *	\brief Constructor, set variables value from given parameters.
	 *	\param agentId Agent id.
	 *	\param tempPath Temeprature path.
	 *	\param tempDivider Temeprature divider.
	 *	\param servicesConfig List of services configuration.
	 *	\param interval Time interval.
	 *	\param name Agent name.
	 */
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
	/**
	 *	\fn public void addServiceConfig(ServiceConfig sc)
	 *	\brief Add single service configuration.
	 *	\param sc Service configuration data.
	 *	\return None.
	 */
	public void addServiceConfig(ServiceConfig sc){
		servicesConfig.add(sc);
	}

	@Override
	public byte getType() {
		return type;
	}
	/**
	 *	\fn public short getAgentId()
	 *	\brief Get agent id.
	 *	\return Agent id.
	 */
	public short getAgentId() {
		return agentId;
	}
	/**
	 *	\fn public String getTempPath()
	 *	\brief Get temperature path.
	 *	\return Temperature path.
	 */
	public String getTempPath() {
		return tempPath;
	}
	/**
	 *	\fn public short getTempDivider()
	 *	\brief Get temperature divider.
	 *	\return Temperature divider.
	 */
	public short getTempDivider() {
		return tempDivider;
	}
	/**
	 *	\fn public ArrayList<ServiceConfig> getServicesConfig()
	 *	\brief Get whole services configuration list.
	 *	\return Services configuration list.
	 */
	public ArrayList<ServiceConfig> getServicesConfig() {
		return servicesConfig;
	}
	/**
	 *	\fn public short getInterval()
	 *	\brief Get agent time interval.
	 *	\return Time interval.
	 */
	public short getInterval() {
		return interval;
	}
	/**
	 *	\fn public String getName()
	 *	\brief Get agent name.
	 *	\return Agent name.
	 */
	public String getName(){
		return name;
	}

	/**
	 *	\fn public String toString()
	 *	\brief Human readable string representation of packet.
	 *	\return Packet as string.
	 */
	@Override
	public String toString() {
		return "PacketConfig [agentId=" + agentId + ", tempPath=" + tempPath
				+ ", tempDivider=" + tempDivider + ", servicesConfig="
				+ servicesConfig + ", interval=" + interval + "]";
	}

}
