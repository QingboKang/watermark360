package cc.yufei.watermark360;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

public class SplashActivity extends Activity {

	// screen width/height
	public static int iScreenWidth;
	public static int iScreenHeight;
	
	public static Activity activity = null;
	
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
	}
	// first type watermark extraction
	public void cameraOnClick(View view)
	{
		startActivity(new Intent(SplashActivity.this, VideoActivity.class));
	}
	
	// second type watermark extraction
	public void cameraOnClick2(View view)
	{
		startActivity(new Intent(SplashActivity.this, ExtractActivity.class));
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
