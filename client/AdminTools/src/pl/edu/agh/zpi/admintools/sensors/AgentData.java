package pl.edu.agh.zpi.admintools.sensors;

import java.io.Serializable;
import java.nio.ByteBuffer;

/**
 *	\class AgentData
 *	\brief Agent data structure as serializable POJO.
 */
public class AgentData implements Serializable{
	//! Serialization id.
	private static final long serialVersionUID = 1L;
	//! Agent id.
	private short id;
	//! Old data.
	@SuppressWarnings("unused")
	private boolean oldData;
	//! Agent name.
	private String name = "";
	//! Current sensor data from agent.
	private SensorsData data;

	private AgentData() {
	}

	/**
	 *	\fn public static AgentData fromByteBuffer(ByteBuffer buffer)
	 *	\brief Fill agent data with given byte buffer.
	 *	\param buffer Byte buffer with agent data.
	 *	\return Converted AgentData object.
	 */
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

	/**
	 *	\fn public short getId()
	 *	\brief Get agent id.
	 *	\return Agent id.
	 */
	public short getId(){
		return id;
	}

	/**
	 *	\fn public SensorsData getData()
	 *	\brief Get current sensor data from agent.
	 *	\return Current sensor data.
	 */
	public SensorsData getData(){
		return data;
	}

	/**
	 *	\fn public String getName()
	 *	\brief Get agent name.
	 *	\return Agent name.
	 */
	public String getName() {
		return name;
	}

	/**
	 *	\fn public String toString()
	 *	\brief Convert AgentData to string representation.
	 *	\return String representation.
	 */
	@Override
	public String toString() {
		return "AgentData [id=" + id + ", data=" + data + "]";
	}

	/**
	 *	\fn public boolean equals(Object o)
	 *	\brief Check equality of object with given parameter.
	 *	\param o Object to check.
	 *	\return If equlas.
	 */
	@Override
	public boolean equals(Object o) {
		if(o != null && !(o instanceof AgentData))
			return false;
		return ((AgentData)o).id == this.id;
	}
}
