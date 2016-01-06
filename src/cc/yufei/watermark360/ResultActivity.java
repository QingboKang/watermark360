package cc.yufei.watermark360;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;

public class ResultActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_result);
		
		final ImageView contentView = (ImageView) findViewById(R.id.iv_result);
		
		contentView.setImageBitmap( ExtractActivity.mFinalExtractBitmap );
	}
}
