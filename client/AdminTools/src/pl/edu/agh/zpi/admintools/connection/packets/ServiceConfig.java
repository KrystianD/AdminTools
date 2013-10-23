package pl.edu.agh.zpi.admintools.connection.packets;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ServiceConfig implements Serializable{
	private static final long serialVersionUID = 1L;
	private String name = "";
	private boolean isTCP;
	private short port;
	
	private ServiceConfig() {
	}

	public ServiceConfig(String name, boolean isTCP, short port) {
		this.name = name;
		this.isTCP = isTCP;
		this.port = port;
	}

	public byte[] toByteArray() {
		byte[] name = this.name.getBytes();
		
		ByteBuffer bArray = ByteBuffer.allocate(2+name.length+1+2);
		bArray.order(ByteOrder.LITTLE_ENDIAN);
		
		bArray.putShort((short)name.length);
		bArray.put(name);
		bArray.put(isTCP == true ? (byte)1 : (byte)0);
		bArray.putShort(port);
		
		return bArray.array();
	}

	public static ServiceConfig fromByteBuffer(ByteBuffer bArray) {
		ServiceConfig sc = new ServiceConfig();

		for (short i = bArray.getShort(); i > 0; i--) {
			char c = (char) bArray.get();
			sc.name += c;
		}
		sc.isTCP = bArray.get() != 0;
		sc.port = bArray.getShort();
		
		return sc;
	}
	
	public String getName() {
		return name;
	}

	public boolean isTCP() {
		return isTCP;
	}

	public short getPort() {
		return port;
	}

	public short getSize(){
		return (short)(2+name.getBytes().length+1+2);
	}

	@Override
	public String toString() {
		return "ServiceConfig [name=" + name + ", isTCP=" + isTCP + ", port="
				+ port + "]";
	}
	
}
