#include "ScrambledMarkFilter.h"

//////////////////////////////////////////////////////////////////////////

extern "C" {
//////////////////////////////////////////////////////////////////////////

////////// global variables //////////
// flag to mark if read the template image
// we will only read the image once because of the factor of performance
bool g_bIsReadTemplateImage = false;
// template image mat
Mat g_TemplateMat;
// the frame image size. 
// we will only resize the template image once if the size is not changed
Size g_szPrior;
// resized template image
Mat g_ResizedTemplateMat;

// the resized center point of the two concentric circles
Point g_ptResizedCenter = Point(-1, -1);

// the resized radius of the two concentric circles
int g_iResizedRadiusSmall;
int g_iResizedRadiusBig;

int iMinSide = -1;
// for speed up the detection of the circle, resize the image
float g_fResizedRatio = 0.3;

///////// end of global variables ///////

static int iCount = 0;

// just for test use in windows
static int iFirstLocSuccess = 0;
static int iSecondLocSuccess = 0;



int IsCentersCoincide(Point ptDetection, int iRadius, int iMinSide)
{
 //   LOGD( "iRadius: %d\n", iRadius);
 //   LOGD( "g_iResizedRadiusBig: %d\n", g_iResizedRadiusBig );
 //   LOGD( "iMinSide: %d\n", iMinSide);
    if( abs(ptDetection.x - g_ptResizedCenter.x) < (iMinSide * 0.05) && 
        abs(ptDetection.y - g_ptResizedCenter.y) < (iMinSide * 0.05) &&
        (  
        abs(iRadius - g_iResizedRadiusBig) < (iMinSide * 0.05)
        ) )
    {

        return 0;
    }
    else
    {
        return -1;
    }
}

// first location
// detect circles in the image
// return 0 if location success
// return others if location failed.
int FirstLocation(Mat & srcImageFull, char* strFirstLocation)
{
    // shrinking image for speeding up process
    Size szDst = Size( srcImageFull.cols * g_fResizedRatio, 
        srcImageFull.rows * g_fResizedRatio );
    Mat srcImage( szDst, srcImageFull.type() );

    resize( srcImageFull, srcImage, szDst, 0, 0, INTER_LINEAR );

    // convert to grayscale image
    Mat midImage, dstImage;
    cvtColor(srcImage,midImage, COLOR_BGR2GRAY);
   // GaussianBlur( midImage, midImage, Size(3, 3), 2, 2 );

    // do hough circles
    int iMinHW = min(szDst.height, szDst.width);
    vector<Vec3f> circles;
    g_iResizedRadiusBig = iMinHW / 2;
    g_iResizedRadiusSmall = g_iResizedRadiusBig * 0.76;
    g_ptResizedCenter = Point( szDst.width/2, szDst.height/2 );


    HoughCircles( midImage, circles, CV_HOUGH_GRADIENT, 1, midImage.rows/8, 
        100, 100, g_iResizedRadiusBig * 0.9, midImage.rows/2);

    // acquiring center and raidus of detected circles
    Point center;
    int radius;
 //   LOGD( "circles.size(): %d\n", circles.size() );
    for( size_t i = 0; i < circles.size(); i++ )
    {
        // the center and radius
        center = Point(cvRound(circles[i][0]), cvRound(circles[i][1]));
        radius = cvRound(circles[i][2]);
        // draw center
        circle( midImage, center, 3, Scalar(0,0,0), -1, 8, 0 );
        // draw circle
        circle( midImage, center, radius, Scalar(0,0,0), 3, 8, 0 );
    }

    // imshow("circle", midImage);

    // location failed
    if( circles.size() == 0 )
    {
        return -1;
    }

    // check if the center point meet requirements
    int iret = IsCentersCoincide(center, radius, iMinHW);

    if (iret == 0)
    {
    	// LOGD( "iRadius: %d\n", radius);
    	// LOGD( "g_iResizedRadiusBig: %d\n", g_iResizedRadiusBig );
    	// LOGD( "iMinSide: %d\n", iMinHW );
      //   imwrite( strFirstLocation, srcImageFull);
    }

    // if success, return 0
    return iret;
}

// enhance watermark - method 1
void EnhanceWatermark( Mat & image,char* strWatermarkRegion,
                       char* strFinalExtracion )
{
    // split the image into RGBA channels
    Mat blue_channel, green_channel, red_channel;
    vector<Mat> channels;

    split(image, channels);

    blue_channel = channels.at(0);
    green_channel = channels.at(1);
    red_channel = channels.at(2);

    // histogram equalization
    equalizeHist( blue_channel, blue_channel);

    // merge back
    channels.at(0) = blue_channel;
    channels.at(1) = blue_channel;
    channels.at(2) = blue_channel;

    merge( channels, image );
}

// enhance watermark - method 2
void EnhanceWatermark2( Mat & image,char* strWatermarkRegion,
                        char* strFinalExtracion )
{
    // split the image into rgba channels
    Mat blue_channel, green_channel, red_channel;
    vector<Mat> channels;

    split(image, channels);
    blue_channel = channels.at(0);
    green_channel = channels.at(1);
    red_channel = channels.at(2);

    // get the mean and stddev values for each channels
    Scalar     mean;
    Scalar     stddev;

    meanStdDev(image, mean, stddev);

    // print these values
//    cout << mean << endl;
//    cout << stddev << endl;

    // result mat - 32 floating point
    Mat matResult(image.size(), CV_32SC1);
    // max and min value of the result mat
    int iMax = 0, iMin = 255;

    for(int r = 0; r < blue_channel.rows; r++)
    {
        for( int c = 0; c < blue_channel.cols; c++ )
        {
            uchar blue_value = blue_channel.at<uchar>(r, c);
            uchar green_value = green_channel.at<uchar>(r, c);
            uchar red_value = red_channel.at<uchar>(r, c);

            // blue
            if( blue_value < mean[0] / 1.5 )
            {
                blue_channel.at<uchar>(r, c) = mean[0];
            }
            // green
            if( green_value < mean[1] / 1.5 )
            {
                green_channel.at<uchar>(r, c) = mean[1];
            }
            // red
            if( red_value < mean[2] / 1.5 )
            {
                red_channel.at<uchar>(r, c) = mean[2];
            }

            //////////////////////////////////////////////////////////////////////////
            blue_value = blue_channel.at<uchar>(r, c);
            green_value = green_channel.at<uchar>(r, c);
            red_value = red_channel.at<uchar>(r, c);

            int diff = blue_value - (red_value + green_value) * 0.5;

            matResult.at<int>(r, c) = diff;

            if( diff >= iMax )
            {
                iMax = diff;
            }
            if( diff <= iMin )
            {
                iMin = diff;
            }
        }
    }

  //  cout << "max: " << iMax << endl;
 //   cout << "min: " << iMin << endl;

    Mat imgResult( matResult.size(), CV_8UC1);
    for(int r = 0; r < matResult.rows; r++)
    {
        for( int c = 0; c < matResult.cols; c++ )
        {
            int ires = matResult.at<int>(r, c);

            imgResult.at<uchar>(r, c) =
                ((float)(ires - iMin) / (float)(iMax - iMin)) * 255;
        }
    }

    equalizeHist( imgResult, imgResult );

    // merge back
    channels.at(0) = imgResult;
    channels.at(1) = imgResult;
    channels.at(2) = imgResult;

    merge( channels, image );
}


// scramble rgb image
int ScrambleImage( Mat & image, char* strWatermarkRegion,
                  char* strFinalExtracion )
{
    Mat tempMat = image.clone();

    // nine rectangles
    Rect upLeftRect = Rect(0, 0, image.cols/3, image.rows/3);
    Rect upMiddleRect = Rect(image.cols/3, 0, image.cols/3, image.rows/3 );
    Rect upRightRect = Rect( 2 * image.cols/3, 0, image.cols/3, image.rows/3);

    Rect middleLeftRect = Rect(0, image.rows/3, image.cols/3, image.rows/3);
    Rect middleMiddleRect = Rect(image.cols/3, image.rows/3, image.cols/3, image.rows/3);
    Rect middleRightRect = Rect(2 * image.cols/3, image.rows/3, image.cols/3, image.rows/3);

    Rect bottomLeftRect = Rect(0, 2 * image.rows/3, image.cols/3, image.rows/3 );
    Rect bottomMiddleRect = Rect(image.cols/3, 2 * image.rows/3, image.cols/3, image.rows/3);
    Rect bottomRightRect = Rect(2 * image.cols/3, 2 * image.rows/3, image.cols/3, image.rows/3);

    // nine subimages of tempMat
    Mat upLeftImage(tempMat, upLeftRect);
    Mat upMiddleImage(tempMat, upMiddleRect);
    Mat upRightImage(tempMat, upRightRect);

    Mat middleLeftImage(tempMat, middleLeftRect);
    Mat middleMiddleImage(tempMat, middleMiddleRect);
    Mat middleRightImage(tempMat, middleRightRect);

    Mat bottomLeftImage(tempMat, bottomLeftRect);
    Mat bottomMiddleImage(tempMat, bottomMiddleRect);
    Mat bottomRightImage(tempMat, bottomRightRect);
    //////////////////////////////////////////////////////////////////////////
    // nine subimages of real image 
    Mat upLeftImageReal(image, upLeftRect);
    Mat upMiddleImageReal(image, upMiddleRect);
    Mat upRightImageReal(image, upRightRect);

    Mat middleLeftImageReal(image, middleLeftRect);
    Mat middleMiddleImageReal(image, middleMiddleRect);
    Mat middleRightImageReal(image, middleRightRect);

    Mat bottomLeftImageReal(image, bottomLeftRect);
    Mat bottomMiddleImageReal(image, bottomMiddleRect);
    Mat bottomRightImageReal(image, bottomRightRect);

    //////////////////////////////////////////////////////////////////////////
    upLeftImage.copyTo( bottomRightImageReal );
    upMiddleImage.copyTo( bottomMiddleImageReal );
    upRightImage.copyTo( bottomLeftImageReal );

    middleLeftImage.copyTo( middleRightImageReal );
    // no middle middle
    middleRightImage.copyTo( middleLeftImageReal );

    bottomLeftImage.copyTo( upRightImageReal );
    bottomMiddleImage.copyTo( upMiddleImageReal );
    bottomRightImage.copyTo( upLeftImageReal );

    //////////////////////////////////////////////////////////////////////////

    tempMat.release();

    return 0;
}


int AddAlphaWeighted(Mat & src1, Mat & src2, float alpha, float beta, float gamma)
{
    // check the src and src have the same size
    if( src1.rows != src2.rows ||
        src1.cols != src2.cols )
    {
        return -1;
    }
    // check the two src have 4 channels
    if( src1.type() != CV_8UC4 ||
        src2.type() != CV_8UC4 )
    {
        return -2;
    }

    int width = src1.cols;
    int height = src1.rows;

    for( int r = 0; r < height; r++ )
    {
        uchar * pData1 = src1.ptr<uchar>(r);
        uchar * pData2 = src2.ptr<uchar>(r);
        for( int c = 0; c < width * 4; c += 4 )
        {
            // non-transparent
            if( pData2[c+3] != 0 )
            {
                pData1[c] = pData1[c] * alpha + pData2[c] * beta + gamma;
                pData1[c + 1] = pData1[c + 1] * alpha + pData2[c + 1] * beta + gamma;
                pData1[c + 2] = pData1[c + 2] * alpha + pData2[c + 2] * beta + gamma;
            }
        }
    }
    return 0;
}


void ExtractWatermark( Mat &correctedImage, char* strWatermarkRegion,
                       char* strFinalExtracion )
{
    // the original watermark region
    Point2i ptStartOrig = Point2f( 417, 1148);
    Point2i ptEndOrig = Point2f( 1510, 1448);

    int iTemplateOrigWidth = g_TemplateMat.cols;
    int iTemplateOrigHeight = g_TemplateMat.rows;

    // the resized watermark region
    Point2i ptStartReal = Point2i( ptStartOrig.x * ((float)iMinSide / iTemplateOrigWidth),
                ptStartOrig.y * ((float)iMinSide / iTemplateOrigHeight) );
    Point2i ptEndReal = Point2i( ptEndOrig.x * ((float)iMinSide / iTemplateOrigWidth),
                ptEndOrig.y * ((float)iMinSide / iTemplateOrigHeight) );

    Rect rectWatermark = Rect(ptStartReal, ptEndReal);
    Mat matWatermark = correctedImage(rectWatermark);

    // extract watermark
    ScrambleImage(matWatermark, strWatermarkRegion, strFinalExtracion);

    // draw red rectangle on the image
    rectangle( correctedImage, rectWatermark, Scalar(0, 0, 255 ), 3, 8, 0 );

    // imshow("correctedImage", correctedImage);
    // imshow("matWatermark", matWatermark);


}

int LocAndExtract( Mat & srcImage, char* strSecondLocation, char* strWatermarkRegion,
                  char* strFinalExtracion )
{
    // convert to grayscale image and blur
    Mat midImage, dstImage;
    cvtColor(srcImage, midImage, COLOR_BGR2GRAY);
    GaussianBlur( midImage, midImage, Size(9, 9), 2, 2 );

    // the radiuses of the two concentric circles in template image
    int iTemplateBigRadius = min(midImage.rows, midImage.cols)  / 2;
    int iTemplateSmallRadius = iTemplateBigRadius * 0.76;

    // mean radius
    int iMeanRadius = int((float)(iTemplateBigRadius + iTemplateSmallRadius)/2 + 0.5);

    // do hough circles
    vector<Vec3f> circles;
    HoughCircles( midImage, circles, CV_HOUGH_GRADIENT, 1, iTemplateBigRadius - iTemplateSmallRadius,
        100, 100, iMeanRadius, midImage.rows/2);

    // acquiring center and radius
    Point center;
    int radius;

    RNG rng;
    for( size_t i = 0; i < circles.size(); i++ )
    {
        center = Point(cvRound(circles[i][0]), cvRound(circles[i][1]));
        radius = cvRound(circles[i][2]);

        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        // draw center
        //      circle( srcImage, center, 3, color, -1, 8, 0 );
        // draw circle
        //   circle( srcImage, center, radius, color, 2, 8, 0 );
    }
    Point ptOrigin = Point( srcImage.cols / 2, srcImage.rows / 2 );
  //  circle( srcImage, ptOrigin, iTemplateBigRadius, Scalar(0, 0, 255), 2, 8, 0 );
    //     circle( srcImage, ptOrigin, iTemplateSmallRadius, Scalar(0, 0, 255), 2, 8, 0 );
    //
    // detect circles failed
    if(circles.size() == 0)
    {
        return -1;
    }

    // image width height
    // they should be equal
    int width = srcImage.cols;
    int height = srcImage.rows;

    // ratio
    float fRatio = (float)width / 1923.0;

    // the original watermark region
    Point2i ptStartOrig = Point2f( 417 * fRatio, 1148 * fRatio);
    Point2i ptEndOrig = Point2f( 1510 * fRatio, 1448 * fRatio);

    int idiffx = center.x - width/2;
    int idiffy = center.y - height/2;

    // the practical watermark region
    Point2i ptStartReal = Point2i(ptStartOrig.x + idiffx, ptStartOrig.y + idiffy);
    Point2i ptEndReal = Point2i(ptEndOrig.x + idiffx, ptEndOrig.y + idiffy);

    Rect rectWatermark = Rect(ptStartReal, ptEndReal);

    // the watermark part of the image
    Mat matWatermark = srcImage(rectWatermark);
    ScrambleImage( matWatermark, strWatermarkRegion, strFinalExtracion );

 //   Mat matWatermark2 = matWatermark.clone();

  //  EnhanceWatermark( matWatermark2, strWatermarkRegion, strFinalExtracion );
    EnhanceWatermark2( matWatermark, strWatermarkRegion, strFinalExtracion );
    // draw red rectangle on the image
  //  rectangle( srcImage, rectWatermark, Scalar(0, 0, 255 ), 3, 8, 0 );

    // imshow("srcImage", srcImage);
    // imshow("matWatermark", matWatermark);
 //   imwrite( strSecondLocation, srcImage );
 //   imwrite( strWatermarkRegion, matWatermark );
 //   imwrite( strFinalExtracion, matWatermark2 );

    return 0;
}


int ScrambledWMFilter( Mat &matBGRA, int height, int width, Mat &templateImage,
                      char* strOriginalFrame, char* strFirstLocation,
                      char* strSecondLocation, char* strWatermarkRegion,
                      char* strFinalExtracion)
{
    // get the minimum length of side
    iMinSide = height > width ? width : height;

    // read template image 
    if( g_bIsReadTemplateImage == false )
    {
        // read in the template image
        g_TemplateMat = templateImage;
        g_bIsReadTemplateImage = true;

    }
    // initialize the size
    if(g_szPrior.width == 0 && g_szPrior.height == 0)
    {
        g_szPrior = Size(iMinSide, iMinSide);

        // resize the template image
        g_ResizedTemplateMat = Mat(iMinSide, iMinSide, g_TemplateMat.type());
        resize( g_TemplateMat, g_ResizedTemplateMat, Size(iMinSide, iMinSide), 0, 0, INTER_NEAREST );
    }
    // the size has been changed
    else if(g_szPrior.width != iMinSide || g_szPrior.height != iMinSide)
    {
        g_szPrior = Size(iMinSide, iMinSide);

        // resize the template image
        g_ResizedTemplateMat = Mat(iMinSide, iMinSide, g_TemplateMat.type());
        resize( g_TemplateMat, g_ResizedTemplateMat, Size(iMinSide, iMinSide), 0, 0, INTER_NEAREST );
    }

    // roi of the source image
    Rect roi_rect;
    if(width > height)
    {
        int diff = width - height;
        roi_rect = Rect(diff/2, 0, iMinSide, iMinSide);
    }
    else
    {
        int diff = height - width;
        roi_rect = Rect(0, diff/2, iMinSide, iMinSide);
    }

    // first location: detect circles in the image
    // if return 0, means location success
    int iret = FirstLocation(matBGRA, strFirstLocation);

  //  LOGD("iret: %d\n", iret);

    // get the roi part of the image
    Mat roiMat;
    roiMat = matBGRA(roi_rect);

    // first location success
    if(iret == 0)
    {
       // printf("first location success. \t%d\n", iFirstLocSuccess);
        iFirstLocSuccess ++;

        // second location and extract watermark image
        int iSecondRet = LocAndExtract( roiMat, strSecondLocation,
            strWatermarkRegion, strFinalExtracion );
        if(iSecondRet == 0)
        {
           // printf("\nSecond location success.\n");
            return 0;
        }
        else
        {
        //    printf("\nSecond location failed.\n");
            return -1;
        }
    }
    // go on acquiring frame images to location
    else
    {
        AddAlphaWeighted(roiMat, g_ResizedTemplateMat, 0.2, 0.8, 0.0);
        return -2;
    }
}


}
