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
package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.connection.Header;
import pl.edu.agh.zpi.admintools.sensors.AgentData;

/**
 *	\class PacketAgentsData
 *	\brief Combined agents data.
 */
public class PacketAgentsData implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	//! Packet full class type.
	public static final String PACKET_AGENTS_DATA = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_AGENTS_DATA";
	private static final byte type = Header.PACKET_AGENTS_DATA;

	private ArrayList<AgentData> list = new ArrayList<AgentData>();
	/**
	 *	\fn public PacketAgentsData()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketAgentsData() {
		// TODO Auto-generated constructor stub
	}
	/**
	 *	\fn public AgentData[] getAgentsArray()
	 *	\brief Get all agents data as array.
	 *	\return Array of agents data.
	 */
	public AgentData[] getAgentsArray(){
		return (AgentData[])list.toArray();
	}
	/**
	 *	\fn public ArrayList<AgentData> getAgentsList()
	 *	\brief Get all agents data as array list.
	 *	\return Array list of agents data.
	 */
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
		//try{
		short agentsAmount;
		ByteBuffer buffer = ByteBuffer.wrap(array);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		agentsAmount = buffer.getShort();
		for(int i = 0 ; i < agentsAmount ; i++){
			list.add(AgentData.fromByteBuffer(buffer));
		}
		//}catch(Exception e){
		//	System.err.print(this.toString());
		//}
	}

	@Override
	public byte getType() {
		return type;
	}

	/**
	 *	\fn public String toString()
	 *	\brief Human readable string representation of packet.
	 *	\return Packet as string.
	 */
	@Override
	public String toString() {
		return "PacketAgentsData [list=" + list + "]";
	}
}