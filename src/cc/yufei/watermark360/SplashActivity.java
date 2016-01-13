package cc.yufei.watermark360;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.os.PowerManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

public class SplashActivity extends Activity {

	// screen width/height
	public static int iScreenWidth;
	public static int iScreenHeight;
	
	public static Activity activity = null;
	
	// 电源管理
	PowerManager.WakeLock mWakeLock;
	
	private TextView tvVersion;
	
//	private static String LOG_TAG = "SplashActivity";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_splash);
		
	     // 方法1 Android获得屏幕的宽和高    
        WindowManager windowManager = getWindowManager();    
        Display display = windowManager.getDefaultDisplay();    
        iScreenWidth = display.getWidth();    
        iScreenHeight = display.getHeight();   
        
        activity = this;
        
        // 电源管理
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        
        mWakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "WM_360");
        mWakeLock.acquire();
        
        tvVersion = (TextView)findViewById(R.id.text_version);
        tvVersion.setText("版本: " + getAppVersionName(SplashActivity.this));
        tvVersion.setTextColor(Color.BLACK);
	}
	
	//获取当前版本号 
	private String getAppVersionName(Context context) { 
		String versionName = ""; 
		try { 
				PackageManager packageManager = context.getPackageManager(); 
				PackageInfo packageInfo = packageManager.getPackageInfo("cc.yufei.watermark360", 0); 
				versionName = packageInfo.versionName; 
				if (TextUtils.isEmpty(versionName)) { 
					return ""; 
				} 
		} catch (Exception e) { 
			e.printStackTrace(); 
		//	return "versionName is empty"; 
		} 
		return versionName; 
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
	}
	
	// first type watermark extraction
	public void cameraOnClick(View view)
	{
		startActivity(new Intent(SplashActivity.this, VideoActivity.class));
	}
	
	// second type watermark extraction
	public void cameraOnClick2(View view)
	{
		Intent intent = new Intent();
		intent.setClass(SplashActivity.this, ExtractActivity.class );
		
		startActivityForResult( intent, 1 );
	}
	
    @Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
    	super.onActivityResult(requestCode, resultCode, data);
		
		switch(requestCode)
		{
		case 1:
			if( ExtractActivity.mFinalExtractBitmap != null )
			{
				Log.i( "extract_result", "onActivityResult");
				startActivity(new Intent(SplashActivity.this, ResultActivity.class));
			}
		}
    }
	
	// contact us
	public void contactUs(View view)
	{
		startActivity(new Intent(SplashActivity.this, ContactActivity.class));
	}
	
	// about us
	public void aboutUs(View view)
	{
		startActivity(new Intent(SplashActivity.this, AboutActivity.class));
	}
	
	
	
	private long lastBackKeyDownTick = 0;
    public static final long MAX_DOUBLE_BACK_DURATION = 1500;
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            long currentTick = System.currentTimeMillis();
            if(currentTick - lastBackKeyDownTick > MAX_DOUBLE_BACK_DURATION){
                Toast.makeText(this,R.string.alert_exitapp, Toast.LENGTH_SHORT).show();
                lastBackKeyDownTick = currentTick;
            } else {
                finish();
            }
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }
    
}
