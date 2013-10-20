package pl.edu.agh.zpi.admintools.utils;

import android.os.Handler;
import android.os.Message;

public class IncomingHandler extends Handler {
	private Handable handable;

	public IncomingHandler(Handable handable) {
		this.handable = handable;
	}

	@Override
	public void handleMessage(Message msg) {
		super.handleMessage(msg);
		handable.handleMessage(msg);
	}
}
