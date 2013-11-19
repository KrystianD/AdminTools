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
 *	\class ServiceData
 *	\brief Service data structure as serializable POJO.
 */
public class ServiceData implements Serializable{
	//! Serialization id.
	private static final long serialVersionUID = 1L;
	//! Service name.
	private String name = "";
	//! Service availability.
	private boolean available;

	private ServiceData() {
	}

	/**
	 *	\fn public static ServiceData fromByteBuffer(ByteBuffer buffer)
	 *	\brief Fill service data with given byte buffer.
	 *	\param buffer Byte buffer with service.
	 *	\return Converted ServiceData object.
	 */
	public static ServiceData fromByteBuffer(ByteBuffer buffer) {
		ServiceData serviceData = new ServiceData();
		for (short i = buffer.getShort(); i > 0; i--) {
			char c = (char) buffer.get();
			serviceData.name += c;
		}
		serviceData.available = (buffer.get() != 0);
		return serviceData;
	}

	/**
	 *	\fn public String getName()
	 *	\brief Get service name.
	 *	\return Service name.
	 */
	public String getName() {
		return name;
	}

	/**
	 *	\fn public boolean isAvailable()
	 *	\brief Check service availability.
	 *	\return If available.
	 */
	public boolean isAvailable() {
		return available;
	}

	/**
	 *	\fn public String toString()
	 *	\brief Convert ServiceData to string representation.
	 *	\return String representation.
	 */
	@Override
	public String toString() {
		return "ServiceData [name=" + name + ", available=" + available + "]";
	}
}
