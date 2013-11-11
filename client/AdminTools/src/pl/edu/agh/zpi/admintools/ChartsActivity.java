package pl.edu.agh.zpi.admintools;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketConfig;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsRequest;
import pl.edu.agh.zpi.admintools.connection.packets.ServiceConfig;
import pl.edu.agh.zpi.admintools.listdata.ChartsSurface;
import pl.edu.agh.zpi.admintools.sensors.AgentData;
import pl.edu.agh.zpi.admintools.sensors.DiskUsageData;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.DatePickerDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.ToggleButton;

public class ChartsActivity extends Activity implements ServiceConnection,
		Handable {
	public static final String AGENT = "pl.edu.agh.zpi.admintools.ChartsActivity.AGENT";

	private boolean isServiceBinded;
	private AgentData agent;

	private String host;
	private int port;
	private String key;
	private int interval;

	private ChartsSurface surface;
	private Button timeStart, timeEnd;
	// private Button settings;
	// private Button submit;
	private TextView information;
	private TextView timeNew, timeOld;
	private TextView valueMax, valueMin;

	private PacketStatsRequest statsRequest = new PacketStatsRequest();
	private PacketStatsRequest lastSend;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(
			this));

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_charts);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		agent = (AgentData) this.getIntent().getSerializableExtra(AGENT);
		host = this.getIntent().getStringExtra(AdminTools.HOST);
		port = this.getIntent().getIntExtra(AdminTools.PORT, 0);
		key = this.getIntent().getStringExtra(AdminTools.KEY);
		interval = this.getIntent().getIntExtra(AdminTools.INTERVAL, 1000);

		surface = (ChartsSurface) findViewById(R.id.surfaceView_charts);
		timeStart = (Button) findViewById(R.id.button_date_start);
		timeEnd = (Button) findViewById(R.id.button_date_end);
		// settings = (Button) findViewById(R.id.button_charts_settings);
		// submit = (Button) findViewById(R.id.button_submit_charts);
		information = (TextView) findViewById(R.id.textView_charts_what);
		timeNew = (TextView) findViewById(R.id.textView_charts_time_new);
		timeOld = (TextView) findViewById(R.id.textView_charts_time_old);
		valueMax = (TextView) findViewById(R.id.textView_charts_value_max);
		valueMin = (TextView) findViewById(R.id.textView_charts_value_min);

		String today = "", tomorrow = "";
		Calendar c = Calendar.getInstance();
		c.set(Calendar.MINUTE, 0);
		c.set(Calendar.HOUR_OF_DAY, 0);
		statsRequest.setStartDate((int) (c.getTime().getTime() / 1000));
		Log.d("qwe", "" + (int) (c.getTime().getTime() / 1000));
		today += c.get(Calendar.DAY_OF_MONTH) + "-";
		today += c.get(Calendar.MONTH) + "-";
		today += c.get(Calendar.YEAR);
		c.add(Calendar.DAY_OF_YEAR, 1);
		statsRequest.setEndDate((int) (c.getTime().getTime() / 1000));
		tomorrow += c.get(Calendar.DAY_OF_MONTH) + "-";
		tomorrow += c.get(Calendar.MONTH) + "-";
		tomorrow += c.get(Calendar.YEAR);

		timeStart.setText(today);
		timeEnd.setText(tomorrow);

		statsRequest.setAgentId(agent.getId());
		statsRequest.setDataType(PacketStatsRequest.TEMP);
		statsRequest.setDiskName("");
		statsRequest
				.setPoints((short) (surface.getWidth() / ChartsSurface.ACCURACY));

		isServiceBinded = bindService(
				new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);

		setResult(RESULT_OK);
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
		sendMessageToService(ConnectionService.CONNECT, host, port, key,
				interval);
		sendMessageToService(ConnectionService.STOP);
		super.onResume();
	}

	@Override
	public void handleMessage(Message msg) {
		Log.d("qwe", "ChartsActivity.handleMessage() " + msg.what);
		switch (msg.what) {
		case ConnectionTask.CONNECTION_ERROR:
			setResult(RESULT_CANCELED);
			finish();
			break;
		case ConnectionTask.STATS_REPLY:
			PacketStatsReply psr = (PacketStatsReply) msg.getData().get(
					PacketStatsReply.PACKET_STATS_REPLY);
			surface.setData(psr.getPoints(), statsRequest.getDataType());
		default:
			// sendMessageToService(ConnectionService.STOP);
			break;
		}
	}

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT, host, port, key,
				interval);
		sendMessageToService(ConnectionService.STOP);
	}

	@Override
	public void onServiceDisconnected(ComponentName name) {
		Log.d("qwe", "ChartsActivity.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
	}

	public void submit(View view) {
		statsRequest
				.setPoints((short) (surface.getWidth() / ChartsSurface.ACCURACY));
		lastSend = statsRequest;
		sendMessageToService(ConnectionService.STATS_REQUEST, statsRequest);
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
				m.setData(b);
				break;
			case ConnectionService.STATS_REQUEST:
				b.putSerializable(PacketStatsRequest.PACKET_STATS_REQUEST,
						(Serializable) data[0]);
				m.setData(b);
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

	public void setAxis(int minVal, int maxVal) {
		String start = "", end = "";
		Calendar c = Calendar.getInstance();

		c.setTimeInMillis((long) lastSend.getEndDate() * 1000);
		end += c.get(Calendar.DAY_OF_MONTH) + "-";
		end += c.get(Calendar.MONTH) + "-";
		end += c.get(Calendar.YEAR);
		timeNew.setText(end);

		c.setTimeInMillis((long) lastSend.getStartDate() * 1000);
		start += c.get(Calendar.DAY_OF_MONTH) + "-";
		start += c.get(Calendar.MONTH) + "-";
		start += c.get(Calendar.YEAR);
		timeOld.setText(start);

		valueMax.setText("" + maxVal);
		valueMin.setText("" + minVal);

		String info = "";
		switch (lastSend.getDataType()) {
		case PacketStatsRequest.CPU:
			info += "CPU\n";
			break;
		case PacketStatsRequest.DISK:
			info += "DISK\n";
			break;
		case PacketStatsRequest.RAM:
			info += "RAM\n";
			break;
		case PacketStatsRequest.TEMP:
			info += "TEMP\n";
			break;
		default:
			break;
		}
		information.setText(info + lastSend.getDiskName());
	}

	public void updateDate() {
		String start = "", end = "";
		Calendar c = Calendar.getInstance();

		c.setTimeInMillis((long) statsRequest.getEndDate() * 1000);
		end += c.get(Calendar.DAY_OF_MONTH) + "-";
		end += c.get(Calendar.MONTH) + "-";
		end += c.get(Calendar.YEAR);
		timeEnd.setText(end);

		c.setTimeInMillis((long) statsRequest.getStartDate() * 1000);
		start += c.get(Calendar.DAY_OF_MONTH) + "-";
		start += c.get(Calendar.MONTH) + "-";
		start += c.get(Calendar.YEAR);
		timeStart.setText(start);

		submit(null);
	}

	public void setDate(View view) {
		DialogFragment dateFragment = null;
		if (view.getId() == R.id.button_date_start) {
			dateFragment = new DatePickerFragment(statsRequest, this, true);
		} else {
			dateFragment = new DatePickerFragment(statsRequest, this, false);
		}
		dateFragment.show(getFragmentManager(), "datePicker");
	}

	public void setDataType(View view) {
		final LayoutInflater inflater = this.getLayoutInflater();
		final View dialogView = inflater.inflate(R.layout.charts_config, null);
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setView(dialogView)
				.setPositiveButton(R.string.save,
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int id) {
								statsRequest.setDiskName("");
								RadioGroup typeGroup = (RadioGroup) dialogView
										.findViewById(R.id.radioGroup_chart_dialog_type);
								RadioGroup diskGroup = (RadioGroup) dialogView
										.findViewById(R.id.radioGroup_chart_dialog_disk);
								switch (typeGroup.getCheckedRadioButtonId()) {
								case R.id.radio_CPU:
									statsRequest
											.setDataType(PacketStatsRequest.CPU);
									break;
								case R.id.radio_RAM:
									statsRequest
											.setDataType(PacketStatsRequest.RAM);
									break;
								case R.id.radio_TEMP:
									statsRequest
											.setDataType(PacketStatsRequest.TEMP);
									break;
								case R.id.radio_DISK:
									statsRequest
											.setDataType(PacketStatsRequest.DISK);
									try {
										RadioButton diskRadio = (RadioButton) dialogView.findViewById(diskGroup
												.getCheckedRadioButtonId());
										statsRequest.setDiskName(diskRadio
												.getText().toString());
									} catch (Exception e) {
										// nie ma żadnych dysków?
										e.printStackTrace();
									}
									break;
								}
								submit(null);
							}
						})
				.setNegativeButton(R.string.cancel,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
							}
						});

		Dialog d = builder.create();
		d.show();

		RadioGroup typeGroup = (RadioGroup) d
				.findViewById(R.id.radioGroup_chart_dialog_type);
		final RadioGroup diskGroup = (RadioGroup) d
				.findViewById(R.id.radioGroup_chart_dialog_disk);
		typeGroup
				.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
					@Override
					public void onCheckedChanged(RadioGroup group, int checkedId) {
						if (checkedId == R.id.radio_DISK) {
							diskGroup.setVisibility(View.VISIBLE);
						} else {
							diskGroup.setVisibility(View.GONE);
						}
					}
				});

		for (DiskUsageData dud : agent.getData().getDiskUsages()) {
			RadioButton radio = new RadioButton(diskGroup.getContext());
			radio.setText(dud.getName());
			diskGroup.addView(radio);
			if (statsRequest.getDiskName().equals("")
					|| dud.getName().equals(statsRequest.getDiskName())) {
				View rb = diskGroup.getChildAt(diskGroup.getChildCount() - 1);
				diskGroup.check(rb.getId());
			}
		}

		switch (statsRequest.getDataType()) {
		case PacketStatsRequest.CPU:
			typeGroup.check(R.id.radio_CPU);
			break;
		case PacketStatsRequest.RAM:
			typeGroup.check(R.id.radio_RAM);
			break;
		case PacketStatsRequest.TEMP:
			typeGroup.check(R.id.radio_TEMP);
			break;
		case PacketStatsRequest.DISK:
			typeGroup.check(R.id.radio_DISK);
			break;
		}
	}

	/*
	 * @Override public boolean onCreateOptionsMenu(Menu menu) { // Inflate the
	 * menu; this adds items to the action bar if it is present.
	 * getMenuInflater().inflate(R.menu.charts, menu); return true; }
	 * 
	 * @Override public boolean onOptionsItemSelected(MenuItem item) { switch
	 * (item.getItemId()) { case android.R.id.home: // This ID represents the
	 * Home or Up button. In the case of this // activity, the Up button is
	 * shown. Use NavUtils to allow users // to navigate up one level in the
	 * application structure. For // more details, see the Navigation pattern on
	 * Android Design: // //
	 * http://developer.android.com/design/patterns/navigation.html#up-vs-back
	 * // NavUtils.navigateUpFromSameTask(this); return true; } return
	 * super.onOptionsItemSelected(item); }
	 */
}

