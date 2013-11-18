package pl.edu.agh.zpi.admintools.utils;

import android.os.Handler;
import android.os.Message;

/**
 *	\class IncomingHandler
 *	\brief Incoming message handler.
 */
public class IncomingHandler extends Handler {
	private Handable handable;

	/**
	 *	\fn public IncomingHandler(Handable handable)
	 *	\brief Constructor, sets Handable msg handler.
	 *	\param handable Handable handler.
	 */
	public IncomingHandler(Handable handable) {
		this.handable = handable;
	}
	/**
	 *	\fn public void handleMessage(Message msg)
	 *	\brief Handle incoming message.
	 *	\param msg Message to handle.
	 *	\return None.
	 */
	@Override
	public void handleMessage(Message msg) {
		super.handleMessage(msg);
		handable.handleMessage(msg);
	}
}
