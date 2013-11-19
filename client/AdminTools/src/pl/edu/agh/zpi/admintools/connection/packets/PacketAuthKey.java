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
 *	\class PacketAuthKey
 *	\brief Authentication packet.
 */
public class PacketAuthKey implements IPacket {
	private final byte type = Header.PACKET_AUTH;

	private byte[] key = new byte[16];
	private boolean sendConfig = false;

	/**
	 *	\fn public PacketAuthKey()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketAuthKey() {
	}

	/**
	 *	\fn public PacketAuthKey(byte[] key, boolean sendConfig)
	 *	\brief Constructor, sets key and config flag.
	 *	\param key Authentication key.
	 *	\param sendConfig Send config flag.
	 */
	public PacketAuthKey(byte[] key, boolean sendConfig) {
		if(key.length == 16){
			this.key = key;
		}
		this.sendConfig = sendConfig;
	}
	/**
	 *	\fn public byte[] getKey()
	 *	\brief Get current authentication key.
	 *	\return Auth key.
	 */
	public byte[] getKey() {
		return key;
	}
	/**
	 *	\fn public void setKey(byte[] key)
	 *	\brief Set current authentication key.
	 *	\param key Auth key.
	 *	\return None.
	 */
	public void setKey(byte[] key) {
		if(key.length == 16)
			this.key = key;
	}

	public byte[] toByteArray(){
		ByteBuffer buffer = ByteBuffer.allocate(17);
		buffer.put(key);
		buffer.put(sendConfig == true ? (byte)1 : (byte)0);
		return buffer.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
		ByteBuffer buffer = ByteBuffer.wrap(array);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		buffer.get(key);
		sendConfig = (buffer.get() == 1);
	}

	@Override
	public byte getType() {
		return type;
	}
}
