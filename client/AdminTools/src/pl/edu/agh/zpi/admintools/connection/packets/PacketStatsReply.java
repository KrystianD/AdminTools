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

import android.util.Log;

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketStatsReply
 *	\brief Reply for stats request.
 */
public class PacketStatsReply implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	//! Packet full class type.
	public static final String PACKET_STATS_REPLY = "pl.edu.agh.zpi.admintools.connection.packets.PacketStatsReply.PACKET_STATS_REPLY";
	private final byte type = Header.PACKET_STATS_REPLY;
	//! CPU data flag.
	public static int CPU = 0;
	//! RAM memory data flag.
	public static int RAM = 1;
	//! TEMPerature data flag.
	public static int TEMP = 2;
	//! Disc data flag.
	public static int DISK = 3;

	private ArrayList<Float> points = new ArrayList<Float>();
	/**
	 *	\fn public PacketStatsReply()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketStatsReply() {
	}

	public void fromByteArray(byte[] array) {
		ByteBuffer bArray = ByteBuffer.wrap(array);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		short i = bArray.getShort();
		Log.d("qwe", "PacketStatsReply size = " + i);
		for (; i > 0; i--) {
			points.add(bArray.getFloat());
		}
	}
	/**
	 *	\fn public ArrayList<Float> getPoints()
	 *	\brief Get list with given points.
	 *	\return List with points.
	 */
	public ArrayList<Float> getPoints() {
		return points;
	}

	public byte getType() {
		return type;
	}

	@Override
	public byte[] toByteArray() {
		return null;
	}
}
