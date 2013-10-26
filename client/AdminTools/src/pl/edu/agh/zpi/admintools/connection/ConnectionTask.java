package pl.edu.agh.zpi.admintools.connection;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.net.InetAddress;
import java.net.Socket;
import java.util.concurrent.ConcurrentLinkedQueue;

import pl.edu.agh.zpi.admintools.connection.packets.IPacket;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAgentsData;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAuthKey;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfig;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStart;
import android.os.Bundle;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;

public class ConnectionTask implements Runnable {
	public static final int CONNECTED = 0;
	public static final int AGENTS_DATA = 1;
	public static final int AGENT_KEY = 2;
	public static final int AGENT_CONFIG = 3;

	enum State {
		IDLE, CONNECTING, DISCONNECTING, ACTIVE, STOPPING, STARTING
	}

	private Messenger activityMessenger;
	private boolean endConnection = false;
	private long lastPing = System.currentTimeMillis();
	private Socket socket;
	private InputStream input;
	private OutputStream output;
	private ConcurrentLinkedQueue<IPacket> packetQueue = new ConcurrentLinkedQueue<IPacket>();
	private State state = State.IDLE;
	private boolean isConnected = false;
	private String host;
	private int port;

	@Override
	public void run() {
		while (!endConnection) {
			// Log.d("qwe", "ConnectionTask.run() loop " + state);
			switch (state) {
			case IDLE:
				processIdle();
				break;
			case CONNECTING:
				try {
					processConnecting();
				} catch (Exception e) {
					e.printStackTrace();
					state = State.IDLE;
				}
				break;
			case STARTING:
				try {
					processStarting();
				} catch (Exception e) {
					e.printStackTrace();
					state = State.IDLE;
				}
				break;
			case ACTIVE:
				try {
					processActive();
				} catch (Exception e) {
					e.printStackTrace();
					state = State.CONNECTING;
				}
				break;
			case STOPPING:
				try {
					processStopping();
				} catch (Exception e) {
					e.printStackTrace();
					state = State.IDLE;
				}
				break;
			case DISCONNECTING:
				try {
					processDisconnecting();
				} catch (Exception e) {
					e.printStackTrace();
					endConnection = true;
				}
				break;
			default:
				Log.e("qwe", "unknown state");
				break;
			}
			sendPing();
			try {
				Thread.sleep(16);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		Log.d("qwe", "thread dead");
	}

	private void processIdle() {
		try {
			Thread.sleep(10);
		} catch (InterruptedException e) {
			Log.e("qwe", e.getMessage());
		}
	}

	private void processStopping() throws Exception {
		sendHeader(Header.PACKET_STOP);
		state = State.IDLE;
	}

	private void processStarting() throws Exception {
		sendPacket(new PacketStart((short) 1000));
		state = State.ACTIVE;
	}

	private void processConnecting() throws Exception {
		InetAddress address = InetAddress.getByName(host);
		socket = new Socket(address, port);

		input = socket.getInputStream();
		output = socket.getOutputStream();

		processAuthKey();

		callback(CONNECTED, null);

		state = State.STARTING;
	}

	private void processDisconnecting() throws Exception {
		Log.d("qwe", "disconnecting");
		output.close();
		input.close();
		socket.close();
		isConnected = false;
		endConnection = true;
	}

	private void processActive() throws Exception {
		IPacket packet = null;
		Header header;

		if (input.available() >= 3) {
			header = readHeader();
			switch (header.getType()) {
			case Header.PACKET_AGENTS_DATA:
				PacketAgentsData agentsData = new PacketAgentsData();
				readPacket(agentsData, header.getSize());
				callback(AGENTS_DATA, agentsData);
				break;
			case Header.PACKET_KEY_REPLY:
				PacketKeyReply keyReply = new PacketKeyReply();
				readPacket(keyReply, header.getSize());
				callback(AGENT_KEY, keyReply);
				break;
			case Header.PACKET_CONFIG:
				PacketConfig pc = new PacketConfig();
				readPacket(pc, header.getSize());
				callback(AGENT_CONFIG, pc);
				break;
			default:
				Log.e("qwe", "unknown header " + header.getType());
			}

		}
		packet = packetQueue.poll();
		if (packet != null) {
			sendPacket(packet);
		}
	}

	private void sendPing() {
		// Log.d("qwe", "ConnectionTask.sendPing()" + isConnected);
		if (isConnected && System.currentTimeMillis() - lastPing > 1000) {
			try {
				sendHeader(Header.PACKET_PING);
			} catch (IOException e) {
				e.printStackTrace();
			}
			lastPing = System.currentTimeMillis();
		}
	}

	private void processAuthKey() throws Exception {
		String s = "PUAEODBIAGSYILOX";
		PacketAuthKey authKey = new PacketAuthKey(s.getBytes(), false);
		sendPacket(authKey);
		long start = System.currentTimeMillis();
		while (input.available() < Header.HEADER_SIZE) {
			if (System.currentTimeMillis() - start > 1000) {
				throw new Exception("headerTimeout");
			}
			Thread.sleep(10);
		}
		Header header = readHeader();
		byte[] data = new byte[header.getSize()];
		input.read(data);
		Log.d("qwe", "" + header.getType());
		if (header.getType() == Header.PACKET_REPLY) {
			isConnected = true;
			PacketReply r = new PacketReply();
			r.fromByteArray(data);
			// Log.d("qwe", "" + r.getValue());
		} else {
			isConnected = false;
			throw new Exception("authKey");
		}
	}

	private Header readHeader() throws IOException {
		byte[] data = new byte[Header.HEADER_SIZE];
		input.read(data);
		return Header.fromByteArray(data);
	}

	private void sendHeader(byte type) throws IOException {
		Header header = new Header(type, (byte) 0);
		Log.d("qwe", "header type " + type);
		Log.d("qwe", "output null" + (output == null));
		output.write(header.toByteArray());
		output.flush();
	}

	private void readPacket(IPacket packet, short size) throws Exception {
		byte[] data = new byte[size];
		long start = System.currentTimeMillis();
		while (input.available() < size) {
			if (System.currentTimeMillis() - start > 1000) {
				throw new Exception("readPacket");
			}
			Thread.sleep(10);
		}
		input.read(data);
		if (packet != null)
			packet.fromByteArray(data);
	}

	private void sendPacket(IPacket packet) throws IOException {
		byte[] data = packet.toByteArray();
		Header header = new Header(packet.getType(), (short) data.length);
		output.write(header.toByteArray());
		output.write(data);
		output.flush();
	}

	private void callback(int type, Serializable data) {
		Log.d("qwe", "ConnectionTask.callback()");
		if (activityMessenger != null) {
			Bundle b = new Bundle();
			Message m = Message.obtain(null, type);
			switch (type) {
			case AGENTS_DATA:
				b.putSerializable(PacketAgentsData.PACKET_AGENTS_DATA, data);
				break;
			case CONNECTED:
				// nothing to send
				break;
			case AGENT_KEY:
				b.putSerializable(PacketKeyReply.PACKET_KEY_REPLY, data);
			case AGENT_CONFIG:
				b.putSerializable(PacketConfig.PACKET_CONFIG, data);
				break;
			default:
				return;
			}
			m.setData(b);
			try {
				activityMessenger.send(m);
			} catch (RemoteException e) {
				Log.d("qwe", "connectiontask callback exception");
				activityMessenger = null; // ??
				e.printStackTrace();
			}
		}
	}

	public synchronized void connect(String host, int port) {
		Log.d("qwe", "ConnectionTask.connect()" + state);
		if ((!isConnected() && (this.port != port || this.host != host))) {
			this.host = host;
			this.port = port;
			state = State.CONNECTING;
		} else {
			if (state == State.IDLE) {
				state = State.STARTING;
			}
			callback(CONNECTED, null);
		}
	}

	public void disconnect() {
		synchronized (state) {
			state = State.DISCONNECTING;
		}
	}

	public void stop() {
		synchronized (state) {
			state = State.STOPPING;
		}
	}

	public synchronized boolean isConnected() {
		return isConnected;
	}

	public void setMessenger(Messenger activityMessenger) {
		synchronized (activityMessenger) {
			this.activityMessenger = activityMessenger;
		}
	}

	public void enqueueMessage(IPacket packet) {
		try {
			packetQueue.add(packet);
			Log.d("qwe", "ConnectionTask.enqueueMessage() " + packet.getType());
		} catch (Exception e) {
			Log.e("qwe", "ConnectionTask.enqueueMessage()");
		}
	}
}