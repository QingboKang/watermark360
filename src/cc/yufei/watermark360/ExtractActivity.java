package cc.yufei.watermark360;

import com.cameraView.CameraView2;
import com.cameraView.SVDraw;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;


public class ExtractActivity extends Activity {

	private CameraView2 mCameraView;
	public static SVDraw svDraw = null;
	
//	private String LOG_TAG = "ExtractActivity";
	
	// screen width/height
	public static int iScreenWidth;
	public static int iScreenHeight;
	
	public static Activity activity = null;
	public Matrix matrix = new Matrix();
	
	// 提取结果图像
	public static Bitmap mFinalExtractBitmap;
	
	// 电源管理
	PowerManager.WakeLock mWakeLock;
	
	
	private Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			if (msg.what == 1033) {
				Bitmap bitmap = (Bitmap)msg.obj;
				
				matrix.setRotate(90);
				bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
				
				mFinalExtractBitmap = bitmap;
				
				activity.setResult(1);
				Log.i( "extract_result", "setResult");
				activity.finish();
			}
		};
	};
	

	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
	/*	getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);*/

	     // 方法1 Android获得屏幕的宽和高    
        WindowManager windowManager = getWindowManager();    
        Display display = windowManager.getDefaultDisplay();    
        iScreenWidth = display.getWidth();    
        iScreenHeight = display.getHeight();    
		
		setContentView(R.layout.activity_extract);
		
		mCameraView = (CameraView2) findViewById(R.id.mCameraView);
		mCameraView.setHandle(mHandler);
		
		svDraw = (com.cameraView.SVDraw) findViewById(R.id.svDraw);

		activity = this;
		
		mFinalExtractBitmap = null;
		
		
        // 电源管理
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        
        mWakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "WM_360");
        mWakeLock.acquire();
	}
	
	public void onDestroy()
	{
		super.onDestroy();
		
		if( mWakeLock == null )
		{
		//	Log.i(LOG_TAG, "mWakeLock is null");
		}
		if( mWakeLock != null )
		{
		//	Log.i(LOG_TAG, "mWakeLock is not null");
			mWakeLock.release();
		}
	}
	
	public void onPause()
	{
		super.onPause();
		if( mWakeLock != null )
		{
			mWakeLock.release();
			mWakeLock = null;
		}
	}
	
	public void onResume()
	{
		super.onResume();
		if( mWakeLock == null )
		{
	        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
	        
	        mWakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "WM_360");
	        mWakeLock.acquire();
		}
		mFinalExtractBitmap = null;
	}
	
	
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) 
        {
        	mCameraView.stopCamera();
        	this.finish();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }
    
    @Override
    public void onBackPressed()
    {
    	super.onBackPressed(); 
    	mCameraView.stopCamera();
    	this.finish();
    }




}
