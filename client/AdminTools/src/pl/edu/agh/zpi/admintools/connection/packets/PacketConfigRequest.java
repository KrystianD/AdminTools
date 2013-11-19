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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketConfigRequest
 *	\brief Requet for agent configuration.
 */
public class PacketConfigRequest implements IPacket {
	private final byte type = Header.PACKET_CONFIG_REQUEST;
	//! Packet full class type.
	public static final String ID = "pl.edu.agh.zpi.admintools.connection.packets.PacketConfigRequest.id";
	private short agentId;

	/**
	 *	\fn public PacketConfigRequest()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketConfigRequest() {
	}
	/**
	 *	\fn public PacketConfigRequest(short agentId)
	 *	\brief Constructor, set agent id.
	 *	\param agentId Agent id value.
	 */
	public PacketConfigRequest(short agentId) {
		this.agentId = agentId;
	}

	@Override
	public byte[] toByteArray() {
		ByteBuffer bArray = ByteBuffer.allocate(2);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putShort(agentId);
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		agentId = bArray.getShort();
	}
	/**
	 *	\fn public short getAgentId()
	 *	\brief Get agent id.
	 *	\return Agent id.
	 */
	public short getAgentId() {
		return agentId;
	}

	@Override
	public byte getType() {
		return type;
	}

}
