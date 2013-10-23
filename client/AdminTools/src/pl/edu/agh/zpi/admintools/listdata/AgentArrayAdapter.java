package pl.edu.agh.zpi.admintools.listdata;

import java.util.ArrayList;

import pl.edu.agh.zpi.admintools.R;
import pl.edu.agh.zpi.admintools.sensors.AgentData;
import pl.edu.agh.zpi.admintools.sensors.DiskUsageData;
import pl.edu.agh.zpi.admintools.sensors.SensorsData;
import pl.edu.agh.zpi.admintools.sensors.ServiceData;
import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TableRow.LayoutParams;
import android.widget.TextView;
import android.widget.ToggleButton;

public class AgentArrayAdapter extends ArrayAdapter<AgentData> {
	private final Context context;
	private final double divider = 1024 * 1024 * 1024; // GB

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

		TextView name = (TextView) convertView
				.findViewById(R.id.textView_list_agent_name);
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
		ToggleButton toggleButtonDisks = (ToggleButton)convertView.findViewById(R.id.toggleButton_list_disks);
		ToggleButton toggleButtonServices = (ToggleButton)convertView.findViewById(R.id.toggleButton_list_services);
		
		if(toggleButtonServices.isChecked()){
			addServicesContent(servicesTab, serviceData);
			servicesTab.setVisibility(View.VISIBLE);
		}else{
			servicesTab.setVisibility(View.GONE);
		}
		if(toggleButtonDisks.isChecked()){
			addDisksContent(disksTab, diskUsage);
			disksTab.setVisibility(View.VISIBLE);
		}else{
			disksTab.setVisibility(View.GONE);
		}
		
		name.setText("" + agent.getName());

		temp.setText(sensors.getTemp() + "°C");

		cpu.setText(String.format("%.0f%%", sensors.getCpuUsage() * 10));

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
		
		convertView.setId(agent.getId());
		
		return convertView;
	}

	private void addServicesContent(TableLayout layout,
			ArrayList<ServiceData> data) {
		TableRow.LayoutParams params = new TableRow.LayoutParams(
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
		TableRow row;
		TextView name,status;

		
		layout.removeAllViews();
		
		for (ServiceData sd : data) {
			name = new TextView(context);
			status = new TextView(context);
			
			name.setText(sd.getName());
			String availability;
			int color;
			if(sd.isAvailable()){
				availability = context.getString(R.string.available);
				color = Color.GREEN;
			}else{
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
			ArrayList<DiskUsageData> data) {
		TableRow.LayoutParams params = new TableRow.LayoutParams(
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
		TableRow row;
		TextView name, usage;

		layout.removeAllViews();
		
		for (DiskUsageData dud : data) {
			double totalSpaceGB = dud.getTotalSpace()/divider;
			double usedSpaceGB = dud.getUsedSpace()/divider;
			
			name = new TextView(context);
			usage = new TextView(context);

			name.setText(dud.getName());
			usage.setText(String.format("\t%.1fGB/%.1fGB", usedSpaceGB,totalSpaceGB));

			row = new TableRow(context);
			row.setLayoutParams(params);
			row.addView(name);
			row.addView(usage);

			layout.addView(row);
		}
	}
}