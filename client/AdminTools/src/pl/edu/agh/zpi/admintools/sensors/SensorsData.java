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
import java.util.ArrayList;

/**
 *	\class SensorsData
 *	\brief Sensors data structure as serializable POJO.
 */
public class SensorsData implements Serializable {
	//! Serialization id.
	private static final long serialVersionUID = 1L;
	//! Measurement time.
	private int timestamp;
	//! Current CPU temperature.
	private float temp;
	//! CPU temperature valid flag.
	private boolean tempValid;
	//! Percent CPU usage.
	private float cpuUsage;
	//! Total available RAM.
	private long totalRam;
	//! Currently free RAM.
	private long freeRam;
	//! System uptime.
	private int uptime;
	//! List of discs usage.
	private ArrayList<DiskUsageData> diskUsages = new ArrayList<DiskUsageData>();
	//! List of services data.
	private ArrayList<ServiceData> services = new ArrayList<ServiceData>();

	private SensorsData() {
	}

	/**
	 *	\fn public static SensorsData fromByteBuffer(ByteBuffer buffer)
	 *	\brief Fill sensors data with given byte buffer.
	 *	\param buffer Byte buffer with sensors data.
	 *	\return Converted ServiceData object.
	 */
	public static SensorsData fromByteBuffer(ByteBuffer buffer) {
		SensorsData s = new SensorsData();
		s.timestamp = buffer.getInt();
		s.temp = buffer.getFloat();
		s.tempValid = (buffer.get() != 0);
		s.cpuUsage = buffer.getFloat();
		s.totalRam = buffer.getLong();
		s.freeRam = buffer.getLong();
		s.uptime = buffer.getInt();
		for (short i = buffer.getShort(); i > 0; i--) {
			s.diskUsages.add(DiskUsageData.fromByteBuffer(buffer));
		}
		for (short i = buffer.getShort(); i > 0; i--) {
			s.services.add(ServiceData.fromByteBuffer(buffer));
		}
		return s;
	}

	/**
	 *	\fn public float getTemp()
	 *	\brief Get current CPU temperature.
	 *	\return CPU temperature.
	 */
	public float getTemp() {
		return temp;
	}

	/**
	 *	\fn public boolean isTempValid()
	 *	\brief Check if CPU temperature measurement is valid.
	 *	\return Is CPU temperature valid.
	 */
	public boolean isTempValid() {
		return tempValid;
	}

	/**
	 *	\fn public float getCpuUsage()
	 *	\brief Get currently CPU usage.
	 *	\return Percent CPU usage.
	 */
	public float getCpuUsage() {
		return cpuUsage;
	}

	/**
	 *	\fn public long getTotalRam()
	 *	\brief Get total size of RAM memory.
	 *	\return Total RAM.
	 */
	public long getTotalRam() {
		return totalRam;
	}

	/**
	 *	\fn public long getFreeRam()
	 *	\brief Get size of currently free RAM memory.
	 *	\return Free RAM.
	 */
	public long getFreeRam() {
		return freeRam;
	}

	/**
	 *	\fn public int getUptime()
	 *	\brief Get system uptime from last restart.
	 *	\return System uptime.
	 */
	public int getUptime() {
		return uptime;
	}

	/**
	 *	\fn public ArrayList<DiskUsageData> getDiskUsages()
	 *	\brief Get all discs usage.
	 *	\return List of discs usage.
	 */
	public ArrayList<DiskUsageData> getDiskUsages() {
		return diskUsages;
	}

	/**
	 *	\fn public ArrayList<ServiceData> getServices()
	 *	\brief Get services basic data.
	 *	\return List of services data.
	 */
	public ArrayList<ServiceData> getServices() {
		return services;
	}

	/**
	 *	\fn public String toString()
	 *	\brief Convert SensorsData to string representation.
	 *	\return String representation.
	 */
	@Override
	public String toString() {
		return "SensorsData [timestamp=" + timestamp + ", temp=" + temp + ", tempValid=" + tempValid
				+ ", cpuUsage=" + cpuUsage + ", totalRam=" + totalRam
				+ ", freeRam=" + freeRam + ", uptime=" + uptime
				+ ", diskUsages=" + diskUsages + ", services=" + services + "]";
	}
}