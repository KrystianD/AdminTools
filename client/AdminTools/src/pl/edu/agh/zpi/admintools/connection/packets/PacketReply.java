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
 *	\class PacketReply
 *	\brief Packet with request reply value.
 */
public class PacketReply implements IPacket {
	//! No authentication flag value.
	public static final int NO_AUTH = 0;
	private final byte type = Header.PACKET_REPLY;

	private int value;
	/**
	 *	\fn public PacketReply()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketReply() {
	}
	/**
	 *	\fn public int getValue()
	 *	\brief Get reply value.
	 *	\return Reply value.
	 */
	public int getValue(){
		return value;
	}

	@Override
	public byte[] toByteArray(){
		ByteBuffer bArray = ByteBuffer.allocate(4);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putInt(value);
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array){
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		this.value = bArray.getInt();
	}

	@Override
	public byte getType() {
		return type;
	}

}
