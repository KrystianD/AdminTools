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

import pl.edu.agh.zpi.admintools.connection.Header;
import android.util.Log;

/**
 *	\class PacketKeyReply
 *	\brief Packet with authentication key.
 */
public class PacketKeyReply implements IPacket,Serializable {
	private static final long serialVersionUID = 1L;
	private static final byte type = Header.PACKET_KEY_REPLY;
	private static final int SIZE = 16;
	//! Packet full class type.
	public static final String PACKET_KEY_REPLY = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_KEY_REPLY";

	private byte[] key = new byte[SIZE];
	/**
	 *	\fn public PacketKeyReply()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketKeyReply() {
	}

	@Override
	public byte[] toByteArray() {
		return key;
	}

	@Override
	public void fromByteArray(byte[] array) {
		key = array;
		Log.d("qwe", this.toString());
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
		String s = "";
		for(int i = 0 ;  i < SIZE ; i++){
			s += (char)key[i];
		}
		return s;
	}
}
