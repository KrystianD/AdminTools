package pl.edu.agh.zpi.admintools.sensors;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class SensorsData implements Serializable {
	private static final long serialVersionUID = 1L;
	private int timestamp;
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

	public float getTemp() {
		return temp;
	}

	public float getCpuUsage() {
		return cpuUsage;
	}

	public long getTotalRam() {
		return totalRam;
	}

	public long getFreeRam() {
		return freeRam;
	}

	public int getUptime() {
		return uptime;
	}

	public ArrayList<DiskUsageData> getDiskUsages() {
		return diskUsages;
	}

	public ArrayList<ServiceData> getServices() {
		return services;
	}

	@Override
	public String toString() {
		return "SensorsData [timestamp=" + timestamp + ", temp=" + temp + ", tempValid=" + tempValid
				+ ", cpuUsage=" + cpuUsage + ", totalRam=" + totalRam
				+ ", freeRam=" + freeRam + ", uptime=" + uptime
				+ ", diskUsages=" + diskUsages + ", services=" + services + "]";
	}
}