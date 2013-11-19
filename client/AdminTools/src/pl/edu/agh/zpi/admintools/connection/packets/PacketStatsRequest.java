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

import pl.edu.agh.zpi.admintools.connection.Header;

/**
 *	\class PacketStatsRequest
 *	\brief Stats request from agent.
 */
public class PacketStatsRequest implements IPacket, Serializable {
	private static final long serialVersionUID = 1L;
	private final byte type = Header.PACKET_STATS_REQUEST;
	//! Packet full class type.
	public static final String PACKET_STATS_REQUEST = "pl.edu.agh.zpi.admintools.connection.packets.PACKET_CONFIG";
	//! CPU data flag.
	public static final byte CPU = 0;
	//! RAM memory data flag.
	public static final byte RAM = 1;
	//! TEMPerature data flag.
	public static final byte TEMP = 2;
	//! Disc data flag.
	public static final byte DISK = 3;

	private short agentId;
	private int startDate;
	private int endDate;
	private short points;
	private byte dataType;
	private String diskName;

	/**
	 *	\fn public PacketStatsRequest()
	 *	\brief Default constructor, do nothing.
	 */
	public PacketStatsRequest() {
	}

	@Override
	public byte[] toByteArray() {
		short stringSize = (short)diskName.getBytes().length;
		ByteBuffer bArray = ByteBuffer.allocate(2+4+4+2+1+2+stringSize);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		bArray.putShort(agentId);
		bArray.putInt(startDate);
		bArray.putInt(endDate);
		bArray.putShort(points);
		bArray.put(dataType);
		bArray.putShort(stringSize);
		bArray.put(diskName.getBytes());
		return bArray.array();
	}

	@Override
	public void fromByteArray(byte[] array) {
	}
	/**
	 *	\fn public void setAgentId(short agentId)
	 *	\brief Set agent with stats id.
	 *	\param agentId Agent id.
	 *	\return None.
	 */
	public void setAgentId(short agentId) {
		this.agentId = agentId;
	}
	/**
	 *	\fn public void setStartDate(int startDate)
	 *	\brief Set stats start date.
	 *	\param startDate Stats start date.
	 *	\return None.
	 */
	public void setStartDate(int startDate) {
		this.startDate = startDate;
	}
	/**
	 *	\fn public void setEndDate(int endDate)
	 *	\brief Set stats end date.
	 *	\param endDate Stats end date.
	 *	\return None.
	 */
	public void setEndDate(int endDate) {
		this.endDate = endDate;
	}
	/**
	 *	\fn public void setPoints(short points)
	 *	\brief Set stats points.
	 *	\param points Stats points.
	 *	\return None.
	 */
	public void setPoints(short points) {
		this.points = points;
	}
	/**
	 *	\fn public void setDataType(byte dataType)
	 *	\brief Set data type flag.
	 *	\param dataType Type flag.
	 *	\return None.
	 */
	public void setDataType(byte dataType) {
		this.dataType = dataType;
	}
	/**
	 *	\fn public void setDiskName(String diskName)
	 *	\brief Set disc name.
	 *	\param diskName Disc name.
	 *	\return None.
	 */
	public void setDiskName(String diskName) {
		this.diskName = diskName;
	}
	/**
	 *	\fn public short getAgentId()
	 *	\brief Get agent with stats id.
	 *	\return Agent id.
	 */
	public short getAgentId() {
		return agentId;
	}
	/**
	 *	\fn public int getStartDate()
	 *	\brief Get stats start date.
	 *	\return Start date.
	 */
	public int getStartDate() {
		return startDate;
	}
	/**
	 *	\fn public int getEndDate()
	 *	\brief Get stats end date.
	 *	\return End date.
	 */
	public int getEndDate() {
		return endDate;
	}
	/**
	 *	\fn public short getPoints()
	 *	\brief Get stats points.
	 *	\return Stats points.
	 */
	public short getPoints() {
		return points;
	}
	/**
	 *	\fn public byte getDataType()
	 *	\brief Get data type flag.
	 *	\return Type flag.
	 */
	public byte getDataType() {
		return dataType;
	}
	/**
	 *	\fn public String getDiskName()
	 *	\brief Get disc name.
	 *	\return Disc name.
	 */
	public String getDiskName() {
		return diskName;
	}

	@Override
	public byte getType() {
		return type;
	}

}
