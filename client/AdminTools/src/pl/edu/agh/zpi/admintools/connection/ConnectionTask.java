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
	
	enum State {
		IDLE, CONNECTING, DISCONNECTING, ACTIVE
	}

	private Messenger activityMessenger;
	private boolean endConnection = false;
	private long lastPing = System.currentTimeMillis();
	private Socket socket;
	private InputStream input;
	private OutputStream output;
	private ConcurrentLinkedQueue<IPacket> packetQueue = new ConcurrentLinkedQueue<IPacket>();
	private State state = State.IDLE;
	private String host;
	private int port;

	@Override
	public void run() {
		while (!endConnection) {
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
			case ACTIVE:
				try {
					processActive();
				} catch (Exception e) {
					e.printStackTrace();
					state = State.CONNECTING;
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

			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		Log.d("qwe", "thread dead");
	}

	public void processIdle() {
		try {
			Thread.sleep(10);
		} catch (InterruptedException e) {
			Log.e("qwe", e.getMessage());
		}
	}

	public void processConnecting() throws Exception {
		InetAddress address = InetAddress.getByName(host);
		socket = new Socket(address, port);

		input = socket.getInputStream();
		output = socket.getOutputStream();

		processAuthKey();
		sendPacket(new PacketStart((short) 1000));

		callback(CONNECTED, null);
		state = State.ACTIVE;
	}

	public void processDisconnecting() throws Exception {
		Log.d("qwe", "disconnecting");
		sendHeader(Header.PACKET_STOP);
		output.close();
		input.close();
		socket.close();
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
				callback(AGENTS_DATA,agentsData);
				break;
			case Header.PACKET_KEY_REPLY:
				PacketKeyReply keyReply = new PacketKeyReply();
				readPacket(keyReply, header.getSize());
				callback(AGENT_KEY,keyReply);
				break;
			case Header.PACKET_CONFIG:
				//nie powinien tego dostać!
				Log.d("qwe", "packet config. why leo?");
				readPacket(null,header.getSize());
				break;
			default:
				Log.e("qwe", "unknown header " + header.getType());
			}

		}
		packet = packetQueue.poll();
		if (packet != null) {
			sendPacket(packet);
		}
		if (System.currentTimeMillis() - lastPing > 1000) {
			sendHeader(Header.PACKET_PING);
			lastPing = System.currentTimeMillis();
		}
	}

	private void processAuthKey() throws Exception {
		String s = "PUAEODBIAGSYILOX";
		PacketAuthKey authKey = new PacketAuthKey(s.getBytes(),false);
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
			PacketReply r = new PacketReply();
			r.fromByteArray(data);
			//Log.d("qwe", "" + r.getValue());
		} else {
			throw new Exception("authKey");
		}
	}

	private Header readHeader() throws IOException {
		byte[] data = new byte[Header.HEADER_SIZE];
		input.read(data);
		return Header.fromByteArray(data);
	}

	/*
	 * private void sendHeader(byte type, short size) throws IOException {
	 * Header header = new Header(type, size);
	 * output.write(header.toByteArray()); output.flush(); }
	 */

	private void sendHeader(byte type) throws IOException {
		Header header = new Header(type, (byte) 0);
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
		if(packet != null)
			packet.fromByteArray(data);
	}

	private void sendPacket(IPacket packet) throws IOException {
		byte[] data = packet.toByteArray();
		Header header = new Header(packet.getType(), (short) data.length);
		output.write(header.toByteArray());
		output.write(data);
		output.flush();
	}
	
	private void callback(int type, Serializable data){
		if(activityMessenger != null){
			Bundle b = new Bundle();
			Message m = Message.obtain(null, type);
			switch(type){
			case AGENTS_DATA:
				b.putSerializable(PacketAgentsData.PACKET_AGENTS_DATA, data);
				m.setData(b);
				break;
			case CONNECTED:
				// nothing to send
				break;
			case AGENT_KEY:
				b.putSerializable(PacketKeyReply.PACKET_KEY_REPLY, data);
				m.setData(b);
				break;
			default:
				return;
			}
			
			try {
				activityMessenger.send(m);
			} catch (RemoteException e) {
				Log.d("qwe", "connectiontask callback exception");
				activityMessenger = null ; // ??
				e.printStackTrace();
			}
		}
	}

	public synchronized void connect(String host,int port) {
			this.host = host;
			this.port = port;
			state = State.CONNECTING;
	}

	public void disconnect() {
		synchronized (state) {
			state = State.DISCONNECTING;
		}
	}

	public void setMessenger(Messenger activityMessenger) {
		synchronized (activityMessenger) {
			this.activityMessenger = activityMessenger;
		}
	}

	public void enqueueMessage(IPacket packet) {
		try {
			packetQueue.add(packet);
		} catch (Exception e) {
		}
	}
}