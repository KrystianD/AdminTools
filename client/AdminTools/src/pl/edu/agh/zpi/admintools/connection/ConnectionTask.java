package pl.edu.agh.zpi.admintools.connection;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.concurrent.ConcurrentLinkedQueue;

import pl.edu.agh.zpi.admintools.AdminTools;
import pl.edu.agh.zpi.admintools.connection.packets.IPacket;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAgentsData;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAuthKey;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfig;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStart;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsReply;
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
	public static final int CONNECTION_ERROR = 4;
	public static final int STATS_REPLY = 5;
	
	enum State {
		IDLE, CONNECTING, DISCONNECTING, ACTIVE, STOPPING, STARTING
	}

	private Messenger activityMessenger;
	private boolean endTask = false;
	private long lastPing = System.currentTimeMillis();
	private Socket socket;
	private InputStream input;
	private OutputStream output;
	private ConcurrentLinkedQueue<IPacket> packetQueue = new ConcurrentLinkedQueue<IPacket>();
	private State state = State.IDLE;
	private boolean isConnected = false;
	private String host;
	private int port;
	private String key;
	private short interval;
	
	@Override
	public void run() {
		while (!endTask) {
			// Log.d("qwe", "ConnectionTask.run() loop " + state);
			switch (state) {
			case IDLE:
				processIdle();
				break;
			case CONNECTING:
				try {
					processConnecting();
				} catch (Exception e) {
					processNetworkError(e);
					state = State.IDLE;
				}
				break;
			case STARTING:
				try {
					processStarting();
				} catch (Exception e) {
					processNetworkError(e);
				}
				break;
			case ACTIVE:
				try {
					processActive();
				} catch (Exception e) {
					processNetworkError(e);
				}
				break;
			case STOPPING:
				try {
					processStopping();
				} catch (Exception e) {
					processNetworkError(e);
				}
				break;
			case DISCONNECTING:
				try {
					processDisconnecting();
				} catch (Exception e) {
					e.printStackTrace();
					endTask = true;
				}
				break;
			default:
				Log.e("qwe", "unknown state");
				break;
			}
			if(isConnected){
				sendPing();
			}
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
		state = State.ACTIVE;
	}

	private void processStarting() throws Exception {
		sendPacket(new PacketStart((short) interval));
		state = State.ACTIVE;
	}

	private void processConnecting() throws Exception {
		Log.d("qwe", "ConnectionTask.processConnecting() " + host + " " + port);
		InetAddress address = InetAddress.getByName(host);
		InetSocketAddress socketAddress = new InetSocketAddress(address, port);
		socket = new Socket();
		socket.connect(socketAddress, 1500);

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
		endTask = true;
	}

	private void processActive() throws Exception {
		IPacket packet = null;
		Header header;

		if (input.available() >= 3) {
			header = readHeader();
			Log.d("qwe","ConnectionTask.processActive() " + header.getType());
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
			case Header.PACKET_CONFIG_REPLY:
				PacketConfig pc = new PacketConfig();
				readPacket(pc, header.getSize());
				callback(AGENT_CONFIG, pc);
				break;
			case Header.PACKET_CHANGE_REPLY:
				readPacket(new PacketReply(), header.getSize());
				//callback(AGENT_CONFIG, pc);
				break;
			case Header.PACKET_STATS_REPLY:
				Log.d("qwe","ConnectionTask.PACKET_STATS_REPLY");
				PacketStatsReply sr = new PacketStatsReply();
				readPacket(sr, header.getSize());
				callback(STATS_REPLY,sr);
				break;
			default:
				Log.e("qwe", "unknown header " + header.getType());
				readPacket(null, header.getSize());
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
			} catch (Exception e) {
				 processNetworkError(e);
			}
			lastPing = System.currentTimeMillis();
		}
	}

	private void processAuthKey() throws Exception {
		PacketAuthKey authKey = new PacketAuthKey(key.getBytes(), false);
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

	private Header readHeader() throws Exception {
		byte[] data = new byte[Header.HEADER_SIZE];
		input.read(data);
		return Header.fromByteArray(data);
	}

	private void sendHeader(byte type) throws Exception {
		Header header = new Header(type, (byte) 0);
		//Log.d("qwe", "header type " + type);
		//Log.d("qwe", "output null" + (output == null));
		output.write(header.toByteArray());
		output.flush();
	}

	private void readPacket(IPacket packet, short size) throws Exception {
		Log.d("qwe", "ConnectionTask.readPacket() size = " + size);
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

	private void sendPacket(IPacket packet) throws Exception {
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
			case CONNECTION_ERROR:
				b.putSerializable(AdminTools.NETWORK_ERROR, data);
				break;
			case STATS_REPLY:
				b.putSerializable(PacketStatsReply.PACKET_STATS_REPLY, data);
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

	public synchronized void connect(String host, int port, String key, short interval) {
		Log.d("qwe", "ConnectionTask.connect()" + state);
		Log.d("qwe", "ConnectionTask.connect() " + host + " " + port + " " + key + " " + interval);
		if ((!isConnected || this.port != port || this.host != host || !this.key.equals(key) || this.interval != interval)) {
			this.host = host;
			this.port = port;
			this.key = key;
			this.interval = interval;
			state = State.CONNECTING;
		} else {
			state = State.STARTING;
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
	
	private void processNetworkError(Exception e){
		e.printStackTrace();
		state = State.IDLE;
		host = "";
		port = 0;
		isConnected = false;
		callback(CONNECTION_ERROR,e);
	}
}