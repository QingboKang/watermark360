#ifndef __SCRAMBLED_MARK_FILTER_H__
#define __SCRAMBLED_MARK_FILTER_H__

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace cv;
using namespace std;

#include <android/log.h>

#define LOG "jni"
#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, LOG, __VA_ARGS__ )

extern "C" {

int ScrambledWMFilter( Mat &matBGRA, int height, int width,
                      char* strOriginalFrame, char* strFirstLocation,
                      char* strSecondLocation, char* strWatermarkRegion,
                      char* strFinalExtracion);

int FirstLocation(Mat & srcImageFull, char* strFirstLocation);

int IsCentersCoincide( Point ptDetection, int iRadius, int iMinSide );

int LocAndExtract( Mat & srcImage, char* strSecondLocation, char* strWatermarkRegion,
                   char* strFinalExtracion );

int ScrambleImage( Mat & image, char* strWatermarkRegion,
                   char* strFinalExtracion );

// enhance watermark
void EnhanceWatermark( Mat & image,char* strWatermarkRegion,
                      char* strFinalExtracion );
// enhance watermark - method 2
void EnhanceWatermark2( Mat & image,char* strWatermarkRegion,
                       char* strFinalExtracion );


#endif /* __SCRAMBLED_MARK_FILTER_H__ */

}
