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
package pl.edu.agh.zpi.admintools.sensors;

import java.io.Serializable;
import java.nio.ByteBuffer;

/**
 *	\class DiskUsageData
 *	\brief Disc usage data structure as serializable POJO.
 */
public class DiskUsageData implements Serializable {
	//! Serialization id.
	private static final long serialVersionUID = 1L;
	//! Disc name.
	private String name = "";
	//! Total available disc space.
	private long totalSpace;
	//! Used space on disc.
	private long usedSpace;

	private DiskUsageData() {
	}

	/**
	 *	\fn public static DiskUsageData fromByteBuffer(ByteBuffer buffer)
	 *	\brief Fill disc usage data with given byte buffer.
	 *	\param buffer Byte buffer with disc usage.
	 *	\return Converted DiskUsageData object.
	 */
	public static DiskUsageData fromByteBuffer(ByteBuffer buffer) {
		DiskUsageData dud = new DiskUsageData();
		for (short i = buffer.getShort(); i > 0; i--) {
			char c = (char) buffer.get();
			dud.name += c;
		}
		dud.totalSpace = buffer.getLong();
		dud.usedSpace = buffer.getLong();
		return dud;
	}

	/**
	 *	\fn public String getName()
	 *	\brief Get disc name.
	 *	\return Disc name.
	 */
	public String getName() {
		return name;
	}

	/**
	 *	\fn public long getTotalSpace()
	 *	\brief Get total available space on disc.
	 *	\return Total space.
	 */
	public long getTotalSpace() {
		return totalSpace;
	}

	/**
	 *	\fn public long getUsedSpace()
	 *	\brief Get used space on disc.
	 *	\return Used space.
	 */
	public long getUsedSpace() {
		return usedSpace;
	}

	/**
	 *	\fn public String toString()
	 *	\brief Convert DiskUsageData to string representation.
	 *	\return String representation.
	 */
	@Override
	public String toString() {
		return "DiskUsageData [name=" + name + ", totalSpace=" + totalSpace
				+ ", usedSpace=" + usedSpace + "]";
	}
}
