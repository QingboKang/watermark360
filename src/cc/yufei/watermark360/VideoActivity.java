package cc.yufei.watermark360;

import com.cameraView.CameraView;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;



public class VideoActivity extends Activity {

	CameraView mCameraView;
	
	// result imageView
	private ImageView imageView;
	
	private String LOG_TAG = "VideoActivity";
	
	// screen width/height
	public static int iScreenWidth;
	public static int iScreenHeight;
	
	public static Activity activity = null;

	public Matrix matrix = new Matrix();
	
	private Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			if (msg.what == 1033) {
				Bitmap bitmap = (Bitmap)msg.obj;
				
				matrix.setRotate(90);
				bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
				
				imageView.setImageBitmap(bitmap);
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
		
	//	Log.i(LOG_TAG, "before setContentView");
		setContentView(R.layout.activity_video);
	//	Log.i(LOG_TAG, "after setContentView");
		
		mCameraView = (CameraView) findViewById(R.id.mSurfaceView);
		mCameraView.setHandle(mHandler);
		
		imageView = (ImageView)findViewById(R.id.resultImageView);
		
		imageView.setMinimumWidth(iScreenWidth);
		imageView.setMinimumHeight(iScreenHeight);
		activity = this;
		// cannot start camera in onCreate
		
	}



	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();



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
	

}