/****************************************/
/*************** KLASY POMOCNICZE *******/
/****************************************/

class DatePickerFragment extends DialogFragment implements
		DatePickerDialog.OnDateSetListener {
	private boolean isStartTime;
	private PacketStatsRequest packet;
	private ChartsActivity parent;

	public DatePickerFragment(PacketStatsRequest packet, ChartsActivity parent,
			boolean isStartTime) {
		super();
		this.isStartTime = isStartTime;
		this.packet = packet;
		this.parent = parent;
	}

	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState) {
		final Calendar c = Calendar.getInstance();
		if (isStartTime) {
			c.setTimeInMillis((long) packet.getStartDate() * 1000);
		} else {
			c.setTimeInMillis((long) packet.getEndDate() * 1000);
		}
		int year = c.get(Calendar.YEAR);
		int month = c.get(Calendar.MONTH);
		int day = c.get(Calendar.DAY_OF_MONTH);

		return new DatePickerDialog(getActivity(), this, year, month, day);
	}

	@Override
	public void onDateSet(DatePicker view, int year, int monthOfYear,
			int dayOfMonth) {
		Calendar currentDate = Calendar.getInstance();
		currentDate.set(Calendar.HOUR_OF_DAY, 0);
		currentDate.set(Calendar.MINUTE, 0);
		Calendar tomorrow = Calendar.getInstance();
		tomorrow.setTimeInMillis(currentDate.getTimeInMillis());
		tomorrow.add(Calendar.DAY_OF_MONTH, 1);
		Calendar monthAgo = Calendar.getInstance();
		monthAgo.setTimeInMillis(currentDate.getTimeInMillis());
		monthAgo.add(Calendar.MONTH, -1);

		final Calendar c = Calendar.getInstance();
		c.setTimeInMillis(0l);
		c.set(Calendar.YEAR, year);
		c.set(Calendar.MONTH, monthOfYear);
		c.set(Calendar.DAY_OF_MONTH, dayOfMonth);

		if (isStartTime) {
			if (c.getTimeInMillis() < monthAgo.getTimeInMillis()) {
				packet.setStartDate((int) (monthAgo.getTimeInMillis() / 1000));
			} else if ((int) (c.getTimeInMillis() / 1000) >= packet
					.getStartDate()) {
				long time = (long) packet.getEndDate();
				time *= 1000;
				c.setTimeInMillis(time);
				c.add(Calendar.DAY_OF_MONTH, -1);
				packet.setStartDate((int) (c.getTimeInMillis() / 1000));
			} else
				packet.setStartDate((int) (c.getTimeInMillis() / 1000));
		} else {
			if (c.getTimeInMillis() > tomorrow.getTimeInMillis()) {
				packet.setEndDate((int) (tomorrow.getTimeInMillis() / 1000));
			} else if ((int) (c.getTimeInMillis() / 1000) <= packet
					.getEndDate()) {
				long time = (long) packet.getStartDate();
				time *= 1000;
				c.setTimeInMillis(time);
				c.add(Calendar.DAY_OF_MONTH, 1);
				packet.setEndDate((int) (c.getTimeInMillis() / 1000));
			} else
				packet.setEndDate((int) (c.getTimeInMillis() / 1000));
		}
		parent.updateDate();
	}
}
