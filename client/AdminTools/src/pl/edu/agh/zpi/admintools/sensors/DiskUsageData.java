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
