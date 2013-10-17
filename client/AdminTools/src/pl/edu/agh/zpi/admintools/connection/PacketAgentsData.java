package pl.edu.agh.zpi.admintools.connection;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

import pl.edu.agh.zpi.admintools.sensors.AgentData;
import android.util.Log;

public class PacketAgentsData implements IPacket {
	private final byte type = Header.PACKET_AGENTS_DATA;
	
	private ArrayList<AgentData> list = new ArrayList<AgentData>();
	
	public PacketAgentsData() {
		// TODO Auto-generated constructor stub
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
		Log.d("qwe",agentsAmount+"\n"+Arrays.toString(list.toArray()));
	}

	@Override
	public byte getType() {
		return type;
	}

}