package pl.edu.agh.zpi.admintools;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyRequest;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;

public class ConnectionService extends Service implements Handable {
	public static final String MESSENGER = "pl.edu.agh.zpi.admintools.ConnectionService.MESSENGER";
	public static final int GET_MESSENGER = 0;
	public static final int GENERATE_KEY = 1;
	public static final int CONNECT = 2;
	public static final int SEND_SETTINGS = 3;

	public static final String NAME = "pl.edu.agh.zpi.admintools.ConnectionService";

	private Messenger serviceMessenger = new Messenger(
			new IncomingHandler(this));
	private Messenger activityMessenger;

	private ConnectionTask connectionTask = new ConnectionTask();

	public ConnectionService() {
		super();
	}

	@Override
	public void onCreate() {
		Thread conn = new Thread(connectionTask);
		conn.start();
		
		super.onCreate();
	}

	@Override
	public IBinder onBind(Intent intent) {
		Log.d("qwe", "onbind");
		return serviceMessenger.getBinder();
	}

	public void handleMessage(Message msg) {
		switch (msg.what) {
		case GET_MESSENGER:
			activityMessenger = msg.replyTo;
			connectionTask.setMessenger(activityMessenger);
			break;
		case GENERATE_KEY:
			connectionTask.enqueueMessage(new PacketKeyRequest());
			break;
		case CONNECT:
			connectionTask.connect();
			break;
		case SEND_SETTINGS:
			// connectionTask.enqueueMessage(new PacketSettings());
			break;
		default:
			break;
		}
	}

	@Override
	public boolean onUnbind(Intent intent) {
		Log.d("qwe", "service onUnbind");
		connectionTask.disconnect();
		stopSelf();
		return super.onUnbind(intent);
	}

	@Override
	public void onDestroy() {
		Log.d("qwe", "service onDestroy");
		super.onDestroy();
	}

}