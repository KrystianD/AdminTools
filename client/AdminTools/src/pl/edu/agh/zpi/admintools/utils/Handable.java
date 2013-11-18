package pl.edu.agh.zpi.admintools.utils;

import android.os.Message;

/**
 *	\interface Handable
 *	\brief Used by handlers able to handle incoming message.
 */
public interface Handable {
	/**
	 *	\fn public void handleMessage(Message msg)
	 *	\brief Handle incoming message.
	 *	\param msg Message to handle.
	 *	\return None.
	 */
	public void handleMessage(Message msg);
}
