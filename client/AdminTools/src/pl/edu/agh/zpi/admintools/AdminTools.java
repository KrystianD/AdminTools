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

/**
 *	\class AdminTools
 *	\brief Main client activity, manage whole application.
 */
public class AdminTools extends Activity implements ServiceConnection, Handable {
	//! Host field key.
	public static final String HOST = "pl.edu.agh.zpi.admintools.host";
	//! Port field key.
	public static final String PORT = "pl.edu.agh.zpi.admintools.port";
	//! Authentication key field key.
	public static final String KEY = "pl.edu.agh.zpi.admintools.key";
	//! Time interval field key.
	public static final String INTERVAL = "pl.edu.agh.zpi.admintools.interval";
	//! Connection preferences name field key.
	public static final String CONN_PREFS_NAME = "connection_prefs_file";
	//! Network error field key.
	public static final String NETWORK_ERROR = "pl.edu.agh.zpi.admintools.network_error";

	//! Starting stats activity code.
	public static final int STATS_ACTIVITY_CODE = 1;
	//! Starting charts activity code.
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

	/**
	 *	\fn protected void onCreate(Bundle savedInstanceState)
	 *	\brief Execute on activity creation.
	 *	\param savedInstanceState Bundle with object saved state.
	 *	\return None.
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_admin_tools);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		connectionSettings = getSharedPreferences(
					CONN_PREFS_NAME, MODE_MULTI_PROCESS);

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
	/**
	 *	\fn public void onConnect(View view)
	 *	\brief Execute on establish connection.
	 *	\param view Current View.
	 *	\return None.
	 */
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
		Log.d("qwe","key "+key);
		b.putInt(INTERVAL, interval);
		m.setData(b);
		try {
			serviceMessenger.send(m);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		setConnectionUI(false);
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
		Message m = Message.obtain(null, ConnectionService.GET_MESSENGER);
		m.replyTo = activityMessenger;
		try {
			serviceMessenger.send(m);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}
	/**
	 *	\fn public void onServiceDisconnected(ComponentName name)
	 *	\brief Execute on service disconnected event.
	 *	\param name Component name.
	 *	\return None.
	 */
	@Override
	public void onServiceDisconnected(ComponentName name) {
		Log.d("qwe", "AdminTools.onServiceDisconnected()");
		serviceMessenger = null;
		isServiceBinded = false;
	}
	/**
	 *	\fn protected void onDestroy()
	 *	\brief Execute on activity destruction.
	 *	\return None.
	 */
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
	/**
	 *	\fn protected void onResume()
	 *	\brief Execute on activity reasume event.
	 *	\return None.
	 */
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
	/**
	 *	\fn public void handleMessage(Message msg)
	 *	\brief Handle incoming Message.
	 *	\param msg Incoming Message.
	 *	\return None.
	 */
	@Override
	public void handleMessage(Message msg) {
		Log.d("qwe", "AdminTools handleMessage");
		switch (msg.what) {
		case ConnectionTask.CONNECTED:
			Intent intent = new Intent(this, StatsActivity.class);
			intent.putExtra(PORT,
					Integer.parseInt(editTextPort.getText().toString()));
			intent.putExtra(HOST, editTextHost.getText().toString());
			intent.putExtra(KEY, key);

			startActivityForResult(intent, STATS_ACTIVITY_CODE);
			setConnectionUI(false);
			break;
		case ConnectionTask.CONNECTION_ERROR:
			Exception e = (Exception)msg.getData().get(NETWORK_ERROR);
			showAlertToast(e.getMessage());
			break;
		case ConnectionTask.AUTH_FAILED:
			showAlertToast(getString(R.string.auth_error));
			break;
		default:
			break;
		}
	}
	/**
	 *	\fn protected void onActivityResult(int requestCode, int resultCode, Intent data)
	 *	\brief Execute on activity result request.
	 *	\param requestCode Request code.
	 *	\param resultCode Result code.
	 *	\param data Intent data.
	 *	\return None.
	 */
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
	/**
	 *	\fn public boolean onCreateOptionsMenu(Menu menu)
	 *	\brief Execute on creation of options menu.
	 *	\param menu Menu data.
	 *	\return true.
	 */
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.admin_tools, menu);
		return true;
	}
	/**
	 *	\fn public boolean onOptionsItemSelected(MenuItem item)
	 *	\brief Execute on menu options item selection.
	 *	\param item Selected menu item.
	 *	\return If selected.
	 */
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
							String newKey = editText.getText().toString();
							SharedPreferences.Editor editor = connectionSettings
									.edit();
							editor.putString(KEY, newKey);
							editor.commit();
							key = newKey;
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
/**
 *	\class KeyTextWatcher
 *	\brief Text watcher for key text.
 */
class KeyTextWatcher implements TextWatcher {
	private Button positive;
	/**
	 *	\fn public KeyTextWatcher(Button positive)
	 *	\brief Constructor, sets positive Button.
	 *	\param positive Button to set.
	 */
	public KeyTextWatcher(Button positive) {
		super();
		this.positive = positive;
	}
	/**
	 *	\fn public void afterTextChanged(Editable s)
	 *	\brief Execute just after text change event.
	 *	\param s Editable component.
	 *	\return None.
	 */
	@Override
	public void afterTextChanged(Editable s) {
		if (s.toString().length() != 16) {
			positive.setEnabled(false);
		}
		else{
			positive.setEnabled(true);
		}
	}
	/**
	 *	\fn public void beforeTextChanged(CharSequence s, int start, int count, int after)
	 *	\brief Execute on just before text change event. Do nothing.
	 *	\param s New text.
	 *	\param start Start index.
	 *	\param count Count of text.
	 *	\param after After index.
	 *	\return None.
	 */
	@Override
	public void beforeTextChanged(CharSequence s, int start, int count,
			int after) {
	}
	/**
	 *	\fn public void onTextChanged(CharSequence s, int start, int before, int count)
	 *	\brief Execute on text change event. Do nothing.
	 *	\param s New text.
	 *	\param start Start index.
	 *	\param before Before index.
	 *	\param count Count of text.
	 *	\return None.
	 */
	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count) {
	}
}