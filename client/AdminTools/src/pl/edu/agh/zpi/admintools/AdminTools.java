package pl.edu.agh.zpi.admintools;

import java.net.InetAddress;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class AdminTools extends Activity {
	private final String IP = "pl.edu.agh.zpi.admintools.ip";
	private final String PORT = "pl.edu.agh.zpi.admintools.port";
	private final String CONN_PREFS_NAME = "connection_prefs_file";
	
	private EditText editTextIP;
	private EditText editTextPort;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_admin_tools);
		
		editTextIP = (EditText)findViewById(R.id.editText_IP);
		editTextPort = (EditText)findViewById(R.id.editText_Port);
		
		SharedPreferences connectionPreferences = getSharedPreferences(CONN_PREFS_NAME, MODE_PRIVATE);
		editTextIP.setText(connectionPreferences.getString(IP, ""));
		editTextPort.setText(connectionPreferences.getString(PORT, ""));

	}

	public void onConnect(View view){
		if(!checkNetworkStatus())
			return;
		if(!validatePort())
			return;
		if(!validateAddress())
			return;
		
		saveConnection();
		
		int port = Integer.parseInt(editTextPort.getText().toString());
		String address = editTextIP.getText().toString();
		
		
		
		
		
		
		Log.e("qwe", "clicked!");
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.admin_tools, menu);
		return true;
	}
	
	private void showAlertToast(String msg){
		Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
	}
	
	private boolean checkNetworkStatus(){
		ConnectivityManager connMgr = (ConnectivityManager) 
		        getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = connMgr.getActiveNetworkInfo();
		
		if (networkInfo != null && networkInfo.isConnected()) {
			showAlertToast(getString(R.string.network_error));
			return true;
		}
		return false;
	}
	
	private boolean validatePort(){
		try{
			int port = Integer.parseInt(editTextPort.getText().toString());
			if(port < 1){
				showAlertToast(getString(R.string.port_too_small));
				return false;
			}
			else if(port > 65535){
				showAlertToast(getString(R.string.port_too_big));
				return false;
			}
		}catch(NumberFormatException e){
			showAlertToast(getString(R.string.port_wrong_format));
			return false;
		}
		return true;
	}
	
	private boolean validateAddress(){
		final String address = editTextIP.getText().toString();
		ExecutorService executor = Executors.newSingleThreadExecutor();
		Future<Boolean> future = executor.submit(new Callable<Boolean>() {
			         public Boolean call() {
			           try{
			        	   InetAddress.getByName(address);
			        	   Log.e("qwe","ok");
			        	   return true;
			           }catch(Exception e){ 
			        	 return false;
			           }
			         }
			 });
		try {
			boolean result = future.get(500,TimeUnit.MILLISECONDS);
			if(result==false){
				showAlertToast(getString(R.string.address_unknown));
				return false;
			}
		} catch (Exception e){
			showAlertToast(getString(R.string.network_error));
			return false;
		}
		return true;
	}
	
	private void saveConnection(){
		SharedPreferences connectionSettings = getSharedPreferences(CONN_PREFS_NAME, MODE_PRIVATE);
		SharedPreferences.Editor editor = connectionSettings.edit();
		editor.putString(IP,editTextIP.getText().toString());
		editor.putString(PORT,editTextPort.getText().toString());
		editor.commit();
	}
}