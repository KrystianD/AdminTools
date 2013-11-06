package pl.edu.agh.zpi.admintools.sensors;

import java.io.Serializable;
import java.nio.ByteBuffer;

public class AgentData implements Serializable{
	private static final long serialVersionUID = 1L;
	private short id;
	@SuppressWarnings("unused")
	private boolean oldData;
	private String name = "";
	private SensorsData data;

	private AgentData() {
	}

	public static AgentData fromByteBuffer(ByteBuffer buffer) {
		AgentData agent = new AgentData();
		agent.id = buffer.getShort();
		agent.oldData = (buffer.get() == 1);
		for (short i = buffer.getShort(); i > 0; i--) {
			char c = (char) buffer.get();
			agent.name += c;
		}
		agent.data = SensorsData.fromByteBuffer(buffer);
		return agent;
	}

	public short getId(){
		return id;
	}
	
	public SensorsData getData(){
		return data;
	}
	
	public String getName() {
		return name;
	}
	
	@Override
	public String toString() {
		return "AgentData [id=" + id + ", data=" + data + "]";
	}
	
	@Override
	public boolean equals(Object o) {
		if(o != null && !(o instanceof AgentData))
			return false;
		return ((AgentData)o).id == this.id;
	}
}
