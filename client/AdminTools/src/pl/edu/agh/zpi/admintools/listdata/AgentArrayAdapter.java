package pl.edu.agh.zpi.admintools.listdata;

import java.util.ArrayList;
import java.util.Arrays;

import pl.edu.agh.zpi.admintools.R;
import pl.edu.agh.zpi.admintools.StatsActivity;
import pl.edu.agh.zpi.admintools.sensors.AgentData;
import pl.edu.agh.zpi.admintools.sensors.DiskUsageData;
import pl.edu.agh.zpi.admintools.sensors.SensorsData;
import pl.edu.agh.zpi.admintools.sensors.ServiceData;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TableRow.LayoutParams;
import android.widget.TextView;
import android.widget.ToggleButton;

/**
 *	\class AgentArrayAdapter
 *	\brief Adapter for AgentData arrays for easy agent data visualization.
 */
public class AgentArrayAdapter extends ArrayAdapter<AgentData> {
	//! App environment context.
	private final Context context;
	//! Memory divider (for byte to gigabyte calculations).
	private final double divider = 1024 * 1024 * 1024; // GB
<<<<<<< HEAD
	private final double tempAlertLevel = 85;
=======
	//! Temperature alert threshold.
	private final double tempAlertLevel = 65;
	//! HDD usage alert percent threshold.
>>>>>>> 079f7010a30cf4b534ca5febc24be1a237ffd890
	private final double HDDAlertLevel = 0.95;

	public AgentArrayAdapter(Context context) {
		super(context, R.layout.list_agents_stats, new ArrayList<AgentData>());
		this.context = context;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		if (convertView == null) {
			LayoutInflater inflater = (LayoutInflater) context
					.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			convertView = inflater.inflate(R.layout.list_agents_stats, parent,
					false);
		}
		AgentData agent = this.getItem(position);
		SensorsData sensors = agent.getData();
		ArrayList<DiskUsageData> diskUsage = sensors.getDiskUsages();
		ArrayList<ServiceData> serviceData = sensors.getServices();

		ToggleButton toggleButtonName = (ToggleButton) convertView
				.findViewById(R.id.toggleButton_list_agent_name);
		TextView temp = (TextView) convertView
				.findViewById(R.id.textView_list_temp_data);
		TextView cpu = (TextView) convertView
				.findViewById(R.id.textView_list_cpu_data);
		TextView ram = (TextView) convertView
				.findViewById(R.id.textView_list_ram_data);
		TextView uptime = (TextView) convertView
				.findViewById(R.id.textView_list_uptime_data);
		TableLayout servicesTab = (TableLayout) convertView
				.findViewById(R.id.tableLayout_list_services);
		TableLayout disksTab = (TableLayout) convertView
				.findViewById(R.id.tableLayout_list_disks);
		ToggleButton toggleButtonDisks = (ToggleButton) convertView
				.findViewById(R.id.toggleButton_list_disks);
		ToggleButton toggleButtonServices = (ToggleButton) convertView
				.findViewById(R.id.toggleButton_list_services);
		LinearLayout agentLayout = (LinearLayout) convertView
				.findViewById(R.id.linearLayout_list_agent_data);

		addDisksContent(disksTab, diskUsage, agent.getName());
		addServicesContent(servicesTab, serviceData);

		if (toggleButtonServices.isChecked()) {
			servicesTab.setVisibility(View.VISIBLE);
		} else {
			servicesTab.setVisibility(View.GONE);
		}

		if (toggleButtonDisks.isChecked()) {
			disksTab.setVisibility(View.VISIBLE);
		} else {
			disksTab.setVisibility(View.GONE);
		}

		if (toggleButtonName.isChecked()) {
			agentLayout.setVisibility(View.VISIBLE);
		} else {
			agentLayout.setVisibility(View.GONE);
		}

		toggleButtonName.setText("" + agent.getName());
		toggleButtonName.setTextOn("" + agent.getName());
		toggleButtonName.setTextOff("" + agent.getName());

		if (sensors.isTempValid()) {
			if (sensors.getTemp() > tempAlertLevel) {
				createAlertNotification();
				temp.setTextColor(Color.RED);
			} else {
				temp.setTextColor(Color.BLACK);
			}
			temp.setText(String.format("%.2f°C", sensors.getTemp()));
		} else {
			temp.setText(context.getString(R.string.unknown));
			temp.setTextColor(Color.RED);
		}

		cpu.setText(String.format("%.0f%%", sensors.getCpuUsage() * 100));

		double freeRamGB = sensors.getFreeRam() / divider;
		double totalRamGB = sensors.getTotalRam() / divider;
		ram.setText(String.format("%.0f%% (%.2fGB/%.2fGB)", freeRamGB
				/ totalRamGB * 100, freeRamGB, totalRamGB));

		int up = sensors.getUptime();
		int days = up / 86400; // 60*60*24
		int hours = (up / 3600) % 24; // 60*60
		int minutes = (up / 60) % 60;
		int seconds = up % 60;
		uptime.setText(String.format("%dD %dH %dM %dS", days, hours, minutes,
				seconds));

		Button settingsButton = (Button) convertView
				.findViewById(R.id.button_list_settings);
		if (settingsButton != null) { // why do I even need that? :(
										// everything else works!
			// settingsButton.setId(agent.getId());
			settingsButton.setTag(agent.getId());
		}

		if ((position % 2) == 0) {
			convertView.setBackgroundColor(Color.rgb(200, 200, 200));
		} else {
			convertView.setBackgroundColor(Color.rgb(230, 230, 230));
		}

		return convertView;
	}

