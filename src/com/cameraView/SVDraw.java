package com.cameraView;

import cc.yufei.watermark360.R;
import android.content.Context;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.PixelFormat;

import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class SVDraw extends SurfaceView  implements SurfaceHolder.Callback{

	protected SurfaceHolder sh; // 专门用于控制surfaceView的
	private int width;
	private int height;

	private String LOG_TAG = "SVDraw";
	
	public SVDraw(Context context) {
		
		
		super(context);
		
	//	Log.i(LOG_TAG, "SVDraw(Context context)");
		// TODO Auto-generated constructor stub
		sh = getHolder();
		sh.addCallback(this);
		sh.setFormat(PixelFormat.TRANSPARENT); // 设置为透明
		setZOrderOnTop(true);// 设置为顶端
	}

	
	// XML文件解析需要调用View的构造函数View(Context , AttributeSet)
	// 因此自定义SurfaceView中也需要该构造函数
	public SVDraw(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		//Log.i(LOG_TAG, "SVDraw(Context context, AttributeSet attrs) ");
		
		// TODO Auto-generated constructor stub
		sh = getHolder();
		sh.addCallback(this);
		sh.setFormat(PixelFormat.TRANSPARENT); // 设置为透明
		setZOrderOnTop(true);// 设置为顶端
	}

	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int w, int h) {
		// TODO Auto-generated method stub
		width = w;
		height = h;
		
	//	Log.i(LOG_TAG, "surfaceChanged");
	}

	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		
	//	Log.i(LOG_TAG, "surfaceCreated");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
	//	Log.i(LOG_TAG, "surfaceDestroyed");\
		sh.removeCallback(this);
	}

	void clearDraw() {

		Canvas canvas = sh.lockCanvas();
		canvas.drawColor(Color.BLUE);// 清除画布
		sh.unlockCanvasAndPost(canvas);
	}

	/**
	 * 绘制
	 */
	public void doDraw() {
		
		// 获取模板图像
		Bitmap bmp = BitmapFactory.decodeResource(getResources(), R.drawable.template);  
		
			Canvas canvas = sh.lockCanvas();
			
			// 画布尺寸
			int cwidth = canvas.getWidth();
			int cheight = canvas.getHeight();
			
			
		Log.i(LOG_TAG, "cwidth: " + cwidth + "  cheight: " + cheight);
			
			
			canvas.drawColor(Color.TRANSPARENT);// 这里是绘制背景
			Paint p = new Paint(); // 笔触
			p.setAntiAlias(true); // 反锯齿
			p.setColor(Color.RED);
			p.setStyle(Style.STROKE);
			canvas.drawBitmap(bmp, null, new RectF( Math.abs( (cwidth - cheight)/2 ), 0,
					cwidth - Math.abs( (cheight - cwidth)/2 ), cheight ), p);

			// 提交绘制
			sh.unlockCanvasAndPost(canvas);
		

	}

	public void drawLine() {

		Canvas canvas = sh.lockCanvas();

		canvas.drawColor(Color.TRANSPARENT);// 这里是绘制背景
		Paint p = new Paint(); // 笔触
		p.setAntiAlias(true); // 反锯齿
		p.setColor(Color.RED);
		p.setStyle(Style.STROKE);
		canvas.drawLine(width / 2 - 100, 0, width / 2 - 100, height, p);
		canvas.drawLine(width / 2 + 100, 0, width / 2 + 100, height, p);

		// 提交绘制
		sh.unlockCanvasAndPost(canvas);
	}

}