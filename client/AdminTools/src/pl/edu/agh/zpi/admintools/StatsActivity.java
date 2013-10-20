package pl.edu.agh.zpi.admintools;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAgentsData;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyReply;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.support.v4.app.NavUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.TextView;

public class StatsActivity extends Activity implements ServiceConnection, Handable{
	private TextView tv;
	
	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(this));;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_stats_activity);
		
		tv = (TextView)findViewById(R.id.textView1);
		
		bindService(new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
	}	

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		Message m = Message.obtain(null, ConnectionService.GET_MESSENGER);
		m.replyTo = activityMessenger;
		try {
			serviceMessenger.send(m);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void onServiceDisconnected(ComponentName name) {
		serviceMessenger = null;
	}

	@Override
	protected void onPause() {
		unbindService(this);
		super.onDestroy();
	}

	@Override
	protected void onResume() {
		if(serviceMessenger != null){
			Message m = Message.obtain(null, ConnectionService.GET_MESSENGER);
			m.replyTo = activityMessenger;
			try {
				serviceMessenger.send(m);
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
		super.onResume();
	}

	@Override
	public void handleMessage(Message msg) {
		Log.d("qwe", "StatsActivity handleMessage");
		Bundle b;
		switch(msg.what){
		case ConnectionTask.AGENTS_DATA:
			b = msg.getData();
			PacketAgentsData pad = (PacketAgentsData)b.get(PacketAgentsData.PACKET_AGENTS_DATA);
			tv.setText(pad.toString());
			break;
		case ConnectionTask.AGENT_KEY:
			b = msg.getData();
			PacketKeyReply pkr = (PacketKeyReply)b.get(PacketKeyReply.PACKET_KEY_REPLY);
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			LayoutInflater li = this.getLayoutInflater();
			builder.setView(li.inflate(R.layout.dialog_get_key, null));
			builder.setNeutralButton(R.string.ok, new OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {		
				}
			});
			Dialog d = builder.create();
			d.show();
			TextView tv = (TextView)d.findViewById(R.id.textView_dialog_get_key);
			tv.setText(pkr.toString());
			break;
		default:
			break;
		}
		
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.stats, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.action_settings:
			//TODO jakieś tam settingsy
			break;
		case R.id.action_generate_agent_key:
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setMessage(getString(R.string.generate_agent_dialog));
			builder.setPositiveButton(R.string.ok,new OnClickListener() {	
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if(serviceMessenger != null){
						Message m = Message.obtain(null, ConnectionService.GENERATE_KEY);
						try {
							serviceMessenger.send(m);
						} catch (RemoteException e) {
							e.printStackTrace();
						}
					}
				}
			});
			builder.setNegativeButton(R.string.cancel, new OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
				}
			});
			builder.show();
			break;
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
