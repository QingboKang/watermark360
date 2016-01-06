package cc.yufei.watermark360;

public class LibWatermarkFilter {

	static 
	{
		System.loadLibrary("WatermarkFilter");
	}
	
	

	
	public static native int PlainWMFilter( int width, int height, int[] inData, 
		    String strImgFilePath );
	
	public static native int ScrambledWMFilter( int width, int height, int[] inData, int[] outData, 
			int templateWidth, int templateHeight, int[] templateData,
			String strOriginalFrame, String strFirstLocation, String strSecondLocation,
			String strWatermarkRegion, String strFinalExtracion );
	
	public static native int FirstExtract( int width, int height, byte[] yuv, int[] rgba, 
			String strDataFilePath );
	
	public static native int SecondExtract( int width, int height, byte[] yuv, int[] rgba, 
			String strOriginalFrame, String strFirstLocation, String strSecondLocation,
			String strWatermarkRegion, String strFinalExtracion  );

}
