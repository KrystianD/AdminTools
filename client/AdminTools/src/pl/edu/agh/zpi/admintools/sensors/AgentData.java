package pl.edu.agh.zpi.admintools.sensors;

import java.io.Serializable;
import java.nio.ByteBuffer;

public class AgentData implements Serializable{
	private static final long serialVersionUID = 1L;
	private short id;
	private SensorsData data;

	private AgentData() {
	}

	public static AgentData fromByteBuffer(ByteBuffer buffer) {
		AgentData agent = new AgentData();
		agent.id = buffer.getShort();
		agent.data = SensorsData.fromByteBuffer(buffer);
		return agent;
	}

	@Override
	public String toString() {
		return "AgentData [id=" + id + ", data=" + data + "]";
	}
}
