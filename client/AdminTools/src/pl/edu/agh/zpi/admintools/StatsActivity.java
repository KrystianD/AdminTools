package pl.edu.agh.zpi.admintools;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAgentsData;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyReply;
import pl.edu.agh.zpi.admintools.listdata.AgentArrayAdapter;
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
import android.view.View;
import android.widget.ListView;
import android.widget.TextView;

public class StatsActivity extends Activity implements ServiceConnection, Handable{
	private ListView listView;
	private AgentArrayAdapter agentsArray;
	private boolean isServiceBinded;
	
	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(this));
	
	private String host;
	private int port;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_stats_activity);
		
		host = this.getIntent().getStringExtra(AdminTools.HOST);
		port = this.getIntent().getIntExtra(AdminTools.PORT, 0);
		
		TextView tv = (TextView)findViewById(R.id.textView_server_name);
		String serverName = getIntent().getStringExtra(AdminTools.HOST) + ":" + getIntent().getIntExtra(AdminTools.PORT,0);
		tv.setText(serverName);
		
		agentsArray = new AgentArrayAdapter(this);

		listView = (ListView)findViewById(R.id.listView_agents_data);
		listView.setAdapter(agentsArray);
		
		isServiceBinded = bindService(new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
	}	

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT);
	}

	@Override
	public void onServiceDisconnected(ComponentName name) {
		Log.d("qwe", "StatsActivity.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
	}

	@Override
	protected void onDestroy() {
		if(isServiceBinded){
			unbindService(this);
			isServiceBinded = false;
			serviceMessenger = null;
		}
		super.onDestroy();
	}

	@Override
	protected void onStop() {
		sendMessageToService(ConnectionService.STOP);
	
		super.onStop();
	}

	@Override
	protected void onResume() {
		Log.d("qwe", "StatsActivity.onResume()" + serviceMessenger);
		if(serviceMessenger == null){
			isServiceBinded = bindService(new Intent(this, ConnectionService.class), this,
					Context.BIND_AUTO_CREATE);
		}
		sendMessageToService(ConnectionService.CONNECT);
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
			
			agentsArray.clear();
			agentsArray.addAll(pad.getAgentsList());
			agentsArray.notifyDataSetChanged();
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
	
	private void sendMessageToService(int type){
		if(serviceMessenger != null){
			Message m = Message.obtain(null, type);
			if(type == ConnectionService.CONNECT){
				Bundle b = new Bundle();
				b.putInt(AdminTools.PORT, port);
				b.putString(AdminTools.HOST, host);
				m.setData(b);
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
	
	public void toggleTable(View view){
		agentsArray.notifyDataSetChanged();
	}
}
