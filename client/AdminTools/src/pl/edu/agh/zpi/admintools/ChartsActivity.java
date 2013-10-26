package pl.edu.agh.zpi.admintools;

import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.support.v4.app.NavUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

public class ChartsActivity extends Activity implements ServiceConnection,
		Handable {
	public static final String AGENT_ID = "pl.edu.agh.zpi.admintools.ChartsActivity.AGENT_ID";

	private boolean isServiceBinded;
	private short agentId;
	private String host;
	private int port;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(
			this));

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_charts);

		agentId = this.getIntent().getShortExtra(AGENT_ID, (short) 0xFFFF);
		host = this.getIntent().getStringExtra(AdminTools.HOST);
		port = this.getIntent().getIntExtra(AdminTools.PORT, 0);

		isServiceBinded = bindService(
				new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
	}

	@Override
	protected void onDestroy() {
		if (isServiceBinded) {
			unbindService(this);
			isServiceBinded = false;
			serviceMessenger = null;
		}
		super.onDestroy();
	}

	@Override
	protected void onStop() {
		super.onStop();
	}

	@Override
	protected void onResume() {
		Log.d("qwe", "ChartsActivity.onResume()" + serviceMessenger);
		if (serviceMessenger == null) {
			isServiceBinded = bindService(new Intent(this,
					ConnectionService.class), this, Context.BIND_AUTO_CREATE);
		}
		sendMessageToService(ConnectionService.CONNECT, host, port);
		super.onResume();
	}

	@Override
	public void handleMessage(Message msg) {
		Log.d("qwe", "ChartsActivity.handleMessage()");
		switch(msg.what){
		default:
			sendMessageToService(ConnectionService.STOP);
			break;
		}
	}

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT, host, port);
	}

	@Override
	public void onServiceDisconnected(ComponentName name) {
		Log.d("qwe", "ChartsActivity.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
	}

	private void sendMessageToService(int type, Object... data) {
		if (serviceMessenger != null) {
			Message m = Message.obtain(null, type);
			Bundle b = new Bundle();
			switch (type) {
			case ConnectionService.CONNECT:
				b.putString(AdminTools.HOST, (String) data[0]);
				b.putInt(AdminTools.PORT, (Integer) data[1]);
				m.setData(b);
				break;
			default:
				break;
			}
			m.replyTo = activityMessenger;
			try {
				serviceMessenger.send(m);
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.charts, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			// This ID represents the Home or Up button. In the case of this
			// activity, the Up button is shown. Use NavUtils to allow users
			// to navigate up one level in the application structure. For
			// more details, see the Navigation pattern on Android Design:
			//
			// http://developer.android.com/design/patterns/navigation.html#up-vs-back
			//
			NavUtils.navigateUpFromSameTask(this);
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

}
