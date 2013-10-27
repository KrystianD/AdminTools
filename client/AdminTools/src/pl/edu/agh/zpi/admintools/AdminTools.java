/**
TODO obsługa headerTimeout - teraz zawiesza się przy próbie zalogowania

 **/

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
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
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
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.Toast;

public class AdminTools extends Activity implements ServiceConnection, Handable {
	public static final String HOST = "pl.edu.agh.zpi.admintools.host";
	public static final String PORT = "pl.edu.agh.zpi.admintools.port";
	public static final String KEY = "pl.edu.agh.zpi.admintools.key";
	public static final String INTERVAL = "pl.edu.agh.zpi.admintools.interval";
	public static final String CONN_PREFS_NAME = "connection_prefs_file";

	
	public static final int STATS_ACTIVITY_CODE = 1;
	public static final int CHARTS_ACTIVITY_CODE = 2;

	private SharedPreferences connectionSettings;
	
	private EditText editTextHost;
	private EditText editTextPort;
	private Button buttonConnect;
	private ProgressBar progressBar;

	private boolean isServiceBinded;

	private Messenger serviceMessenger;
	private Messenger activityMessenger = new Messenger(new IncomingHandler(
			this));

	private String key;
	private int interval;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_admin_tools);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		connectionSettings = getSharedPreferences(
					CONN_PREFS_NAME, MODE_PRIVATE);
		
		editTextHost = (EditText) findViewById(R.id.editText_IP);
		editTextPort = (EditText) findViewById(R.id.editText_Port);
		buttonConnect = (Button) findViewById(R.id.button_connect);
		progressBar = (ProgressBar) findViewById(R.id.progressBar_connection);

		editTextHost.setText(connectionSettings.getString(HOST, ""));
		editTextPort.setText(connectionSettings.getString(PORT, ""));

		key = connectionSettings.getString(KEY, "");
		interval = connectionSettings.getInt(INTERVAL, 1000);
		
		isServiceBinded = bindService(
				new Intent(this, ConnectionService.class), this,
				Context.BIND_AUTO_CREATE);
	}

	public void onConnect(View view) throws InterruptedException {
		setConnectionUI(true);

		if (!validateConnection()) {
			setConnectionUI(false);
			return;
		}

		saveConnection();

		if (serviceMessenger == null)
			return;
		Message m = Message.obtain(null, ConnectionService.CONNECT);
		Bundle b = new Bundle();
		b.putInt(PORT, Integer.parseInt(editTextPort.getText().toString()));
		b.putString(HOST, editTextHost.getText().toString());
		b.putString(KEY, key);
		b.putInt(INTERVAL, interval);
		m.setData(b);
		try {
			serviceMessenger.send(m);
		} catch (RemoteException e) {
			e.printStackTrace();
		}

	}

	private boolean validateConnection() {
		if(!connectionSettings.contains(KEY))
			return false;
		if (!checkNetworkStatus())
			return false;
		if (!validatePort())
			return false;
		if (!validateAddress())
			return false;
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
		showAlertToast(getString(R.string.network_status_error));
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
		Log.d("qwe", "AdminTools.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
	}

	@Override
	protected void onDestroy() {
		Log.d("qwe", "AdminTools.onDestroy()" + isServiceBinded);
		if (isServiceBinded) {
			unbindService(this);
			isServiceBinded = false;
			serviceMessenger = null;
		}
		super.onDestroy();
	}

	@Override
	protected void onResume() {
		if (serviceMessenger != null) {
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
		switch (msg.what) {
		case ConnectionTask.CONNECTED:
			Intent intent = new Intent(this, StatsActivity.class);
			intent.putExtra(PORT,
					Integer.parseInt(editTextPort.getText().toString()));
			intent.putExtra(HOST, editTextHost.getText().toString());
			
			startActivityForResult(intent, STATS_ACTIVITY_CODE);
			setConnectionUI(false);
			break;
		default:
			break;
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == STATS_ACTIVITY_CODE) {
			if (resultCode == RESULT_CANCELED) {
				Toast t = Toast.makeText(this, R.string.network_error,
						Toast.LENGTH_SHORT);
				t.show();
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.admin_tools, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case (R.id.action_put_key):
			LayoutInflater inflater = this.getLayoutInflater();
			final View dialogView = inflater.inflate(R.layout.dialog_put_key,
					null);
			final AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setTitle(R.string.dialog_put_key_title);
			builder.setView(dialogView);
			builder.setPositiveButton(R.string.ok,
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							EditText editText = (EditText) dialogView
									.findViewById(R.id.editText_dialog_put_key);
							String key = editText.getText().toString();
							SharedPreferences.Editor editor = connectionSettings
									.edit();
							editor.putString(KEY, key);
							editor.commit();
						}
					});
			builder.setNegativeButton(R.string.cancel,
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
						}
					});
			AlertDialog dialog = builder.create();
			dialog.show();

			Button positive = dialog.getButton(AlertDialog.BUTTON_POSITIVE);
			positive.setEnabled(false);
			KeyTextWatcher watcher = new KeyTextWatcher(positive);
			EditText editText = (EditText) dialogView
					.findViewById(R.id.editText_dialog_put_key);
			editText.addTextChangedListener(watcher);
			editText.setText(connectionSettings.getString(KEY, ""));
			break;
		default:
			break;
		}
		return true;
	}
}

/************************/
/** additional classes **/
/************************/

class KeyTextWatcher implements TextWatcher {
	private Button positive;

	public KeyTextWatcher(Button positive) {
		super();
		this.positive = positive;
	}

	@Override
	public void afterTextChanged(Editable s) {
		if (s.toString().length() != 16) {
			positive.setEnabled(false);
		}
		else{
			positive.setEnabled(true);
		}
	}

	@Override
	public void beforeTextChanged(CharSequence s, int start, int count,
			int after) {
	}

	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count) {
	}
}