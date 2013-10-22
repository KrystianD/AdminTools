package pl.edu.agh.zpi.admintools.sensors;

import java.io.Serializable;
import java.nio.ByteBuffer;

public class ServiceData implements Serializable{
	private static final long serialVersionUID = 1L;
	private String name = "";
	private boolean available;

	private ServiceData() {
	}

	public static ServiceData fromByteBuffer(ByteBuffer buffer) {
		ServiceData serviceData = new ServiceData();
		for (short i = buffer.getShort(); i > 0; i--) {
			char c = (char) buffer.get();
			serviceData.name += c;
		}
		serviceData.available = (buffer.get() != 0);
		return serviceData;
	}

	public String getName() {
		return name;
	}

	public boolean isAvailable() {
		return available;
	}

	@Override
	public String toString() {
		return "ServiceData [name=" + name + ", available=" + available + "]";
	}
}
