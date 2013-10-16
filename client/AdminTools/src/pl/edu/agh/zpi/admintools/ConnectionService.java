package pl.edu.agh.zpi.admintools;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;

import pl.edu.agh.zpi.admintools.connection.AgentsData;
import pl.edu.agh.zpi.admintools.connection.AuthKey;
import pl.edu.agh.zpi.admintools.connection.Header;
import pl.edu.agh.zpi.admintools.connection.IPacket;
import pl.edu.agh.zpi.admintools.connection.KeyReply;
import pl.edu.agh.zpi.admintools.connection.Reply;
import android.app.IntentService;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;

public class ConnectionService extends IntentService {
	public static final int GENERATE_KEY = 0;
	
	public static final String NAME = "pl.edu.agh.zpi.admintools.ConnectionService";
	private boolean connected = false;
	private boolean endConnection = false;
	private boolean configurationChanged = false;
	private long lastPing;
	private Socket socket;
	private InputStream input;
	private OutputStream output;

	private Messenger messenger = new Messenger(new IncomingHandler(this));
	
	public ConnectionService() {
		super(NAME);
	}

	@Override
	protected void onHandleIntent(Intent intent) {
		if (connected) {
			return;
		}

		String host = intent.getStringExtra(AdminTools.HOST);
		int port = intent.getIntExtra(AdminTools.PORT, -1);

		try {
			Header header;
			InetAddress address = InetAddress.getByName(host);
			socket = new Socket(address, port);

			connected = true;
			input = socket.getInputStream();
			output = socket.getOutputStream();

			processAuthKey();
			sendHeader(Header.PACKET_START);
			lastPing = System.currentTimeMillis();
			while (!endConnection && socket.isConnected()) {
				if (input.available() >= 3) {
					header = readHeader();

					switch (header.getType()) {
					case Header.PACKET_AGENTS_DATA:
						AgentsData agentsData = new AgentsData();
						readPacket(agentsData, header.getSize());
						break;
					case Header.PACKET_KEY_REPLY:
						KeyReply keyReply = new KeyReply();
						readPacket(keyReply, header.getSize());
						break;
					default:
						Log.e("qwe", "unknown header " + header.getType());
					}

				}

				if (configurationChanged) {
					// sendNewConfig();
				}
				if (System.currentTimeMillis() - lastPing > 1000) {
					sendHeader(Header.PACKET_PING);
					lastPing = System.currentTimeMillis();
				}
				Thread.sleep(10);
			}
			sendHeader(Header.PACKET_STOP);
			output.close();
			input.close();
			socket.close();
			connected = false;
		} catch (Exception e) {
			connected = false;
			Log.e("qwe", e.getLocalizedMessage());
			e.printStackTrace();
		}
	}

	public boolean isConnected() {
		return connected;
	}

	public synchronized void endConnection() {
		endConnection = true;
	}

	private void processAuthKey() throws Exception {
		AuthKey authKey = new AuthKey(new byte[] { 1, 2, 3, 4, 5, 6, 7, 8, 9,
				10, 11, 12, 13, 14, 15, 16 });
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
		Log.d("qwe", ""+header.getType());
		if (header.getType() == Header.PACKET_REPLY) {
			Reply r = new Reply();
			r.fromByteArray(data);
			Log.d("qwe", "" + r.getValue());
		} else {
			throw new Exception("authKey");
		}
	}

	private Header readHeader() throws IOException {
		byte[] data = new byte[Header.HEADER_SIZE];
		input.read(data);
		return Header.fromByteArray(data);
	}

	/*private void sendHeader(byte type, short size) throws IOException {
		Header header = new Header(type, size);
		output.write(header.toByteArray());
		output.flush();
	}*/

	private void sendHeader(byte type) throws IOException {
		Header header = new Header(type, (byte)0);
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
		packet.fromByteArray(data);
	}

	private void sendPacket(IPacket packet) throws IOException {
		byte[] data = packet.toByteArray();
		Header header = new Header(packet.getType(), (short)data.length);
		output.write(header.toByteArray());
		output.write(data);
		output.flush();
	}
	
	@Override
	public IBinder onBind(Intent intent){
		return messenger.getBinder();
	}
	
	public void handleMessage(Message msg) throws IOException{
		switch(msg.what){
		case GENERATE_KEY:
			sendHeader(Header.PACKET_KEY_REQUEST);
			break;
		default:
		}
	}
	
}

class IncomingHandler extends Handler{
	private ConnectionService service;
	public IncomingHandler(ConnectionService service){
		this.service = service;
	}
	
	@Override
	public void handleMessage(Message msg){
		super.handleMessage(msg);
		try {
			service.handleMessage(msg);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}