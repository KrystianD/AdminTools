/**
 *	\namespace pl.edu.agh.zpi.admintools.utils
 *	\brief Utilities module.
 */
package pl.edu.agh.zpi.admintools.utils;

import android.graphics.PointF;

/**
 *	\class Vector2D
 *	\brief Represents two-dimensional vector as point.
 */
public class Vector2D {
	//! X coordinate.
	public float x;
	//! Y coordinate.
	public float y;

	/**
	 *	\fn public Vector2D()
	 *	\brief Default constructor. Zeroes coordinates.
	 */
	public Vector2D() {
		x = 0;
		y = 0;
	}
	/**
	 *	\fn public Vector2D(float x, float y)
	 *	\brief Constructor, sets coordinates.
	 *	\param x X coordinate.
	 *	\param y Y coordinate.
	 */
	public Vector2D(float x, float y) {
		this.x = x;
		this.y = y;
	}
	/**
	 *	\fn public void mul(float val)
	 *	\brief Multiply vector by scalar.
	 *	\param val Scalar to multiply by.
	 *	\return None.
	 */
	public void mul(float val){
		x *= val;
		y *= val;
	}
	/**
	 *	\fn public static Vector2D mul(Vector2D a , float b)
	 *	\brief Create new vector as result of multiplication of vector and scalar.
	 *	\param a Vector to multiply.
	 *	\param b Scalar to multiplay by.
	 *	\result Instance of Vector2D as result of multiplication.
	 */
	public static Vector2D mul(Vector2D a , float b){
		float x = a.x*b;
		float y = a.y*b;
		return new Vector2D(x, y);
	}
	/**
	 *	\fn public void add(Vector2D vec)
	 *	\brief Add vector to current vector.
	 *	\param vec Vector2D to add.
	 *	\result None.
	 */
	public void add(Vector2D vec){
		x += vec.x;
		y += vec.y;
	}
	/**
	 *	\fn public void sub(Vector2D vec)
	 *	\brief Subtract vector from current vector.
	 *	\param vec Vector2D to subtract.
	 *	\return None.
	 */
	public void sub(Vector2D vec){
		x -= vec.x;
		y -= vec.y;
	}
	/**
	 *	\fn public float length()
	 *	\brief Calculate length (magnitued) of current vector.
	 *	\return Length of vector.
	 */
	public float length(){
		return (float) Math.sqrt(x*x + y*y);
	}
	/**
	 *	\fn public void norm()
	 *	\brief Normalize current vector.
	 *	\return None.
	 */
	public void norm(){
		float lenInv = 1/length();
		x *= lenInv;
		y *= lenInv;
	}
	/**
	 *	\fn public PointF getPointF()
	 *	\brief Create PointF instance from current vector coordinates.
	 *	\return Instance of PointF filled with vector coordinates.
	 */
	public PointF getPointF(){
		return new PointF(x,y);
	}
}
