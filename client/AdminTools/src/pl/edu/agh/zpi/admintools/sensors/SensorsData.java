package pl.edu.agh.zpi.admintools.sensors;

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class SensorsData {
	private float temp;
	private boolean tempValid;
	private float cpuUsage;
	private long totalRam;
	private long freeRam;
	private int uptime;
	private ArrayList<DiskUsageData> diskUsages = new ArrayList<DiskUsageData>();
	private ArrayList<ServiceData> services = new ArrayList<ServiceData>();

	private SensorsData() {
	}

	public static SensorsData fromByteBuffer(ByteBuffer buffer) {
		SensorsData s = new SensorsData();
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

	@Override
	public String toString() {
		return "SensorsData [temp=" + temp + ", tempValid=" + tempValid
				+ ", cpuUsage=" + cpuUsage + ", totalRam=" + totalRam
				+ ", freeRam=" + freeRam + ", uptime=" + uptime
				+ ", diskUsages=" + diskUsages + ", services=" + services + "]";
	}
}