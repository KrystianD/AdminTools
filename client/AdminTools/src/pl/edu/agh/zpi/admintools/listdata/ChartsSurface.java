package pl.edu.agh.zpi.admintools.listdata;

import java.util.ArrayList;
import java.util.Collections;

import pl.edu.agh.zpi.admintools.ChartsActivity;
import pl.edu.agh.zpi.admintools.connection.packets.PacketStatsRequest;
import pl.edu.agh.zpi.admintools.utils.Vector2D;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 *	\class ChartsSurface
 *	\brief Surface for displaying charts data.
 */
public class ChartsSurface extends SurfaceView implements
		SurfaceHolder.Callback {
	//! Charts data accuracy.
	public static final int ACCURACY = 10;

	ArrayList<Float> values = new ArrayList<Float>();
	float minVal;
	float maxVal;
	float diff;
	ChartsActivity context;
	SurfaceHolder holder;

	/**
	 *	\fn public ChartsSurface(Context context, AttributeSet attrs)
	 *	\brief Constructor, set global context and set of attribute.
	 *	\param context Context with env global informations.
	 *	\param attrs A collection of attributes (AttributeSet).
	 */
	public ChartsSurface(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = (ChartsActivity) context;
		this.holder = getHolder();
		holder.addCallback(this);
	}
	/**
	 *	\fn public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
	 *	\brief Execute on surface changed event.
	 *	\param holder Surface holder class.
	 *	\param format Surface format.
	 *	\param width Surface width.
	 *	\param height Surface height.
	 *	\return None.
	 */
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		draw();
	}
	/**
	 *	\fn public void surfaceCreated(SurfaceHolder holder)
	 *	\brief Execute on surface creation.
	 *	\param holder Surface holder class.
	 *	\return None.
	 */
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		draw();
	}
	/**
	 *	\fn public void surfaceDestroyed(SurfaceHolder holder)
	 *	\brief Execute on surface destruction.
	 *	\param holder Surface holder class.
	 *	\return None.
	 */
	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
	}
	/**
	 *	\fn public void setData(ArrayList<Float> data, int type)
	 *	\brief Set new data tu surface and display it on charts.
	 *	\param data List with series of data.
	 *	\param type Stats type flag.
	 *	\return None.
	 */
	public void setData(ArrayList<Float> data, int type) {
		Log.d("qwe", "ChartsSurface.setData()");
		values = data;
		if (data == null || data.size() < 4)
			return;
		switch (type) {
		case PacketStatsRequest.CPU:
			minVal = 0;
			maxVal = (float)Math.ceil(Collections.max(data)) * 100 + 30;
			context.setAxis((int) minVal, (int) maxVal);
			
			for (int i = 0; i < data.size(); i++) {
				data.set(i, data.get(i) * 100);
			}
			break;
		case PacketStatsRequest.DISK:
			minVal = 0;
			maxVal = 100;
			for (int i = 0; i < data.size(); i++) {
				data.set(i, data.get(i) * 100);
			}
			context.setAxis((int) minVal, (int) maxVal);
			break;
		case PacketStatsRequest.RAM:
			minVal = 0;
			maxVal = 100;
			context.setAxis((int) minVal, (int) maxVal);
			for (int i = 0; i < data.size(); i++) {
				data.set(i, data.get(i) * 100);
			}
			break;
		case PacketStatsRequest.TEMP:
			minVal = 0;
			maxVal = 130;
			context.setAxis((int) minVal, (int) maxVal);
			break;
		default:
			break;
		}
		diff = (short) (maxVal - minVal);

		draw();
	}

	private Vector2D getPoint(int x, float val, Canvas c) {
		val = (val - minVal) * c.getHeight() / diff;
		val = c.getHeight() - val;
		return new Vector2D(x * ACCURACY, val);
	}

	private void draw() {
		Canvas c = null;
		Paint paint = new Paint();
		paint.setColor(Color.WHITE);
		try {
			c = holder.lockCanvas();

			c.drawRect(new Rect(0, 0, c.getWidth(), c.getHeight()), paint);

			if (values == null || values.size() == 0)
				return;

			paint.setColor(Color.rgb(100, 100, 100));

			for (int i = c.getHeight(); i > 0; i -= (c.getHeight() / maxVal) * 10) {
				c.drawLine(0, i, c.getWidth(), i, paint);
			}

			paint.setColor(Color.BLUE);
			paint.setStyle(Paint.Style.STROKE);
			paint.setFlags(Paint.ANTI_ALIAS_FLAG | Paint.DITHER_FLAG);
			paint.setStrokeWidth(2.0f);

			boolean isEmpty = true;
			Path linePath = new Path();
			for (int i = 2; i < values.size() - 1; i++) {
				if (values.get(i) < 0 || values.get(i + 1) < 0
						|| values.get(i - 1) < 0 || values.get(i - 2) < 0) {
					isEmpty = true;
					continue;
				}
				
				Vector2D p1 = getPoint(i - 2, values.get(i - 2), c);
				Vector2D p2 = getPoint(i - 1, values.get(i - 1), c);
				Vector2D p3 = getPoint(i, values.get(i), c);
				Vector2D p4 = getPoint(i + 1, values.get(i + 1), c);

				Vector2D P1 = p2;
				Vector2D P2 = p3;
				p1.sub(p2);
				p1.mul(-1);
				Vector2D T1 = p1;
				p4.sub(p3);
				Vector2D T2 = p4;

				for (int j = 0; j < ACCURACY; j++) {
					Vector2D pt = new Vector2D();
					float s = (float) j / (float) ACCURACY;
					float H1 = 2 * s * s * s - 3 * s * s + 1;
					float H2 = -2 * s * s * s + 3 * s * s;
					float H3 = s * s * s - 2 * s * s + s;
					float H4 = s * s * s - s * s;

					pt.add(Vector2D.mul(P1, H1));
					pt.add(Vector2D.mul(P2, H2));
					pt.add(Vector2D.mul(T1, H3));
					pt.add(Vector2D.mul(T2, H4));

					if (isEmpty) {
						linePath.moveTo(pt.x, pt.y);
						isEmpty = false;
					}
					linePath.lineTo(pt.x, pt.y);
				}
			}
			c.drawPath(linePath, paint);
		} finally {
			if (c != null) {
				holder.unlockCanvasAndPost(c);
			}
			c = null;
		}
	}
}
