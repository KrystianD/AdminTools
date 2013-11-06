package pl.edu.agh.zpi.admintools.utils;

import android.graphics.PointF;

public class Vector2D {
	public float x;
	public float y;
	
	public Vector2D() {
		x = 0;
		y = 0;
	}
	
	public Vector2D(float x, float y) {
		this.x = x;
		this.y = y;
	}

	public void mul(float val){
		x *= val;
		y *= val;
	}
	
	public static Vector2D mul(Vector2D a , float b){
		float x = a.x*b;
		float y = a.y*b;
		return new Vector2D(x, y);
	}
	
	public void add(Vector2D vec){
		x += vec.x;
		y += vec.y;
	}
	
	public void sub(Vector2D vec){
		x -= vec.x;
		y -= vec.y;
	}
	
	public float length(){
		return (float) Math.sqrt(x*x + y*y);
	}
	
	public void norm(){
		float lenInv = 1/length();
		x *= lenInv;
		y *= lenInv;
	}
	
	public PointF getPointF(){
		return new PointF(x,y);
	}
}
