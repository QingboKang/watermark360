package com.cameraView;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import cc.yufei.watermark360.SplashActivity;
import cc.yufei.watermark360.VideoActivity;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.ShutterCallback;
import android.hardware.Camera.Size;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

import cc.yufei.watermark360.*;

public class CameraView extends SurfaceView implements Callback {

	SurfaceHolder mSurfaceHolder;
	Camera mCamera = null;

	Context mContext;
	private final int cameraId = 0;

	private Handler mHandler;
	
	
	// 时间线程 聚焦
	private Timer mTimer;
	private TimerTask mTimerTask;
	
	private String LOG_TAG = "CameraView";
	
	// 路径 文件名 jni使用
	public static String strCaptureFilePath = "/storage/emulated/0/YFWatermark/";
	public static int count_frame = 0;
	
	public void setHandle(Handler handler) {
		mHandler = handler;
	}

	private PictureCallback raw = new PictureCallback() {

		public void onPictureTaken(byte[] data, Camera camera) {

		}
	};
	private PictureCallback jpeg = new PictureCallback() {
		public void onPictureTaken(byte[] data, Camera camera) {
			// TODO Auto-generated method stub
			try {
				Bitmap bm = BitmapFactory.decodeByteArray(data, 0, data.length);
			//	Log.i("length", "" + data.length);
			//	Log.i("data[0]", "" + data[0]);
				File file = new File("/sdcard/wjh.jpg");
				BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(file));
				bm.compress(Bitmap.CompressFormat.JPEG, 100, bos); // 保存图片
				bos.flush();
				bos.close();
			} catch (Exception e) {
				e.printStackTrace();
			}

		}
	};

	private PreviewCallback mPreviewCallback = new PreviewCallback() {

		public void onPreviewFrame(byte[] data, Camera camera) {
			if (data != null) {
				
				int imageWidth = mCamera.getParameters().getPreviewSize().width;
				int imageHeight = mCamera.getParameters().getPreviewSize().height;
				
			//	Log.i( LOG_TAG, "w: " + imageWidth + " h: " + imageHeight);
				long start, end1, end2;
			    start = System.currentTimeMillis();
				
				int RGBData[] = new int[imageWidth * imageHeight];
			/*	decodeYUV420SP(RGBData, data, imageWidth, imageHeight); // 解码 
				*/
				
				
				
			//	Log.i(LOG_TAG, "before jni decode");
				LibWatermarkFilter.FirstExtract(imageWidth, imageHeight, data, RGBData, 
						strCaptureFilePath + 
						"frame_" + count_frame + imageWidth + "_" + imageHeight + ".bin");
				
                end1 = System.currentTimeMillis();
              //  Log.i( LOG_TAG, "time: " + (end1 - start) + " ms"); 
			    
				Bitmap bm = Bitmap.createBitmap(RGBData, imageWidth, imageHeight, Config.ARGB_8888);
			
				count_frame ++ ;
				
                end2 = System.currentTimeMillis();
              //  Log.i( LOG_TAG, "total time: " + (end2 - start) + " ms"); 
			    
				// 
				//Bitmap bm_result = getBitmapFromC(bm);
				
				Message message = new Message();
				message.what = 1033;
				message.obj = bm;
				mHandler.sendMessage(message); // 显示图片
			
			}
		}
	};

	private AutoFocusCallback autoFocus = new AutoFocusCallback() {

		public void onAutoFocus(boolean success, Camera camera) {
			// TODO Auto-generated method stub

		}
	};
	
	private Bitmap getBitmapFromC(Bitmap bmp_src)
	{
    	// Get the width/height of the image
        int i_bmp_width = bmp_src.getWidth();
        int i_bmp_height = bmp_src.getHeight();
        
        // get the image data
        int[] pixels = new int[i_bmp_width * i_bmp_height];
        bmp_src.getPixels(pixels, 0, i_bmp_width, 0, 0, i_bmp_width, i_bmp_height);
        
    	LibWatermarkFilter.PlainWMFilter(i_bmp_width,
    			i_bmp_height, pixels, strCaptureFilePath + "camera_bgra.png"); 
    	
        // result bitmap
        Bitmap bmp_result = Bitmap.createBitmap(i_bmp_width, i_bmp_height, Bitmap.Config.ARGB_8888);
        
        bmp_result.setPixels(pixels, 0, i_bmp_width, 0, 0, i_bmp_width, i_bmp_height); 
        
        return bmp_result;
	}


	/**
	 * 解码
	 * 
	 * @param rgb
	 * @param yuv420sp
	 * @param width
	 * @param height
	 */
	static public void decodeYUV420SP(int[] rgb, byte[] yuv420sp, int width, int height) {
		final int frameSize = width * height;

		for (int j = 0, yp = 0; j < height; j++) {
			int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
			for (int i = 0; i < width; i++, yp++) {
				int y = (0xff & ((int) yuv420sp[yp])) - 16;
				if (y < 0)
					y = 0;
				if ((i & 1) == 0) {
					v = (0xff & yuv420sp[uvp++]) - 128;
					u = (0xff & yuv420sp[uvp++]) - 128;
				}

				int y1192 = 1192 * y;
				int r = (y1192 + 1634 * v);
				int g = (y1192 - 833 * v - 400 * u);
				int b = (y1192 + 2066 * u);

				if (r < 0)
					r = 0;
				else if (r > 262143)
					r = 262143;
				if (g < 0)
					g = 0;
				else if (g > 262143)
					g = 262143;
				if (b < 0)
					b = 0;
				else if (b > 262143)
					b = 262143;

				rgb[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
			}
		}
	}

	public void startCamera() {
		
	//	Log.i(LOG_TAG, "startCamera()");
		
		mCamera.startPreview();
	}

	public void stopCamera() {
		
		//Log.i(LOG_TAG, "stopCamera()");
		
		if (mCamera != null) {
			mCamera.stopPreview();
		}
	}

	public CameraView(Context context) {
		super(context);
		
	//	Log.i(LOG_TAG, "CameraView(Context context)");
		
		// TODO Auto-generated constructor stub
		mSurfaceHolder = getHolder();// 获得surfaceHolder引用
		mSurfaceHolder.addCallback(this);
		mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);// 设置类型
		mContext = context;
		
	}

	public CameraView(Context context, AttributeSet attrs) {
		super(context, attrs);
		
	//	Log.i(LOG_TAG, "CameraView(Context context, AttributeSet attrs)");
		
		mSurfaceHolder = getHolder();// 获得surfaceHolder引用
		mSurfaceHolder.addCallback(this);
		mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);// 设置类型
		mContext = context;
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

	//	Log.i(LOG_TAG, "surfaceChanged");
		
	/*	Camera.Parameters parameters = mCamera.getParameters();// 获得相机参数
		parameters.setPreviewSize(width, height); // 设置预览图像大小
		parameters.setPictureFormat(PixelFormat.JPEG); // 设置照片格式
		// if (CameraActivity.ScreenOrient(CameraActivity.activity) ==
		// Configuration.ORIENTATION_LANDSCAPE) {
		// Log.i("横屏", "横屏");
		// mCamera.setDisplayOrientation(0);
		// } else if (CameraActivity.ScreenOrient(CameraActivity.activity) ==
		// Configuration.ORIENTATION_PORTRAIT) {
		// Log.i("素屏", "素屏");
		// mCamera.setDisplayOrientation(90);
		// }
	
		mCamera.setParameters(parameters);// 设置相机参数 */


	//	mCamera.setDisplayOrientation( 90 );
		
		mCamera.startPreview();
		mCamera.autoFocus(autoFocus); 
		
        mTimer = new Timer();
		mTimerTask = new CameraTimerTask();
		mTimer.schedule(mTimerTask, 0, 1000);
	}
	
    class CameraTimerTask extends TimerTask {
    	@Override
    	public void run() {
    		if(mCamera != null){
    			mCamera.autoFocus(autoFocus);
    		}
    		
    	}
    }

	public void surfaceCreated(SurfaceHolder holder) {
		
	//	Log.i(LOG_TAG, "surfaceCreated" );
		
		// TODO Auto-generated method stub
		if (mCamera == null) {
			mCamera = Camera.open(cameraId);// 开启相机,不能放在构造函数中，不然不会显示画面.
			
			//
	    	 DisplayMetrics metric = new DisplayMetrics();

			// 设置参数
			Camera.Parameters parameters = mCamera.getParameters();
			List<Size> mSupportedPreviewSizes = parameters.getSupportedPreviewSizes();
		
			// 这里屏幕的宽度 高度需要反着来 
			Size mysize = getOptimalPreviewSize(mSupportedPreviewSizes, 
					SplashActivity.iScreenWidth, SplashActivity.iScreenHeight);
			
	        parameters.setPreviewSize(mysize.width, mysize.height);
	        mCamera.setParameters(parameters);
			
			mCamera.setPreviewCallback(mPreviewCallback);
			try {
				mCamera.setPreviewDisplay(holder);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		
	//	Log.i(LOG_TAG, "surfaceDestroyed");
		
		mCamera.setPreviewCallback(null);
		
		// TODO Auto-generated method stub
		mCamera.stopPreview();// 停止预览
		mCamera.release();// 释放相机资源
		mCamera = null;
		
		mTimer.cancel();
		
	}
	
    private Size getOptimalPreviewSize(List<Size> sizes, int w, int h) {
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) w / h;
        if (sizes == null) return null;

        Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;

        int targetHeight = h;

        // Try to find an size match aspect ratio and size
        for (Size size : sizes) {
            double ratio = (double) size.width / size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE) continue;
            if (Math.abs(size.height - targetHeight) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.height - targetHeight);
            }
        }

        // Cannot find the one match the aspect ratio, ignore the requirement
        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Size size : sizes) {
                if (Math.abs(size.height - targetHeight) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.height - targetHeight);
                }
            }
        }
        return optimalSize;
    }
    
		public int curDregree(int cameraId) {
			 android.hardware.Camera.CameraInfo info =
		             new android.hardware.Camera.CameraInfo();
		     android.hardware.Camera.getCameraInfo(cameraId, info);
		     int rotation = VideoActivity.activity.getWindowManager().getDefaultDisplay()
		             .getRotation();
		     int degrees = 0;
		     switch (rotation) {
		         case Surface.ROTATION_0: degrees = 0; break;
		         case Surface.ROTATION_90: degrees = 90; break;
		         case Surface.ROTATION_180: degrees = 180; break;
		         case Surface.ROTATION_270: degrees = 270; break;
		     }

		     int result;
		     if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
		         result = (info.orientation + degrees) % 360;
		         result = (360 - result) % 360;  // compensate the mirror
		     } else {  // back-facing
		         result = (info.orientation - degrees + 360) % 360;
		     }
		     return result;
		}
	        

}
