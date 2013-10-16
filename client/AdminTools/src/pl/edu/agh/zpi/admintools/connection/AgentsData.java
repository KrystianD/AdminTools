package pl.edu.agh.zpi.admintools.connection;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

import android.util.Log;

public class AgentsData implements IPacket {
	private final byte type = Header.PACKET_AGENTS_DATA;
	
	private ArrayList<AgentData> list = new ArrayList<AgentData>();
	
	public AgentsData() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public byte[] toByteArray() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void fromByteArray(byte[] array) {
		short id,size;
		float temp;
		ByteBuffer buffer = ByteBuffer.wrap(array);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		
		size = buffer.getShort();
		
		while(size-- > 0){
			id = buffer.getShort();
			temp = buffer.getFloat();
			list.add(new AgentData(id,temp));
		}
		Log.d("qwe", Arrays.toString(list.toArray()));
	}

	@Override
	public byte getType() {
		return type;
	}

}