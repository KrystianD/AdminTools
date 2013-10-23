package pl.edu.agh.zpi.admintools;

import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAgentsData;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfig;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfigRequest;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyReply;
import pl.edu.agh.zpi.admintools.connection.packets.ServiceConfig;
import pl.edu.agh.zpi.admintools.listdata.AgentArrayAdapter;
import pl.edu.agh.zpi.admintools.listdata.LongClickItemListener;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Dialog;
import android.app.Service;
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
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.ToggleButton;

public class StatsActivity extends Activity implements ServiceConnection,
		Handable {
	private final static int SETTINGS = 0;
	private final static int CHARTS = 1;

	private ListView listView;
	private AgentArrayAdapter agentsArray;
	private LongClickItemListener longClickListener;
	private boolean isServiceBinded;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(
			this));

	private String host;
	private int port;

	private TextWatcher shortTextWatcher = new ShortTextWatcher();
	private OnFocusChangeListener shortFocusChangeListener = new ShortOnFocusChangeListener();

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_stats_activity);

		host = this.getIntent().getStringExtra(AdminTools.HOST);
		port = this.getIntent().getIntExtra(AdminTools.PORT, 0);

		TextView tv = (TextView) findViewById(R.id.textView_server_name);
		String serverName = getIntent().getStringExtra(AdminTools.HOST) + ":"
				+ getIntent().getIntExtra(AdminTools.PORT, 0);
		tv.setText(serverName);

		agentsArray = new AgentArrayAdapter(this);
		longClickListener = new LongClickItemListener(this);

		listView = (ListView) findViewById(R.id.listView_agents_data);
		listView.setAdapter(agentsArray);
		listView.setLongClickable(true);
		listView.setOnItemLongClickListener(longClickListener);

		isServiceBinded = bindService(
				new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
	}

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT, host, port);
	}

	@Override
	public void onServiceDisconnected(ComponentName name) {
		Log.d("qwe", "StatsActivity.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
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
		sendMessageToService(ConnectionService.STOP);

		super.onStop();
	}

	@Override
	protected void onResume() {
		Log.d("qwe", "StatsActivity.onResume()" + serviceMessenger);
		if (serviceMessenger == null) {
			isServiceBinded = bindService(new Intent(this,
					ConnectionService.class), this, Context.BIND_AUTO_CREATE);
		}
		sendMessageToService(ConnectionService.CONNECT, host, port);
		super.onResume();
	}

	@Override
	public void handleMessage(Message msg) {
		Log.d("qwe", "StatsActivity handleMessage");
		Bundle b;
		switch (msg.what) {
		case ConnectionTask.AGENTS_DATA:
			b = msg.getData();

			PacketAgentsData pad = (PacketAgentsData) b
					.get(PacketAgentsData.PACKET_AGENTS_DATA);

			agentsArray.clear();
			agentsArray.addAll(pad.getAgentsList());
			agentsArray.notifyDataSetChanged();
			break;
		case ConnectionTask.AGENT_KEY:
			b = msg.getData();
			PacketKeyReply pkr = (PacketKeyReply) b
					.get(PacketKeyReply.PACKET_KEY_REPLY);
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
			TextView tv = (TextView) d
					.findViewById(R.id.textView_dialog_get_key);
			tv.setText(pkr.toString());
			break;
		case ConnectionTask.AGENT_CONFIG:
			b = msg.getData();
			PacketConfig pc = (PacketConfig) b.get(PacketConfig.PACKET_CONFIG);
			buildSettingsDialog(pc);
			break;
		default:
			break;
		}

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
			case ConnectionService.REQUEST_CONFIG:
				b.putShort(PacketConfigRequest.ID, (Short)data[0]);
				m.setData(b);
				break;
			case ConnectionService.SEND_CONFIG:
				b.putSerializable(PacketConfig.PACKET_CONFIG, (PacketConfig)data[0]);
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

	public void listenerFeedback(int which, short id) {
		switch (which) {
		case SETTINGS:
			sendMessageToService(ConnectionService.REQUEST_CONFIG, id);
			break;
		case CHARTS:

			break;
		}
	}

	private void buildSettingsDialog(PacketConfig pc) {
		Log.d("qwe", pc.toString());
		LayoutInflater inflater = this.getLayoutInflater();
		final short agentId = pc.getAgentId();
		final View dialogView = inflater.inflate(R.layout.dialog_agent_config,
				null);
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("ID: " + agentId);
		builder.setView(dialogView)
				.setPositiveButton(R.string.save,
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int id) {
								String tempPath = ((EditText) dialogView
										.findViewById(R.id.editText_dialog_config_tempPath))
										.getText().toString();
								String tempDivStr = ((EditText) dialogView
										.findViewById(R.id.editText_dialog_config_tempDivider))
										.getText().toString();
								String intervalStr = ((EditText) dialogView
										.findViewById(R.id.editText_dialog_config_interval))
										.getText().toString();

								short tempDivider = Short
										.parseShort(tempDivStr);
								short interval = Short.parseShort(intervalStr);

								ArrayList<ServiceConfig> servicesConfig = new ArrayList<ServiceConfig>();

								LinearLayout layout = (LinearLayout) dialogView
										.findViewById(R.id.linearLayout_dialog_config_services);

								for (int i = 0; i < layout.getChildCount(); i++) {
									View v = layout.getChildAt(i);

									String name = ((EditText) v
											.findViewById(R.id.editText_dialog_agent_config_service_name))
											.getText().toString();
									boolean isTCP = ((ToggleButton) v
											.findViewById(R.id.toggleButton_dialog_agent_config_service_isTcp))
											.isChecked();
									short port = Short.parseShort(((EditText) v
											.findViewById(R.id.editText_dialog_agent_config_service_port))
											.getText().toString());

									servicesConfig.add(new ServiceConfig(name,
											isTCP, port));
								}

								PacketConfig pc = new PacketConfig(agentId,
										tempPath, tempDivider, servicesConfig,
										interval);

								Log.d("qwe", pc.toString());
								
								sendMessageToService(ConnectionService.SEND_CONFIG, pc);
							}
						})
				.setNegativeButton(R.string.cancel,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
							}
						});

		Dialog d = builder.create();
		d.show();

		EditText tempPath = (EditText) d
				.findViewById(R.id.editText_dialog_config_tempPath);
		EditText tempDivider = (EditText) d
				.findViewById(R.id.editText_dialog_config_tempDivider);
		EditText interval = (EditText) d
				.findViewById(R.id.editText_dialog_config_interval);
		LinearLayout servicesLayout = (LinearLayout) d
				.findViewById(R.id.linearLayout_dialog_config_services);

		tempDivider.addTextChangedListener(shortTextWatcher);
		tempDivider.setOnFocusChangeListener(shortFocusChangeListener);
		interval.addTextChangedListener(shortTextWatcher);
		interval.setOnFocusChangeListener(shortFocusChangeListener);

		tempPath.setText(pc.getTempPath());
		tempDivider.setText("" + pc.getTempDivider());
		interval.setText("" + pc.getInterval());

		ArrayList<ServiceConfig> serviceArray = pc.getServicesConfig();
		for (ServiceConfig sc : serviceArray) {
			View child = getLayoutInflater()
					.inflate(R.layout.dialog_agent_config_service,
							servicesLayout, false);
			servicesLayout.addView(child);

			EditText name = (EditText) child
					.findViewById(R.id.editText_dialog_agent_config_service_port);
			EditText port = (EditText) child
					.findViewById(R.id.editText_dialog_agent_config_service_name);
			ToggleButton isTcp = (ToggleButton) child
					.findViewById(R.id.toggleButton_dialog_agent_config_service_isTcp);

			port.addTextChangedListener(shortTextWatcher);
			port.setOnFocusChangeListener(shortFocusChangeListener);

			name.setText(sc.getName());
			port.setText("" + sc.getPort());
			isTcp.setChecked(sc.isTCP());
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
			// TODO jakieś tam settingsy
			break;
		case R.id.action_generate_agent_key:
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setMessage(getString(R.string.generate_agent_dialog));
			builder.setPositiveButton(R.string.ok, new OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if (serviceMessenger != null) {
						Message m = Message.obtain(null,
								ConnectionService.GENERATE_KEY);
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

	public void toggleTable(View view) {
		agentsArray.notifyDataSetChanged();
	}

	public void onDialogConfigButtonClicked(View view) {
		LinearLayout parent = (LinearLayout) view.getParent().getParent();
		LinearLayout layout = (LinearLayout) parent.getChildAt(7); // magic
																	// number!!
		if (view.getId() == R.id.button_dialog_config_add_service) {
			View child = getLayoutInflater().inflate(
					R.layout.dialog_agent_config_service, layout, false);
			EditText et = (EditText) child
					.findViewById(R.id.editText_dialog_agent_config_service_port);
			et.addTextChangedListener(shortTextWatcher);
			et.setOnFocusChangeListener(shortFocusChangeListener);
			layout.addView(child);
		} else {
			int index = layout.getChildCount();
			Log.d("qwe", "index= " + index + "  layout" + layout);
			if (index > 0)
				layout.removeViewAt(index - 1);
		}
	}
}

class ShortTextWatcher implements TextWatcher {
	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count) {
	}

	@Override
	public void beforeTextChanged(CharSequence s, int start, int count,
			int after) {
	}

	@Override
	public void afterTextChanged(Editable s) {
		String str = s.toString();
		if (!str.matches("^[0-9]+") && !str.equals("")) {
			s.delete(s.length() - 1, s.length() - 1);
			str = s.toString();
		}
		try {
			Short.parseShort(str);
		} catch (Exception e) {
			s.clear();
			if (!str.equals("")) {
				s.append("" + (Short.MAX_VALUE - 1));
			}
		}
	}
};

class ShortOnFocusChangeListener implements View.OnFocusChangeListener {

	@Override
	public void onFocusChange(View v, boolean hasFocus) {
		EditText et = (EditText) v;
		String txt = et.getText().toString();
		if (txt.equals("")) {
			et.setText("0");
		}
	}

}