#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include "include/libyuv.h"

#include "ScrambledMarkFilter.h"

extern "C" {


#define MR(Y,U,V) (1.164383 * (Y - 16 ) + (1.596027)*(V-128))
#define MG(Y,U,V) (1.164383 * (Y - 16 ) - (0.391762)*(U-128) - (0.812969)*(V-128) )
#define MB(Y,U,V) (1.164383 * (Y - 16 ) + (2.01723 * (U-128)))

/**
 * function: Convert YUV420sp(NV21) data to RGB ColorSpace data.
 * param: pYUV -- the pointer point to YUV420sp data
 * param: pRGB -- the pointer point to the RGB data, must be
                  allocated memory by caller. (datasize = 3*width*height)
 * param: height -- the height of the image.
 * param: width -- the width of the image.
 * return: 0 -- success.   others -- failed.
 */
int YUV420SP_C_BGR( char* pYUV, unsigned char* pRGB, int height, int width)
{
    if(!pRGB)
    {
        return -1;
    }
    char* pY = pYUV;
    char* pUV = pYUV + height*width;

    unsigned char* pBGR = NULL;
    unsigned char R = 0;
    unsigned char G = 0;
    unsigned char B = 0;
    unsigned char Y = 0;
    unsigned char Y1 = 0;
    unsigned char U = 0;
    unsigned char V = 0;
    double tmp = 0;
    for ( int i = 0; i < height; i += 2 )
    {
        for ( int j = 0; j < width; ++j )
        {
            pBGR = pRGB+ i*width*3 + j*3;

            Y = *(pY + i * width + j);
            Y1 = *(pY + (i+1) * width + j);
            V = *pUV;
            U = *(pUV + 1);

            //B
            tmp = MB(Y, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            B = (unsigned char)tmp;

            //G
            tmp = MG(Y, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            G = (unsigned char)tmp;
            //R
            tmp = MR(Y, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            R = (unsigned char)tmp;

            *pBGR     = B;
            *(pBGR+1) = G;
            *(pBGR+2) = R;

            // second
            //B
            tmp = MB(Y1, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            B = (unsigned char)tmp;
            //G
            tmp = MG(Y1, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            G = (unsigned char)tmp;
            //R
            tmp = MR(Y1, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            R = (unsigned char)tmp;

            *(pBGR + width * 3) = B;
            *(pBGR + width * 3 + 1) = G;
            *(pBGR + width * 3 + 2) = R;

            if( (j+1)%2 == 0)
            {
                pUV += 2;
            }
        }
    }
    return 0;
}


char* jstringTostring(JNIEnv* env, jstring jstr)
{
       char* rtn = NULL;
       jclass clsstring = env->FindClass("java/lang/String");
       jstring strencode = env->NewStringUTF("utf-8");
       jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
       jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
       jsize alen = env->GetArrayLength(barr);
       jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
       if (alen > 0)
       {
                 rtn = (char*)malloc(alen + 1);
                 memcpy(rtn, ba, alen);
                 rtn[alen] = 0;
       }
       env->ReleaseByteArrayElements(barr, ba, 0);
       return rtn;
}

/**
* function: Convert YUV420sp(NV21) data to RGB ColorSpace data.
* param: pYUV -- the pointer point to YUV420sp data
* param: pRGB -- the pointer point to the RGB data, must be
allocated memory by caller. (datasize = 3*width*height)
* param: height -- the height of the image.
* param: width -- the width of the image.
* return: 0 -- success.   others -- failed.
*/
int YUV420SP_C_BGRA( char* pYUV, unsigned char* pRGBA, int height, int width)
{
    if(!pRGBA)
    {
        return -1;
    }
    char* pY = pYUV;
    char* pUV = pYUV + height*width;

    unsigned char* pBGRA = NULL;
    unsigned char R = 0;
    unsigned char G = 0;
    unsigned char B = 0;
    unsigned char Y = 0;
    unsigned char Y1 = 0;
    unsigned char U = 0;
    unsigned char V = 0;
    double tmp = 0;
    for ( int i = 0; i < height; i += 2 )
    {
        for ( int j = 0; j < width; ++j )
        {
            pBGRA = pRGBA+ i*width*4 + j*4;

            Y = *(pY + i * width + j);
            Y1 = *(pY + (i+1) * width + j);
            V = *pUV;
            U = *(pUV + 1);

            //B
            tmp = MB(Y, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            B = (unsigned char)tmp;

            //G
            tmp = MG(Y, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            G = (unsigned char)tmp;
            //R
            tmp = MR(Y, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            R = (unsigned char)tmp;

            *pBGRA     = B;
            *(pBGRA+1) = G;
            *(pBGRA+2) = R;
            *(pBGRA+3) = 255;

            // second
            //B
            tmp = MB(Y1, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            B = (unsigned char)tmp;
            //G
            tmp = MG(Y1, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            G = (unsigned char)tmp;
            //R
            tmp = MR(Y1, U, V);
            if(tmp > 255) tmp = 255;
            if(tmp < 0) tmp = 0;
            R = (unsigned char)tmp;

            *(pBGRA + width * 4) = B;
            *(pBGRA + width * 4 + 1) = G;
            *(pBGRA + width * 4 + 2) = R;
            *(pBGRA + width * 4 + 3) = 255;

            if( (j+1)%2 == 0)
            {
                pUV += 2;
            }
        }
    }
    return 0;
}



int WriteToBinaryFile(char * filePath, char * pData, int iDataLength)
{
	FILE* fp = fopen( filePath, "wb+");
	if(fp == NULL)
	{
		return -1;
	}

	fwrite( pData, sizeof(char), iDataLength, fp );

	fclose(fp);

	return 0;
}

JNIEXPORT jint JNICALL Java_cc_yufei_watermarkmultifilter_LibWatermarkFilter_ScrambledWMFilter2(
		JNIEnv* env, jobject obj, jint width, jint height, jintArray inData, jintArray outData,
		jint templateWidth, jint templateHeight, jintArray templateData,
		jstring strImgFilePath, jstring strFrameFilePath );
JNIEXPORT jint JNICALL Java_cc_yufei_watermarkmultifilter_LibWatermarkFilter_ScrambledWMFilter2(
		JNIEnv* env, jobject obj, jint width, jint height, jintArray inData, jintArray outData,
		jint templateWidth, jint templateHeight, jintArray templateData,
		jstring strImgFilePath, jstring strFrameFilePath )
{
	// convert jni parameters to C parameters
	jint* pInTemplateData = env->GetIntArrayElements(templateData, 0);

	// the template image
	Mat matTemplate(templateHeight, templateWidth, CV_8UC4, (uchar*)pInTemplateData);

	char * imageFilePath = jstringTostring(env, strImgFilePath);
	char * frameFilePath = jstringTostring(env, strFrameFilePath);

	//imwrite( imageFilePath, matTemplate);

	//LOGD("write template over");


	env->ReleaseIntArrayElements(templateData, pInTemplateData, 0);

	return 0;
}



JNIEXPORT jint JNICALL Java_cc_yufei_watermarkmultifilter_LibWatermarkFilter_ScrambledWMFilter(
		JNIEnv* env, jobject obj, jint width, jint height, jintArray inData, jintArray outData,
		jint templateWidth, jint templateHeight, jintArray templateData,
		jstring strOriginalFrame, jstring strFirstLocation, jstring strSecondLocation,
		jstring strWatermarkRegion, jstring strFinalExtracion );

JNIEXPORT jint JNICALL Java_cc_yufei_watermarkmultifilter_LibWatermarkFilter_ScrambledWMFilter(
		JNIEnv* env, jobject obj, jint width, jint height, jintArray inData, jintArray outData,
		jint templateWidth, jint templateHeight, jintArray templateData,
		jstring strOriginalFrame, jstring strFirstLocation, jstring strSecondLocation,
		jstring strWatermarkRegion, jstring strFinalExtracion )
{
//	LOGD("w: %d   h: %d", width, height);

	time_t t1, t2;

	// convert jni parameters to C parameters
	jint* pInData = env->GetIntArrayElements(inData, 0);
	jint* pInTemplateData = env->GetIntArrayElements(templateData, 0);

	char * pchOriginalFrame = jstringTostring(env, strOriginalFrame);
	char * pchFirstLocation = jstringTostring(env, strFirstLocation);
	char * pchSecondLocation = jstringTostring(env, strSecondLocation);
	char * pchWatermarkRegion = jstringTostring(env, strWatermarkRegion);
	char * pchFinalExtracion = jstringTostring(env, strFinalExtracion);

	// the input frame image
	Mat matBGRA(height, width, CV_8UC4, (uchar*)pInData);
	// the template image
	Mat matTemplate(templateHeight, templateWidth, CV_8UC4, (uchar*)pInTemplateData);



	int iret = 0;
			/* = ScrambledWMFilter( matBGRA, height, width, matTemplate,
			pchOriginalFrame, pchFirstLocation, pchSecondLocation,
			pchWatermarkRegion, pchFinalExtracion ); */

	if( iret == 0 )
	{
	 //   imwrite(pchOriginalFrame, matBGRA  );
	}


    matBGRA.release();

    env->ReleaseIntArrayElements(inData, pInData, 0);
    env->ReleaseIntArrayElements(templateData, pInTemplateData, 0);


	return iret;
}



JNIEXPORT jint JNICALL Java_cc_yufei_watermark360_LibWatermarkFilter_PlainWMFilter(
		JNIEnv* env, jobject obj, jint width, jint height, jintArray inData, jstring strImgFilePath );
JNIEXPORT jint JNICALL Java_cc_yufei_watermark360_LibWatermarkFilter_PlainWMFilter(
		JNIEnv* env, jobject obj, jint width, jint height, jintArray inData, jstring strImgFilePath )
{

	jint* pInData = env->GetIntArrayElements(inData, 0);

	Mat matBGRA(height, width, CV_8UC4, (uchar*)pInData);
//	LOGD("After Mat %d  %d", height, width);

    // roi rectangle
    Rect roiRect(1./5 * width, 1./5 * height, width * 3./5, height * 3./5 );


    // get the roi area of the image
    Mat roiMat(matBGRA, roiRect);

    // split the roi are into rgba channels
    Mat roi_blue;
    vector<Mat> roi_channels;

    split(roiMat, roi_channels);

    roi_blue = roi_channels.at(0);

    // histogram equalization
    equalizeHist( roi_blue, roi_blue);

    // merge back
    roi_channels.at(0) = roi_blue;
    roi_channels.at(1) = roi_blue;
    roi_channels.at(2) = roi_blue;

    merge(roi_channels, roiMat);

    // draw red rectangle
    rectangle( roiMat, Point(0, 0), Point(roiMat.cols, roiMat.rows),
        Scalar(0, 0, 255, 255), 5, 8, 0 );

    matBGRA.release();
    roiMat.release();
    roi_channels.at(0).release();
    roi_channels.at(1).release();
    roi_channels.at(2).release();
    roi_channels.at(3).release();
    roi_blue.release();
    env->ReleaseIntArrayElements(inData, pInData, 0);

	return 0;
  }

JNIEXPORT jint JNICALL Java_cc_yufei_watermark360_LibWatermarkFilter_SecondExtract(
		JNIEnv* env, jobject obj, jint width, jint height, jbyteArray yuv, jintArray bgra,
		jstring strOriginalFrame, jstring strFirstLocation, jstring strSecondLocation,
		jstring strWatermarkRegion, jstring strFinalExtracion );

JNIEXPORT jint JNICALL Java_cc_yufei_watermark360_LibWatermarkFilter_SecondExtract(
		JNIEnv* env, jobject obj, jint width, jint height, jbyteArray yuv, jintArray bgra,
		jstring strOriginalFrame, jstring strFirstLocation, jstring strSecondLocation,
		jstring strWatermarkRegion, jstring strFinalExtracion )
{
	// convert jstring into char*
	char * pchOriginalFrame = jstringTostring(env, strOriginalFrame);
	char * pchFirstLocation = jstringTostring(env, strFirstLocation);
	char * pchSecondLocation = jstringTostring(env, strSecondLocation);
	char * pchWatermarkRegion = jstringTostring(env, strWatermarkRegion);
	char * pchFinalExtracion = jstringTostring(env, strFinalExtracion);

    // Get native access to the given Java arrays.
    jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
    jint*  _bgra = env->GetIntArrayElements(bgra, 0);

    // convert yuv420sp buffer to bgra buffer
    YUV420SP_C_BGRA( (char*) _yuv, (uchar*)_bgra, height, width );

    // Prepare a cv::Mat that points to the BGRA output data.
    Mat matBGRA(height, width, CV_8UC4, (uchar *)_bgra);

    //
   // imwrite( pchOriginalFrame, matBGRA );

	int iret = ScrambledWMFilter( matBGRA, height, width,
			pchOriginalFrame, pchFirstLocation, pchSecondLocation,
			pchWatermarkRegion, pchFinalExtracion );


    // Release the native lock we placed on the Java arrays.
    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);

    return iret;
}

JNIEXPORT void JNICALL Java_cc_yufei_watermark360_LibWatermarkFilter_FirstExtract(
		JNIEnv* env, jobject obj, jint width, jint height, jbyteArray yuv, jintArray bgra,
		jstring strDataFilePath );
JNIEXPORT void JNICALL Java_cc_yufei_watermark360_LibWatermarkFilter_FirstExtract(
		JNIEnv* env, jobject obj, jint width, jint height, jbyteArray yuv, jintArray bgra,
		jstring strDataFilePath )
{
  //  time_t t1, t2;
//	t1 = clock();
	    // Get native access to the given Java arrays.
	    jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
	    jint*  _bgra = env->GetIntArrayElements(bgra, 0);



	    YUV420SP_C_BGRA( (char*) _yuv, (uchar*)_bgra, height, width );



	    // Prepare a cv::Mat that points to the YUV420sp data.
	//    Mat myuv(height + height/2, width, CV_8UC1, (uchar *)_yuv);
	    // Prepare a cv::Mat that points to the BGRA output data.
	    Mat matBGRA(height, width, CV_8UC4, (uchar *)_bgra);

	    // OpenCV can now access/modify the BGRA image if we want ...
	    // roi rectangle
	     Rect roiRect(1./5 * width, 1./5 * height, width * 3./5, height * 3./5 );


	     // get the roi area of the image
	     Mat roiMat(matBGRA, roiRect);

	     // split the roi are into rgba channels
	     Mat roi_blue;
	     vector<Mat> roi_channels;

	     split(roiMat, roi_channels);

	     roi_blue = roi_channels.at(0);

	     // histogram equalization
	     equalizeHist( roi_blue, roi_blue);

	     // merge back
	     roi_channels.at(0) = roi_blue;
	     roi_channels.at(1) = roi_blue;
	     roi_channels.at(2) = roi_blue;

	     merge(roi_channels, roiMat);

	     // draw red rectangle
	     rectangle( roiMat, Point(0, 0), Point(roiMat.cols, roiMat.rows),
	         Scalar(0, 0, 255, 255), 5, 8, 0 );

	     matBGRA.release();
	     roiMat.release();
	     roi_channels.at(0).release();
	     roi_channels.at(1).release();
	     roi_channels.at(2).release();
	     roi_channels.at(3).release();
	     roi_blue.release();


	    // Release the native lock we placed on the Java arrays.
	    env->ReleaseIntArrayElements(bgra, _bgra, 0);
	    env->ReleaseByteArrayElements(yuv, _yuv, 0);

	 //   t2 = clock();
	 //   LOGD("%ld", (t2 - t1)/1000);
}


}
