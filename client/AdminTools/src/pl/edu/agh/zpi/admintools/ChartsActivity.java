/*
Copyright (c) 2013, AGH University of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package pl.edu.agh.zpi.admintools;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Calendar;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsReply;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsRequest;
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
import android.content.SharedPreferences;
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
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

/**
 *	\class ChartsActivity
 *	\brief Activity for interacting and handling charts visualization.
 */
public class ChartsActivity extends Activity implements ServiceConnection,
		Handable {
	//! Agent item full key.
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
	private Button typeButton;
	private TextView timeNew, timeOld;
	private TextView valueMax, valueMin;

	private PacketStatsRequest statsRequest = new PacketStatsRequest();
	private PacketStatsRequest lastSend;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(
			this));

	private SharedPreferences connectionSettings;
	/**
	 *	\fn protected void onCreate(Bundle savedInstanceState)
	 *	\brief Execute on activity creation.
	 *	\param savedInstanceState Bundle with object saved state.
	 *	\return None.
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_charts);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		agent = (AgentData) this.getIntent().getSerializableExtra(AGENT);
//		host = this.getIntent().getStringExtra(AdminTools.HOST);
//		port = this.getIntent().getIntExtra(AdminTools.PORT, 0);
//		key = this.getIntent().getStringExtra(AdminTools.KEY);
//		interval = this.getIntent().getIntExtra(AdminTools.INTERVAL, 1000);

		connectionSettings = getSharedPreferences(AdminTools.CONN_PREFS_NAME,
				MODE_MULTI_PROCESS);

		Log.d("qwe", "StatsActivity.onCreate()");

		host = connectionSettings.getString(AdminTools.HOST, "");
		port = Integer.parseInt(connectionSettings.getString(AdminTools.PORT,
				""));
		key = connectionSettings.getString(AdminTools.KEY, "");
		interval = connectionSettings.getInt(AdminTools.INTERVAL, 2000);
		
		
		getActionBar().setTitle("ChartsActivity\t" + agent.getName());

		surface = (ChartsSurface) findViewById(R.id.surfaceView_charts);
		timeStart = (Button) findViewById(R.id.button_date_start);
		timeEnd = (Button) findViewById(R.id.button_date_end);
		// settings = (Button) findViewById(R.id.button_charts_settings);
		// submit = (Button) findViewById(R.id.button_submit_charts);
		typeButton = (Button) findViewById(R.id.button_charts_settings);
		timeNew = (TextView) findViewById(R.id.textView_charts_time_new);
		timeOld = (TextView) findViewById(R.id.textView_charts_time_old);
		valueMax = (TextView) findViewById(R.id.textView_charts_value_max);
		valueMin = (TextView) findViewById(R.id.textView_charts_value_min);

		String today = "", tomorrow = "";
		Calendar c = Calendar.getInstance();
		c.set(Calendar.MINUTE, 0);
		c.set(Calendar.HOUR_OF_DAY, 0);
		c.set(Calendar.SECOND, 0);
		c.set(Calendar.MILLISECOND, 0);
		Log.d("qwe", "startDate " + c);
		statsRequest.setStartDate((int) (c.getTime().getTime() / 1000));
		Log.d("qwe", "" + (int) (c.getTime().getTime() / 1000));
		today += c.get(Calendar.DAY_OF_MONTH) + "-";
		today += (c.get(Calendar.MONTH)+1) + "-";
		today += c.get(Calendar.YEAR);
		c.add(Calendar.DAY_OF_YEAR, 1);
		statsRequest.setEndDate((int) (c.getTime().getTime() / 1000));
		tomorrow += c.get(Calendar.DAY_OF_MONTH) + "-";
		tomorrow += (c.get(Calendar.MONTH)+1) + "-";
		tomorrow += c.get(Calendar.YEAR);

		timeStart.setText(today);
		timeEnd.setText(tomorrow);

		Log.d("qwe",
				"charts agent ID " + agent.getId() + "name " + agent.getName());
		statsRequest.setAgentId(agent.getId());
		statsRequest.setDataType(PacketStatsRequest.TEMP);
		statsRequest.setDiskName("");

		isServiceBinded = bindService(
				new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
		
		setResult(RESULT_OK);
	}
	/**
	 *	\fn protected void onDestroy()
	 *	\brief Execute on activity destruction.
	 *	\return None.
	 */
	@Override
	protected void onDestroy() {
		if (isServiceBinded) {
			unbindService(this);
			isServiceBinded = false;
			serviceMessenger = null;
		}
		super.onDestroy();
	}
	/**
	 *	\fn protected void onStop()
	 *	\brief Execute on activity stop event.
	 *	\return None.
	 */
	@Override
	protected void onStop() {
		super.onStop();
	}
	/**
	 *	\fn protected void onResume()
	 *	\brief Execute on activity reasume event.
	 *	\return None.
	 */
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
	/**
	 *	\fn public void handleMessage(Message msg)
	 *	\brief Handle incoming Message.
	 *	\param msg Incoming Message.
	 *	\return None.
	 */
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
			ArrayList<Float> points = psr.getPoints();
			if(points.size() == 0){
				Toast.makeText(getApplicationContext(), getString(R.string.charts_size_zero), Toast.LENGTH_SHORT).show();
			}
			else{
				surface.setData(points, statsRequest.getDataType());
			}
			break;
		default:
			// sendMessageToService(ConnectionService.STOP);
			break;
		}
	}
	/**
	 *	\fn public void onServiceConnected(ComponentName name, IBinder service)
	 *	\brief Execute on service connection establish event.
	 *	\param name Component name.
	 *	\param service Service binder.
	 *	\return None.
	 */
	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		serviceMessenger = new Messenger(service);
		sendMessageToService(ConnectionService.GET_MESSENGER);
		sendMessageToService(ConnectionService.CONNECT, host, port, key,
				interval);
		sendMessageToService(ConnectionService.STOP);
		submit(null);
	}
	/**
	 *	\fn public void onServiceDisconnected(ComponentName name)
	 *	\brief Execute on service disconnected event.
	 *	\param name Component name.
	 *	\return None.
	 */
	@Override
	public void onServiceDisconnected(ComponentName name) {
		Log.d("qwe", "ChartsActivity.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
	}
	/**
	 *	\fn public void submit(View view)
	 *	\brief Submit given view. Do something.
	 *	\param view Submitted view.
	 *	\return None.
	 */
	public void submit(View view) {
		Log.d("qwe", "ChartsActivity.submit()");

		surface.post(new Runnable() {
			@Override
			public void run() {
				while (surface.getWidth() == 0)
					;
				statsRequest.setPoints((short) (surface.getWidth() / ChartsSurface.ACCURACY));
				lastSend = statsRequest;
				sendMessageToService(ConnectionService.STATS_REQUEST,
						statsRequest);
			}
		});

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
	/**
	 *	\fn public void setAxis(int minVal, int maxVal)
	 *	\brief Set charts axis.
	 *	\param minVal Minimal axis value.
	 *	\param maxVal Maximum axis value.
	 *	\return None.
	 */
	public void setAxis(int minVal, int maxVal) {
		Log.d("qwe", "ChartsActivity.setAxis()");
		String start = "", end = "";
		Calendar c = Calendar.getInstance();

		c.setTimeInMillis((long) lastSend.getEndDate() * 1000);
		end += c.get(Calendar.DAY_OF_MONTH) + "-";
		end += (c.get(Calendar.MONTH)+1) + "-";
		end += c.get(Calendar.YEAR);
		timeNew.setText(end);

		c.setTimeInMillis((long) lastSend.getStartDate() * 1000);
		start += c.get(Calendar.DAY_OF_MONTH) + "-";
		start += (c.get(Calendar.MONTH)+1) + "-";
		start += c.get(Calendar.YEAR);
		timeOld.setText(start);

		valueMax.setText("" + maxVal);
		valueMin.setText("" + minVal);

		String info = "";
		switch (lastSend.getDataType()) {
		case PacketStatsRequest.CPU:
			info += this.getString(R.string.cpu_chart);
			break;
		case PacketStatsRequest.DISK:
			info += this.getString(R.string.disk_chart);
			break;
		case PacketStatsRequest.RAM:
			info += this.getString(R.string.ram_chart);
			break;
		case PacketStatsRequest.TEMP:
			info += this.getString(R.string.temp_chart);
			break;
		default:
			break;
		}
		typeButton.setText(info);
	}
	/**
	 *	\fn public void updateDate()
	 *	\brief Update charts date.
	 *	\return None.
	 */
	public void updateDate() {
		Log.d("qwe", "ChartsActivity.updateDate()");
		String start = "", end = "";
		Calendar c = Calendar.getInstance();

		c.setTimeInMillis((long) statsRequest.getEndDate() * 1000);
		end += c.get(Calendar.DAY_OF_MONTH) + "-";
		end += (c.get(Calendar.MONTH)+1) + "-";
		end += c.get(Calendar.YEAR);
		timeEnd.setText(end);

		c.setTimeInMillis((long) statsRequest.getStartDate() * 1000);
		start += c.get(Calendar.DAY_OF_MONTH) + "-";
		start += (c.get(Calendar.MONTH)+1) + "-";
		start += c.get(Calendar.YEAR);
		timeStart.setText(start);

		
		submit(null);
	}
	/**
	 *	\fn public void setDate(View view)
	 *	\brief Set charts date.
	 *	\param view Current View.
	 *	\return None.
	 */
	public void setDate(View view) {
		Log.d("qwe", "ChartsActivity.setDate()");
		DialogFragment dateFragment = null;
		if (view.getId() == R.id.button_date_start) {
			dateFragment = new DatePickerFragment(statsRequest, this, true);
		} else {
			dateFragment = new DatePickerFragment(statsRequest, this, false);
		}
		dateFragment.show(getFragmentManager(), "datePicker");
	}
	/**
	 *	\fn public void setDataType(View view)
	 *	\brief Set charts data type.
	 *	\param view Current View.
	 *	\return None.
	 */
	public void setDataType(View view) {
		Log.d("qwe", "ChartsActivity.setDataType()");
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
/**
 *	\class DatePickerFragment
 *	\brief Date picker dialog.
 */
class DatePickerFragment extends DialogFragment implements
		DatePickerDialog.OnDateSetListener {
	private boolean isStartTime;
	private PacketStatsRequest packet;
	private ChartsActivity parent;
	private boolean wasSet;

	/**
	 *	\fn public DatePickerFragment(PacketStatsRequest packet, ChartsActivity parent, boolean isStartTime)
	 *	\brief Constructor, set components data.
	 *	\param packet Packet stats request.
	 *	\param parent Parent activity.
	 *	\parma isStartTime Flag if its start time.
	 */
	public DatePickerFragment(PacketStatsRequest packet , ChartsActivity parent,
			boolean isStartTime) {
		super();
		Log.d("qwe", "DatePicker()");
		this.isStartTime = isStartTime;
		this.packet = packet;
		this.parent = parent;
		this.wasSet = false;
	}
	/**
	 *	\fn public Dialog onCreateDialog(Bundle savedInstanceState)
	 *	\brief Execute on creation of dialog.
	 *	\param savedInstanceState Bundle with object saved state.
	 *	\return Created dialog.
	 */
	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState) {
		Log.d("qwe", "DatePicker.onCreateDialog()");
		final Calendar c = Calendar.getInstance();
		if (isStartTime) {
			c.setTimeInMillis((long) packet.getStartDate() * 1000);
		} else {
			c.setTimeInMillis((long) packet.getEndDate() * 1000);
		}
		int year = c.get(Calendar.YEAR);
		int month = c.get(Calendar.MONTH);
		int day = c.get(Calendar.DAY_OF_MONTH);

		return new DatePickerDialog(parent, this, year, month, day);
	}
	/**
	 *	\fn public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth)
	 *	\brief Execute on date set event.
	 *	\param view Date picker instance.
	 *	\param year Selected year.
	 *	\param monthOfYear Selected month of year.
	 *	\param dayOfMonth Selected day of month.
	 *	\return None.
	 */
	@Override
	public void onDateSet(DatePicker view, int year, int monthOfYear,
			int dayOfMonth) {
		if (wasSet)
			return;
		wasSet = true;
		Calendar currentDate = Calendar.getInstance();
		currentDate.set(Calendar.HOUR_OF_DAY, 0);
		currentDate.set(Calendar.MINUTE, 0);
		currentDate.set(Calendar.SECOND, 0);
		currentDate.set(Calendar.MILLISECOND, 0);
		Calendar tomorrow = Calendar.getInstance();
		tomorrow.setTimeInMillis(currentDate.getTimeInMillis());
		tomorrow.add(Calendar.DAY_OF_MONTH, 1);
		Calendar monthAgo = Calendar.getInstance();
		monthAgo.setTimeInMillis(currentDate.getTimeInMillis());
		monthAgo.add(Calendar.MONTH, -1);

		final Calendar c = Calendar.getInstance();
		c.set(Calendar.YEAR, year);
		c.set(Calendar.MONTH, monthOfYear);
		c.set(Calendar.DAY_OF_MONTH, dayOfMonth);
		c.set(Calendar.HOUR_OF_DAY, 0);
		c.set(Calendar.MINUTE, 0);
		c.set(Calendar.SECOND, 0);
		c.set(Calendar.MILLISECOND, 0);

		Log.d("qwe", "DatePicker.onDateSet() before" + c);
		if (isStartTime) {
			if (c.getTimeInMillis() < monthAgo.getTimeInMillis()) {
				packet.setStartDate((int) (monthAgo.getTimeInMillis() / 1000));
			} else if ((int) (c.getTimeInMillis() / 1000) >= packet
					.getEndDate()) {
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
					.getStartDate()) {
				long time = (long) packet.getStartDate();
				time *= 1000;
				c.setTimeInMillis(time);
				c.add(Calendar.DAY_OF_MONTH, 1);
				packet.setEndDate((int) (c.getTimeInMillis() / 1000));
			} else
				packet.setEndDate((int) (c.getTimeInMillis() / 1000));
		}
		Log.d("qwe", "DatePicker.onDateSet() after" + c);
		parent.updateDate();
	}
}
