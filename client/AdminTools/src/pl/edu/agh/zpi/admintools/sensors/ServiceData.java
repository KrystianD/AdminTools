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
