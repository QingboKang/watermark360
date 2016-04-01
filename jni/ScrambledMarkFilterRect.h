/****************************** Module Header ******************************\
Module Name: <ScrambledMarkFilterRect.h>
Project: <Watermark360 APP>
Copyright (c) Yufei Co.Ltd.

This file implements the 2nd type of watermark extraction algorithm

version 1.6

Written by Kangqingbo at 2016.03.23

\***************************************************************************/

#ifndef __SCRAMBLED_MARK_FILTER_RECT_H__
#define __SCRAMBLED_MARK_FILTER_RECT_H__

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <cmath>
#include <time.h>

using namespace cv;
using namespace std;

#include <android/log.h>

#define LOG "YufeiSDK"
#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, LOG, __VA_ARGS__ )

#define  RECT_AREA_MAX  0.15
#define  RECT_AREA_MIN  0.02

extern "C" {

int ScrambledWMFilterRect( Mat &matBGRA, int height, int width, 
                          char* strOriginalFrame, char* strCorrFrame, 
                          char* strLocWatermark, char* strExtractResult );


///// inner function

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

double DistanceOfTwoPts( cv::Point pt1, cv::Point pt2 );

int GetIndex(cv::Point pt, cv::Point* pptRect);

int CheckVertices(std::vector<cv::Point> ptVertices );

std::vector<cv::Point> GetFourVertices(std::vector<cv::Point>& contour, cv::Point* pptRect);

int Correction( cv::Mat& inImg, std::vector<cv::Point>& contour, cv::Rect& rectLogo );

// scramble rgb image 
int ScrambleImage( Mat & image );

// enhance watermark - method 2
void EnhanceWatermark2( Mat & image );

// enhance watermark - method 3
void EnhanceWatermark3( Mat & image);

// enhance watermark - method 1
void EnhanceWatermark1( Mat & image);

void AffinetCorrection( Mat& srcImage, std::vector<cv::Point>& ptVertics, Rect rect );

int RectBasedLocation( Mat& src, cv::Rect& rectLogo);

int ExtractWatermarkRect( cv::Mat& locImg, Rect& rectLogo );


}

#endif /* __SCRAMBLED_MARK_FILTER_RECT_H__ */
