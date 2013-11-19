/*
Copyright (c) 2013, AGH University of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**
 *	\namespace pl.edu.agh.zpi.admintools.sensors
 *	\brief Sensors data transfer objects (DTOs).
 */
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

	/**
	 *	\fn public void setAgentData(AgentData agent)
	 *	\brief Set agent data from other object.
	 *	\param agent AgentData object.
	 *	\return None.
	 */
	public void setAgentData(AgentData agent){
		this.id = agent.id;
		this.name = agent.name;
		this.data = agent.data;
	}
}
