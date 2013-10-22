package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.connection.Header;
import pl.edu.agh.zpi.admintools.sensors.AgentData;

public class PacketAgentsData implements IPacket, Serializable {	
	private static final long serialVersionUID = 1L;
	public static final String PACKET_AGENTS_DATA = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_AGENTS_DATA";
	private static final byte type = Header.PACKET_AGENTS_DATA;
	
	private ArrayList<AgentData> list = new ArrayList<AgentData>();
	
	public PacketAgentsData() {
		// TODO Auto-generated constructor stub
	}

	public AgentData[] getAgentsArray(){
		return (AgentData[])list.toArray();
	}
	
	public ArrayList<AgentData> getAgentsList(){
		return list;
	}
	
	@Override
	public byte[] toByteArray() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void fromByteArray(byte[] array) {
		short agentsAmount;
		ByteBuffer buffer = ByteBuffer.wrap(array);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		agentsAmount = buffer.getShort();
		for(int i = 0 ; i < agentsAmount ; i++){
			list.add(AgentData.fromByteBuffer(buffer));
		}
	}

	@Override
	public byte getType() {
		return type;
	}

	@Override
	public String toString() {
		return "PacketAgentsData [list=" + list + "]";
	}
}