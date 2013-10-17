package pl.edu.agh.zpi.admintools;

import java.io.IOException;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.PacketKeyRequest;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;

public class ConnectionService extends Service {
	public static final int GENERATE_KEY = 0;
	public static final int CONNECT = 1;

	public static final String NAME = "pl.edu.agh.zpi.admintools.ConnectionService";

	private Messenger messenger = new Messenger(new IncomingHandler(this));
	private ConnectionTask connectionTask = new ConnectionTask();

	public ConnectionService() {
		super();
	}

	@Override
	public void onCreate() {
		Log.d("qwe", "oncreate");

		Thread conn = new Thread(connectionTask);
		conn.start();

		super.onCreate();
	}

	@Override
	public IBinder onBind(Intent intent) {
		Log.d("qwe", "onbind");
		return messenger.getBinder();
	}

	public void handleMessage(Message msg) throws IOException {
		switch (msg.what) {
		case GENERATE_KEY:
			connectionTask.enqueueMessage(new PacketKeyRequest());
			break;
		case CONNECT:
			connectionTask.connect();
			break;
		default:
		}
	}

}

class IncomingHandler extends Handler {
	private ConnectionService service;

	public IncomingHandler(ConnectionService service) {
		this.service = service;
	}

	@Override
	public void handleMessage(Message msg) {
		super.handleMessage(msg);
		try {
			service.handleMessage(msg);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}