	private void addServicesContent(TableLayout layout,
			ArrayList<ServiceData> data) {
		TableRow.LayoutParams params = new TableRow.LayoutParams(
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
		TableRow row;
		TextView name, status;

		layout.removeAllViews();

		for (ServiceData sd : data) {
			name = new TextView(context);
			status = new TextView(context);

			name.setText(sd.getName());
			String availability;
			int color;
			if (sd.isAvailable()) {
				availability = context.getString(R.string.available);
				color = Color.GREEN;
			} else {
				availability = context.getString(R.string.inavailable);
				color = Color.RED;
			}
			status.setText("\t" + availability);
			status.setTextColor(color);

			row = new TableRow(context);
			row.setLayoutParams(params);
			row.addView(name);
			row.addView(status);

			layout.addView(row);
		}
	}

	private void addDisksContent(TableLayout layout,
			ArrayList<DiskUsageData> data, String agentName) {
		TableRow.LayoutParams params = new TableRow.LayoutParams(
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
		TableRow row;
		TextView name, usage;

		layout.removeAllViews();

		for (DiskUsageData dud : data) {
			double totalSpaceGB = dud.getTotalSpace() / divider;
			double usedSpaceGB = dud.getUsedSpace() / divider;

			name = new TextView(context);
			// name.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT,
			// LayoutParams.WRAP_CONTENT, 1f));
			usage = new TextView(context);

			// cutting strings into lines
			String nameString = "";
			String tmp[] = dud.getName().split("-");
			ArrayList<String> parts = new ArrayList<String>();
			for (int i = 0; i < tmp.length; i++) {
				if (i + 1 < tmp.length)
					tmp[i] += "-";
				String[] tmpSlash = tmp[i].split("/");
				if (tmpSlash.length > 1) {
					for (int j = 0; j < tmpSlash.length; j++) {
						if (!tmpSlash[j].contains("-"))
							tmpSlash[j] += "/";
					}
				}
				parts.addAll(Arrays.asList(tmpSlash));
			}
			String tail = "";
			for (String s : parts) {
				if (tail.equals("")) {
					tail += s;
				} else if (s.length() + tail.length() <= 25) {
					tail += s;
				} else {
					nameString += tail;
					tail = '\n' + s;
				}
			}
			if (!tail.equals("" + '\n'))
				nameString += tail;

			name.setText(nameString);
			usage.setText(String.format("\t%.1fGB/%.1fGB", usedSpaceGB,
					totalSpaceGB));

			row = new TableRow(context);
			row.setLayoutParams(params);
			row.addView(name);
			row.addView(usage);

			layout.addView(row);

			if (usedSpaceGB / totalSpaceGB > HDDAlertLevel) {
				createAlertNotification();
				name.setTextColor(Color.RED);
				usage.setTextColor(Color.RED);
			} else {
				name.setTextColor(Color.BLACK);
				usage.setTextColor(Color.BLACK);
			}
		}
	}

	private void createAlertNotification() {
		Intent resultIntent = new Intent(context, StatsActivity.class);
		TaskStackBuilder stackBuilder = TaskStackBuilder.create(context);
		// Adds the back stack
		stackBuilder.addParentStack(StatsActivity.class);
		// Adds the Intent to the top of the stack
		stackBuilder.addNextIntent(resultIntent);
		// Gets a PendingIntent containing the entire back stack
		PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0,
				PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(
				context);
		builder.setContentIntent(resultPendingIntent)
				.setContentTitle("Admin Tools")
				.setContentText("Something is wrong!")
				.setAutoCancel(true)
				.setSmallIcon(R.drawable.ic_launcher);
		Notification noti = builder.build();
		noti.flags |= Notification.FLAG_AUTO_CANCEL;
		NotificationManager mNotificationManager = (NotificationManager) context
				.getSystemService(Context.NOTIFICATION_SERVICE);
		mNotificationManager.notify(1, noti);
	}
}