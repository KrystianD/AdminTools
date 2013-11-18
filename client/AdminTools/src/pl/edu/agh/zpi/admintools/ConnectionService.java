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

/**
 *	\class ConnectionService
 *	\brief Service for handling connection events and messages.
 */
public class ConnectionService extends Service implements Handable {
	//! Full service MESSENGER path.
	public static final String MESSENGER = "pl.edu.agh.zpi.admintools.ConnectionService.MESSENGER";
	//! Get messenger message type.
	public static final int GET_MESSENGER = 0;
	//! Generate key message type.
	public static final int GENERATE_KEY = 1;
	//! Connect message type.
	public static final int CONNECT = 2;
	//! Disconnect message type.
	public static final int DISCONNECT = 3;
	//! Send configuration message type.
	public static final int SEND_CONFIG = 4;
	//! Stop message type.
	public static final int STOP = 5;
	//! Request configuration message type.
	public static final int REQUEST_CONFIG = 6;
	//! Set interval message type.
	public static final int SET_INTERVAL = 7;
	//! Stats request message type.
	public static final int STATS_REQUEST = 8;
	//! Full connection service class name.
	public static final String NAME = "pl.edu.agh.zpi.admintools.ConnectionService";

	private Messenger serviceMessenger = new Messenger(
			new IncomingHandler(this));
	private Messenger activityMessenger;

	private ConnectionTask connectionTask = new ConnectionTask();
	private Thread connectionThread;

	/**
	 *	\fn public ConnectionService()
	 *	\brief Default constructor, calling super constructor.
	 */
	public ConnectionService() {
		super();
	}
	/**
	 *	\fn public void onCreate()
	 *	\brief Executed on service creation.
	 *	\return None.
	 */
	@Override
	public void onCreate() {
		connectionThread = new Thread(connectionTask);
		connectionThread.start();

		super.onCreate();
	}
	/**
	 *	\fn public IBinder onBind(Intent intent)
	 *	\brief Execute on service bind.
	 *	\return New binder.
	 */
	@Override
	public IBinder onBind(Intent intent) {
		Log.d("qwe", "ConnectionService.onBind()");

		return serviceMessenger.getBinder();
	}
	/**
	 *	\fn public void handleMessage(Message msg)
	 *	\brief Handle incoming connection message.
	 *	\return None.
	 */
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
	/**
	 *	\fn public boolean onUnbind(Intent intent)
	 *	\brief Execute on service unbinding.
	 *	\return If succeeded.
	 */
	@Override
	public boolean onUnbind(Intent intent) {
		Log.d("qwe", "service onUnbind");
		connectionTask.disconnect();
		stopSelf();
		return super.onUnbind(intent);
	}
	/**
	 *	\fn public void onDestroy()
	 *	\brief Execute on service destruction.
	 *	\return None.
	 */
	@Override
	public void onDestroy() {
		Log.d("qwe", "service onDestroy");
		super.onDestroy();
	}

}