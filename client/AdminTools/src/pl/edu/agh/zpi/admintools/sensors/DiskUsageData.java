package pl.edu.agh.zpi.admintools.sensors;

import java.nio.ByteBuffer;

public class DiskUsageData {
	private String name = "";
	private long totalSpace;
	private long usedSpace;

	private DiskUsageData() {
	}

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

	@Override
	public String toString() {
		return "DiskUsageData [name=" + name + ", totalSpace=" + totalSpace
				+ ", usedSpace=" + usedSpace + "]";
	}
}
