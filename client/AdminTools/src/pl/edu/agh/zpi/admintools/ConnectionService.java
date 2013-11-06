package pl.edu.agh.zpi.admintools;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfig;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfigRequest;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyRequest;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStart;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsRequest;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;

public class ConnectionService extends Service implements Handable {
	public static final String MESSENGER = "pl.edu.agh.zpi.admintools.ConnectionService.MESSENGER";
	public static final int GET_MESSENGER = 0;
	public static final int GENERATE_KEY = 1;
	public static final int CONNECT = 2;
	public static final int DISCONNECT = 3;
	public static final int SEND_CONFIG = 4;
	public static final int STOP = 5;
	public static final int REQUEST_CONFIG = 6;
	public static final int SET_INTERVAL = 7;
	public static final int STATS_REQUEST = 8;
	
	public static final String NAME = "pl.edu.agh.zpi.admintools.ConnectionService";
	
	private Messenger serviceMessenger = new Messenger(
			new IncomingHandler(this));
	private Messenger activityMessenger;

	private ConnectionTask connectionTask = new ConnectionTask();
	private Thread connectionThread;
	
	public ConnectionService() {
		super();
	}

	@Override
	public void onCreate() {
		connectionThread = new Thread(connectionTask);
		connectionThread.start();
		
		super.onCreate();
	}

	@Override
	public IBinder onBind(Intent intent) {
		Log.d("qwe", "ConnectionService.onBind()");

		return serviceMessenger.getBinder();
	}

	public void handleMessage(Message msg) {
		//Log.d("qwe","ConnectionService.handleMessage()");
		Bundle b;
		switch (msg.what) {
		case GET_MESSENGER:
			activityMessenger = msg.replyTo;
			connectionTask.setMessenger(activityMessenger);
			break;
		case GENERATE_KEY:
			connectionTask.enqueueMessage(new PacketKeyRequest());
			break;
		case CONNECT:
			b = msg.getData();
			String host = b.getString(AdminTools.HOST);
			int port = b.getInt(AdminTools.PORT);
			
			String key = b.getString(AdminTools.KEY);
			int interv = b.getInt(AdminTools.INTERVAL);
			Log.d("qwe","key "+port+" "+host+" "+key);
			connectionTask.connect(host,port,key, (short)interv );
			break;
		case DISCONNECT:
			if(!connectionTask.isConnected()){
				connectionTask.disconnect();
			}
			break;
		case STOP:
			connectionTask.stop();
			break;
		case SEND_CONFIG:
			b = msg.getData();
			PacketConfig data = (PacketConfig)b.getSerializable(PacketConfig.PACKET_CONFIG);
			connectionTask.enqueueMessage(data);
			break;
		case REQUEST_CONFIG:
			b = msg.getData();
			short id = b.getShort(PacketConfigRequest.ID);
			connectionTask.enqueueMessage(new PacketConfigRequest(id));
			break;
		case SET_INTERVAL:
			b = msg.getData();
			int interval = b.getInt(AdminTools.INTERVAL);
			connectionTask.enqueueMessage(new PacketStart((short) interval));
		case STATS_REQUEST:
			b = msg.getData();
			PacketStatsRequest stats = (PacketStatsRequest)b.getSerializable(PacketStatsRequest.PACKET_STATS_REQUEST);;
			connectionTask.enqueueMessage(stats);
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