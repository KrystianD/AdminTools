package pl.edu.agh.zpi.admintools.listdata;

import pl.edu.agh.zpi.admintools.R;
import pl.edu.agh.zpi.admintools.StatsActivity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;

public class LongClickItemListener implements
		AdapterView.OnItemLongClickListener {

	private StatsActivity context;
	private short id;
	
	public LongClickItemListener(StatsActivity context) {
		this.context = context;
	}

	@Override
	public boolean onItemLongClick(AdapterView<?> parent, View view,
			int position, long id) {
		this.id = (short)view.getId();
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		builder.setTitle(R.string.choose_agent_action);
		builder.setItems(R.array.choose_agent_array,
				new MyDialogOnClickListener(this));
		
		builder.show();

		return false;
	}

	void dialogFeedback(int what){
		context.listenerFeedback(what,id);
	}
	
}

class MyDialogOnClickListener implements DialogInterface.OnClickListener {
	private final LongClickItemListener parent;
	
	public MyDialogOnClickListener(LongClickItemListener parent) {
		this.parent = parent;
	}

	@Override
	public void onClick(DialogInterface dialog, int which) {
		parent.dialogFeedback(which);
	}
}
