package pl.edu.agh.zpi.admintools;

import java.util.ArrayList;
import java.util.zip.Inflater;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketAgentsData;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfig;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfigRequest;
import pl.edu.agh.zpi.admintools.connection.packets.PacketKeyReply;
import pl.edu.agh.zpi.admintools.connection.packets.ServiceConfig;
import pl.edu.agh.zpi.admintools.listdata.AgentArrayAdapter;
import pl.edu.agh.zpi.admintools.sensors.AgentData;
import pl.edu.agh.zpi.admintools.sensors.DiskUsageData;
import pl.edu.agh.zpi.admintools.sensors.SensorsData;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.support.v4.app.NavUtils;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.ToggleButton;

public class StatsActivity extends Activity implements ServiceConnection,
		Handable {
	private final static int SETTINGS = 0;
	private final static int CHARTS = 1;

	// ! Temperature alert threshold.
	public static double tempAlertLevel = 85;
	// ! HDD usage alert percent threshold.
	public static double HDDAlertLevel = 0.95;

	private ListView listView;
	private AgentArrayAdapter agentsArray;
	// private LongClickItemListener longClickListener;
	private boolean isServiceBinded;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(
			this));

	private String host;
	private int port;
	private String key;
	private int interval;

	private TextWatcher shortTextWatcher = new ShortTextWatcher();
	private OnFocusChangeListener shortFocusChangeListener = new ShortOnFocusChangeListener();

	private SharedPreferences connectionSettings;

	private LinearLayout linearLayoutAlertsInternal;
	private LinearLayout linearLayoutAlertsBounding;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_stats_activity);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		connectionSettings = getSharedPreferences(AdminTools.CONN_PREFS_NAME,
				MODE_MULTI_PROCESS);

		Log.d("qwe", "StatsActivity.onCreate()");

		// host = this.getIntent().getStringExtra(AdminTools.HOST);
		// port = this.getIntent().getIntExtra(AdminTools.PORT, 0);
		// key = this.getIntent().getStringExtra(AdminTools.KEY);
		// interval = this.getIntent().getIntExtra(AdminTools.INTERVAL, 1000);

		host = connectionSettings.getString(AdminTools.HOST, "");
		port = Integer.parseInt(connectionSettings.getString(AdminTools.PORT,
				""));
		key = connectionSettings.getString(AdminTools.KEY, "");
		interval = connectionSettings.getInt(AdminTools.INTERVAL, 0);

		TextView tv = (TextView) findViewById(R.id.textView_server_name);
		String serverName = host + ":" + port;
		tv.setText(serverName);

		linearLayoutAlertsInternal = (LinearLayout) findViewById(R.id.linearLayout_alerts_internal);
		linearLayoutAlertsBounding = (LinearLayout) findViewById(R.id.linearLayout_alerts);
		
		agentsArray = new AgentArrayAdapter(this);
		// longClickListener = new LongClickItemListener(this);

		listView = (ListView) findViewById(R.id.listView_agents_data);
		listView.setAdapter(agentsArray);
		// listView.setLongClickable(true);
		// listView.setOnItemLongClickListener(longClickListener);

		isServiceBinded = bindService(
				new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);

		setResult(RESULT_OK); // na wstępie zakładamy że jest ok
	}

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT, host, port, key,
				interval);
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
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT, host, port, key,
				interval);
		super.onResume();
	}

	@Override
	public void handleMessage(Message msg) {
		Log.d("qwe", "StatsActivity handleMessage " + msg.what);
		Bundle b;
		switch (msg.what) {
		case ConnectionTask.AGENTS_DATA:
			b = msg.getData();

			PacketAgentsData pad = (PacketAgentsData) b
					.get(PacketAgentsData.PACKET_AGENTS_DATA);

			ArrayList<AgentData> agents = pad.getAgentsList();
			for (int i = 0; i < agents.size(); i++) {
				AgentData agent = agents.get(i);
				boolean isAlerted = checkAlert(agent, i);
				agentsArray.set(i, agent, isAlerted);
			}

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
		case ConnectionTask.CONNECTION_ERROR:
			setResult(RESULT_CANCELED);
			finish();
		case ConnectionTask.AUTH_FAILED:
			Toast.makeText(getApplicationContext(), getString(R.string.auth_error), Toast.LENGTH_SHORT).show();
			finish();
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
				b.putString(AdminTools.KEY, (String) data[2]);
				b.putInt(AdminTools.INTERVAL, (Integer) data[3]);
				break;
			case ConnectionService.REQUEST_CONFIG:
				b.putShort(PacketConfigRequest.ID, (Short) data[0]);
				break;
			case ConnectionService.SEND_CONFIG:
				b.putSerializable(PacketConfig.PACKET_CONFIG,
						(PacketConfig) data[0]);
				break;
			case ConnectionService.SET_INTERVAL:
				b.putInt(AdminTools.INTERVAL, interval);
				break;
			default:
				break;
			}
			m.setData(b);
			m.replyTo = activityMessenger;
			try {
				serviceMessenger.send(m);
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}

	public void manageAgentListButtons(View view) {
		short id = (Short) ((Button) view).getTag();
		if (view.getId() == R.id.button_list_settings) {
			sendMessageToService(ConnectionService.REQUEST_CONFIG, id);
		} else {
			AgentData agent = null;
			for (int i = 0; i < agentsArray.getCount(); i++) {
				AgentData tmp = agentsArray.getItem(i);
				if (tmp.getId() == id) {
					agent = tmp;
					break;
				}
			}
			Intent intent = new Intent(this, ChartsActivity.class);
			intent.putExtra(AdminTools.PORT, port);
			intent.putExtra(AdminTools.HOST, host);
			intent.putExtra(AdminTools.KEY, key);
			intent.putExtra(ChartsActivity.AGENT, agent);
			startActivityForResult(intent, AdminTools.CHARTS_ACTIVITY_CODE);
		}
	}

	private void buildSettingsDialog(PacketConfig pc) {
		Log.d("qwe", pc.toString());

		AgentData agent = null;
		for (int i = 0; i < agentsArray.getCount(); i++) {
			AgentData tmp = agentsArray.getItem(i);
			if (tmp.getId() == pc.getAgentId()) {
				agent = tmp;
				break;
			}
		}
		if (agent == null)
			return;

		LayoutInflater inflater = this.getLayoutInflater();
		final short agentId = pc.getAgentId();
		final View dialogView = inflater.inflate(R.layout.dialog_agent_config,
				null);
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(agent.getName());
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
								String agentName = ((EditText) dialogView
										.findViewById(R.id.editText_dialog_config_name))
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
										interval, agentName);

								Log.d("qwe", pc.toString());

								sendMessageToService(
										ConnectionService.SEND_CONFIG, pc);
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
		EditText name = (EditText) d
				.findViewById(R.id.editText_dialog_config_name);
		LinearLayout servicesLayout = (LinearLayout) d
				.findViewById(R.id.linearLayout_dialog_config_services);

		tempDivider.addTextChangedListener(shortTextWatcher);
		tempDivider.setOnFocusChangeListener(shortFocusChangeListener);
		interval.addTextChangedListener(shortTextWatcher);
		interval.setOnFocusChangeListener(shortFocusChangeListener);

		tempPath.setText(pc.getTempPath());
		tempDivider.setText("" + pc.getTempDivider());
		interval.setText("" + pc.getInterval());
		name.setText(pc.getName());

		ArrayList<ServiceConfig> serviceArray = pc.getServicesConfig();
		for (ServiceConfig sc : serviceArray) {
			View child = getLayoutInflater()
					.inflate(R.layout.dialog_agent_config_service,
							servicesLayout, false);
			servicesLayout.addView(child);

			EditText serviceName = (EditText) child
					.findViewById(R.id.editText_dialog_agent_config_service_name);
			EditText port = (EditText) child
					.findViewById(R.id.editText_dialog_agent_config_service_port);
			ToggleButton isTcp = (ToggleButton) child
					.findViewById(R.id.toggleButton_dialog_agent_config_service_isTcp);

			port.addTextChangedListener(shortTextWatcher);
			port.setOnFocusChangeListener(shortFocusChangeListener);

			serviceName.setText(sc.getName());
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
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		switch (item.getItemId()) {
		case R.id.action_interval:
			LayoutInflater inflater = this.getLayoutInflater();
			final View dialogView = inflater.inflate(
					R.layout.dialog_refresh_interval, null);
			builder.setTitle(R.string.dialog_set_interval);
			builder.setView(dialogView);
			builder.setPositiveButton(R.string.ok,
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							EditText editText = (EditText) dialogView
									.findViewById(R.id.editText_dialog_refresh_interval);
							double intervalDouble = Double.parseDouble(editText
									.getText().toString());
							interval = (int) intervalDouble;

							SharedPreferences.Editor editor = connectionSettings
									.edit();
							editor.putInt(AdminTools.INTERVAL, interval);
							editor.commit();

							sendMessageToService(
									ConnectionService.SET_INTERVAL, interval);
						}
					});
			builder.setNegativeButton(R.string.cancel,
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
						}
					});
			builder.show();

			EditText editText = (EditText) dialogView
					.findViewById(R.id.editText_dialog_refresh_interval);
			editText.setText(""
					+ connectionSettings.getInt(AdminTools.INTERVAL, 1000));

			break;
		case R.id.action_generate_agent_key:

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

	// rozwijanie listy agentów
	public void toggleTable(View view) {
		agentsArray.notifyDataSetChanged();
	}

	public void onDialogConfigButtonClicked(View view) {
		LinearLayout parent = (LinearLayout) view.getParent().getParent();
		LinearLayout layout = (LinearLayout) parent.getChildAt(9); // magic
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

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == AdminTools.CHARTS_ACTIVITY_CODE) {
			if (resultCode == RESULT_CANCELED) {
				setResult(RESULT_CANCELED);
				finish();
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	private boolean checkAlert(AgentData agent, int position) {
		SensorsData sensorsData = agent.getData();
		if (sensorsData.isTempValid()
				&& sensorsData.getTemp() >= tempAlertLevel) {
			setAlert(true, position, agent);
			return true;
		} else {
			for (DiskUsageData dud : sensorsData.getDiskUsages()) {
				double totalSpace = dud.getTotalSpace();
				double usedSpace = dud.getUsedSpace();
				if (usedSpace / totalSpace >= HDDAlertLevel) {
					setAlert(true, position, agent);
					return true;
				}
			}
		}
		setAlert(false, position, agent);
		return false;
	}

	private void setAlert(boolean isAlerted, int position, AgentData agent) {
		boolean added = false;
		for (int i = 0; i < linearLayoutAlertsInternal.getChildCount(); i++) {
			View view = linearLayoutAlertsInternal.getChildAt(i);
			if (view.getTag().equals(position)) {
				if (!isAlerted) {
					linearLayoutAlertsInternal.removeViewAt(i);
				}
				added = true;
				break;
			}
		}
		if (!added && isAlerted) {
			LayoutInflater inflater = (LayoutInflater) this
					.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			TextView alert = (TextView)inflater.inflate(R.layout.alert_text_view, null, true);
			
			alert.setTag(position);
			alert.setClickable(true);
			alert.setTextColor(Color.RED);
			alert.setText(agent.getName());
			
			linearLayoutAlertsInternal.addView(alert);
		}
		// jeżeli laout będzie za wysoki, ustawia 150dp, włącza się wtedy scrolling ^^
		linearLayoutAlertsBounding.getLayoutParams().height = LayoutParams.WRAP_CONTENT;
		final float scale = this.getResources().getDisplayMetrics().density;
		int pixels = (int) (150 * scale + 0.5f);
		if(linearLayoutAlertsBounding.getHeight() >= pixels ){
			linearLayoutAlertsBounding.getLayoutParams().height = pixels;
		}
	}

	public void findAgentByAlert(final View view) {
		view.setBackgroundColor(Color.GREEN);
		Log.d("qwe", "StatsActivity.findAgentByAlert " + view.getTag());
		int position = (Integer) view.getTag();
		listView.smoothScrollToPosition(position);
		view.postDelayed(new Runnable() {	
			@Override
			public void run() {
				view.setBackgroundColor(Color.TRANSPARENT);
			}
		}, 200);
	}

}

/************************/
/** additional classes **/
/************************/

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