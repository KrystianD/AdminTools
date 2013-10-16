package pl.edu.agh.zpi.admintools;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

public class ConnectionService extends IntentService {
	public static final String NAME = "pl.edu.agh.zpi.admintools.ConnectionService";
	private boolean connected;
	private boolean endConnection = false;
	private Socket socket;
	private InputStream input;
	private OutputStream output;
	
	public ConnectionService() {
		super(NAME);
	}

	@Override
	protected void onHandleIntent(Intent intent) {
		String host = intent.getStringExtra(AdminTools.HOST);
		int port = intent.getIntExtra(AdminTools.PORT, -1);
		
		try{
			InetAddress address = InetAddress.getByName(host);
			socket = new Socket(address,port);
			connected = true;
			input = socket.getInputStream();
			output = socket.getOutputStream();
			
			while(!endConnection){
				Thread.sleep(500);
				Log.d("qwe","żyję!");
			}
		
		}catch(Exception e){
			connected = false;
			Log.e("qwe",e.getLocalizedMessage());
		}
	}
	
	public boolean isConnected(){
		return connected;
	}

	public synchronized void endConnection(){
		endConnection = true;
	}
}
