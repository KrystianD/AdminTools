package pl.edu.agh.zpi.admintools;

import java.net.InetAddress;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import pl.edu.agh.zpi.admintools.connection.ConnectionTask;
import pl.edu.agh.zpi.admintools.utils.Handable;
import pl.edu.agh.zpi.admintools.utils.IncomingHandler;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.Toast;

public class AdminTools extends Activity implements ServiceConnection, Handable {
	public static final String HOST = "pl.edu.agh.zpi.admintools.host";
	public static final String PORT = "pl.edu.agh.zpi.admintools.port";

	private static final String CONN_PREFS_NAME = "connection_prefs_file";

	private EditText editTextHost;
	private EditText editTextPort;
	private Button buttonConnect;
	private ProgressBar progressBar;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(this));
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_admin_tools);
		
		editTextHost = (EditText) findViewById(R.id.editText_IP);
		editTextPort = (EditText) findViewById(R.id.editText_Port);
		buttonConnect = (Button) findViewById(R.id.button_connect);
		progressBar = (ProgressBar) findViewById(R.id.progressBar_connection);

		SharedPreferences connectionPreferences = getSharedPreferences(
				CONN_PREFS_NAME, MODE_PRIVATE);
		editTextHost.setText(connectionPreferences.getString(HOST, ""));
		editTextPort.setText(connectionPreferences.getString(PORT, ""));

		bindService(new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
	}

	public void onConnect(View view) throws InterruptedException {
		setConnectionUI(true);
		
		if (!checkNetworkStatus())
			return;
		if (!validatePort())
			return;
		if (!validateAddress())
			return;

		saveConnection();

		if (serviceMessenger == null)
			return;
		Message m = Message.obtain(null, ConnectionService.CONNECT);
		try {
			serviceMessenger.send(m);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		
	}

	public void generateKey(View view) {
		if (serviceMessenger == null)
			return;

		Message m = Message.obtain(null, ConnectionService.GENERATE_KEY);
		try {
			serviceMessenger.send(m);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.admin_tools, menu);
		return true;
	}

	private void showAlertToast(String msg) {
		Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
	}

	private boolean checkNetworkStatus() {
		ConnectivityManager connMgr = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = connMgr.getActiveNetworkInfo();

		if (networkInfo != null && networkInfo.isConnected()) {
			return true;
		}
		return false;
	}

	private boolean validatePort() {
		try {
			int port = Integer.parseInt(editTextPort.getText().toString());
			if (port < 1) {
				showAlertToast(getString(R.string.port_too_small));
				return false;
			} else if (port > 65535) {
				showAlertToast(getString(R.string.port_too_big));
				return false;
			}
		} catch (NumberFormatException e) {
			showAlertToast(getString(R.string.port_wrong_format));
			return false;
		}
		return true;
	}

	private boolean validateAddress() {
		final String address = editTextHost.getText().toString();
		ExecutorService executor = Executors.newSingleThreadExecutor();
		Future<Boolean> future = executor.submit(new Callable<Boolean>() {
			public Boolean call() {
				try {
					InetAddress.getByName(address);
					return true;
				} catch (Exception e) {
					return false;
				}
			}
		});
		try {
			boolean result = future.get(500, TimeUnit.MILLISECONDS);
			if (result == false) {
				showAlertToast(getString(R.string.address_unknown));
				return false;
			}
		} catch (Exception e) {
			showAlertToast(getString(R.string.network_error));
			return false;
		}
		return true;
	}

	private void setConnectionUI(boolean connecting) {
		if (connecting) {
			buttonConnect.setEnabled(false);
			buttonConnect.setText(getString(R.string.connecting));
			progressBar.setVisibility(View.VISIBLE);
		} else {
			buttonConnect.setEnabled(true);
			buttonConnect.setText(getString(R.string.connect));
			progressBar.setVisibility(View.INVISIBLE);
		}
	}

	private void saveConnection() {
		SharedPreferences connectionSettings = getSharedPreferences(
				CONN_PREFS_NAME, MODE_PRIVATE);
		SharedPreferences.Editor editor = connectionSettings.edit();
		editor.putString(HOST, editTextHost.getText().toString());
		editor.putString(PORT, editTextPort.getText().toString());
		editor.commit();
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
	protected void onDestroy() {
		//Log.d("qwe", "unbinded AdminTools");
		try{
			unbindService(this);
		}catch(Exception e){}
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
		Log.d("qwe", "AdminTools handleMessage");
		switch(msg.what){
		case ConnectionTask.CONNECTED:
			Intent intent = new Intent(this, StatsActivity.class);
			startActivity(intent);
			setConnectionUI(false);
			break;
		default:
			break;
		}
	}
}