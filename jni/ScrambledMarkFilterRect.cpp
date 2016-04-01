#include "ScrambledMarkFilterRect.h"

extern "C" {

Mat g_correctionImage;

/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

double DistanceOfTwoPts( cv::Point pt1, cv::Point pt2 )
{
    return sqrt( pow(pt1.x - pt2.x, 2.) + pow( pt1.y - pt2.y, 2. ) );
}

int GetIndex(cv::Point pt, cv::Point* pptRect)
{
    double dMinDistance = 0xFFFFFFFFFFFFFFFF; 
    int iMinIndex = -1;
    for( int i = 0; i < 4; i++ )
    {
        double dTemp = DistanceOfTwoPts( pt, pptRect[i] );
        if( dTemp < dMinDistance )
        {
            dMinDistance = dTemp;
            iMinIndex = i;
        }
    }

    return iMinIndex;
}

int CheckVertices(std::vector<cv::Point> ptVertices )
{
    for( int i = 0; i < ptVertices.size(); i++ )
    {
        cv::Point pt1 = ptVertices[i];
        for(int j = 0; j < ptVertices.size(); j++)
        {
            if( i == j)
                continue;
            cv::Point pt2 = ptVertices[j];

            if( abs( pt1.x - pt2.x ) < 5 && 
                abs(pt1.y - pt2.y) < 5)
            {
                return -1;
            }
        }
    }
    return 0;
}

std::vector<cv::Point> GetFourVertices(std::vector<cv::Point>& contour, cv::Point* pptRect)
{
    int max_sumxy = 0, min_sumxy = 0x7FFFFFFF;
    int max_diffxy = -0x7FFFFFFF, max_diffyx = -0x7FFFFFFF;

    // four points( vertices )
    cv::Point ptTemp[4];

    for( int i = 0; i < contour.size(); i++ )
    {
        cv::Point pt = contour[i];

        int sumxy = pt.x + pt.y;
        int diff_xy = pt.x - pt.y;
        int diff_yx = pt.y - pt.x;


        if(max_sumxy < sumxy)
        {
            max_sumxy = sumxy;
            ptTemp[0] = pt;
        }
        if(min_sumxy > sumxy)
        {
            min_sumxy = sumxy;
            ptTemp[1] = pt;
        }

        if(max_diffxy < diff_xy)
        {
            max_diffxy = diff_xy;
            ptTemp[2] = pt;
        }
        if(max_diffyx < diff_yx)
        {
            max_diffyx = diff_yx;
            ptTemp[3] = pt;
        }
    }

    std::vector<cv::Point> ptVertices(4);
    for(int i = 0; i < 4; i++)
    {
        int iIndex = GetIndex( ptTemp[i], pptRect );
        ptVertices[iIndex] = ptTemp[i];
    }
    int iret = CheckVertices( ptVertices );
    if(iret != 0)
    {
        ptVertices[0] = cv::Point(-1, -1);
        return ptVertices;
    }

    return ptVertices;
}

int Correction( cv::Mat& inImg, std::vector<cv::Point>& contour, cv::Rect& rectLogo )
{
    // get bounding rectangle
    rectLogo = cv::boundingRect(contour);

    // get four points of the rectangle
    cv::Point* pptRect = (cv::Point*) malloc(sizeof(cv::Point) * 4);
    pptRect[0] = cv::Point(rectLogo.x, rectLogo.y);
    pptRect[1] = cv::Point(rectLogo.x + rectLogo.width, rectLogo.y);
    pptRect[2] = cv::Point(rectLogo.x, rectLogo.y + rectLogo.height);
    pptRect[3] = cv::Point(rectLogo.x + rectLogo.width, rectLogo.y + rectLogo.height);

    // get four vertices
    std::vector<cv::Point> ptVertices = GetFourVertices(contour, pptRect);

    if( ptVertices[0] == Point(-1, -1) )
    {
        free(pptRect);
        return -1;
    }

    AffinetCorrection( inImg, ptVertices, rectLogo );

    free( pptRect );
    return 1;
}

// scramble rgb image 
int ScrambleImage( Mat & image )
{
    Mat tempMat = image.clone();

    // nine rectangles
    cv::Rect upLeftRect = cv::Rect(0, 0, image.cols/3, image.rows/3);
    cv::Rect upMiddleRect = cv::Rect(image.cols/3, 0, image.cols/3, image.rows/3 );
    cv::Rect upRightRect = cv::Rect( 2 * image.cols/3, 0, image.cols/3, image.rows/3);

    cv::Rect middleLeftRect = cv::Rect(0, image.rows/3, image.cols/3, image.rows/3);
    cv::Rect middleMiddleRect = cv::Rect(image.cols/3, image.rows/3, image.cols/3, image.rows/3);
    cv::Rect middleRightRect = cv::Rect(2 * image.cols/3, image.rows/3, image.cols/3, image.rows/3);

    cv::Rect bottomLeftRect = cv::Rect(0, 2 * image.rows/3, image.cols/3, image.rows/3 );
    cv::Rect bottomMiddleRect = cv::Rect(image.cols/3, 2 * image.rows/3, image.cols/3, image.rows/3);
    cv::Rect bottomRightRect = cv::Rect(2 * image.cols/3, 2 * image.rows/3, image.cols/3, image.rows/3);

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


// enhance watermark - method 2
void EnhanceWatermark2( Mat & image )
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

            int diff = blue_value - (red_value + green_value) * 0.4;

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

    // inverse gray values
    //   Mat realResult(imgResult.rows, imgResult.cols, imgResult.type());
    //  absdiff( Mat(imgResult.rows, imgResult.cols, imgResult.type(), Scalar(255)), 
    //     imgResult, realResult);

    // merge back
    channels.at(0) = imgResult;
    channels.at(1) = imgResult;
    channels.at(2) = imgResult;

    merge( channels, image );
}

// enhance watermark - method 3
void EnhanceWatermark3( Mat & image)
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

            int diff = blue_value - (red_value + green_value) * 0.3;

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

    //////////////////////////////////////////////////////////////////////////

    // nine rectangles
    int height = imgResult.rows;
    int width = imgResult.cols;

    cv::Rect upLeftRect = cv::Rect(0, 0, width/3, height/3);
    cv::Rect upMiddleRect = cv::Rect( width/3, 0, width/3, height/3 );
    cv::Rect upRightRect = cv::Rect( 2 * width/3, 0, width/3, height/3);

    cv::Rect middleLeftRect = cv::Rect(0, height/3, width/3, height/3);
    cv::Rect middleMiddleRect = cv::Rect(width/3, height/3, width/3, height/3);
    cv::Rect middleRightRect = cv::Rect(2 * width/3, height/3, width/3, height/3);

    cv::Rect bottomLeftRect = cv::Rect(0, 2 * height/3, width/3, height/3 );
    cv::Rect bottomMiddleRect = cv::Rect(width/3, 2 * height/3, width/3, height/3);
    cv::Rect bottomRightRect = cv::Rect(2 * width/3, 2 * height/3, width/3, height/3);

    // nine subimages of tempMat
    Mat upLeftImage(imgResult, upLeftRect);
    Mat upMiddleImage(imgResult, upMiddleRect);
    Mat upRightImage(imgResult, upRightRect);

    Mat middleLeftImage(imgResult, middleLeftRect);
    Mat middleMiddleImage(imgResult, middleMiddleRect);
    Mat middleRightImage(imgResult, middleRightRect);

    Mat bottomLeftImage(imgResult, bottomLeftRect);
    Mat bottomMiddleImage(imgResult, bottomMiddleRect);
    Mat bottomRightImage(imgResult, bottomRightRect);

    equalizeHist( upLeftImage, upLeftImage );
    equalizeHist( upMiddleImage, upMiddleImage );
    equalizeHist( upRightImage, upRightImage );

    equalizeHist( middleLeftImage, middleLeftImage );
    equalizeHist( middleMiddleImage, middleMiddleImage );
    equalizeHist( middleRightImage, middleRightImage );

    equalizeHist( bottomLeftImage, bottomLeftImage );
    equalizeHist( bottomMiddleImage, bottomMiddleImage );
    equalizeHist( bottomRightImage, bottomRightImage );

    // merge back
    channels.at(0) = imgResult;
    channels.at(1) = imgResult;
    channels.at(2) = imgResult;

    merge( channels, image );

}

// enhance watermark - method 1
void EnhanceWatermark1( Mat & image)
{
    // split the image into rgba channels
    Mat blue_channel, green_channel, red_channel;
    vector<Mat> channels;

    split(image, channels);
    blue_channel = channels.at(0);


    equalizeHist( blue_channel, blue_channel );

    cv::adaptiveThreshold( blue_channel, blue_channel, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 19, 19 );
    //  

    // merge back
    channels.at(0) = blue_channel;
    channels.at(1) = blue_channel;
    channels.at(2) = blue_channel;

    merge( channels, image );
}

void AffinetCorrection( Mat& srcImage, std::vector<cv::Point>& ptVertics, Rect rect )
{
    // 这些情况 不需要校正
    if( abs(ptVertics[0].x - ptVertics[1].x) < 10  &&
        abs( ptVertics[2].x - ptVertics[3].x ) < 10 )
    {
        return;
    }
    //定义两组点，代表两个三角形
    Point2f srcTriangle[3];
    Point2f dstTriangle[3];

    //定义一些Mat变量
    Mat rotMat( 2, 3, CV_32FC1 );
    Mat warpMat( 2, 3, CV_32FC1 );


    // 设置目标图像的大小和类型与源图像一致
    g_correctionImage = Mat::zeros( srcImage.rows, srcImage.cols, srcImage.type() );

    //【3】设置源图像和目标图像上的三组点以计算仿射变换
    srcTriangle[0] = ptVertics[0];
    srcTriangle[1] =  ptVertics[1];
    srcTriangle[2] =  ptVertics[3];

    dstTriangle[0] = Point2f( rect.x, rect.y );
    dstTriangle[1] = Point2f( rect.x + rect.width, rect.y );
    dstTriangle[2] = Point2f( rect.x + rect.width, rect.y + rect.height );

    //【4】求得仿射变换
    warpMat = getAffineTransform( srcTriangle, dstTriangle );

    //【5】对源图像应用刚刚求得的仿射变换
    warpAffine( srcImage, g_correctionImage, warpMat, srcImage.size() );

    rotMat.release();
    warpMat.release();
}

int RectBasedLocation( Mat& src, cv::Rect& rectLogo)
{
    if( src.empty() )
    {
        return -3;
    }

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(src, gray, CV_BGR2GRAY);

    // Use Canny instead of threshold to catch squares with gradient shading
    cv::Mat bw;
    cv::adaptiveThreshold( gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 7, 7 );

    // Find contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(bw, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    std::vector<cv::Point> approx;

    int totalArea = src.rows * src.cols;
    double dRectAreaMax = RECT_AREA_MAX * totalArea;
    double dRectAreaMin = RECT_AREA_MIN * totalArea;

    for (int i = 0; i < contours.size(); i++)
    {
        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

        // Skip small or non-convex objects
        double dArea = std::fabs(cv::contourArea(contours[i]));
        if ( dArea < dRectAreaMin || dArea > dRectAreaMax || !cv::isContourConvex(approx))
            continue;

        // only focus on rectangle which have 4 vertices
        if (approx.size() == 4 )
        {
            // Number of vertices of polygonal curve
            int vtc = approx.size();

            // Get the cosines of all corners
            std::vector<double> cos;
            for (int j = 2; j < vtc+1; j++)
            {
                cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));
            }

            // Sort ascending the cosine values
            std::sort(cos.begin(), cos.end());

            // Get the lowest and the highest cosine
            double mincos = cos.front();
            double maxcos = cos.back();

            // Use the degrees obtained above and the number of vertices
            // to determine the shape of the contour
            if ( mincos >= -0.1 && maxcos <= 0.3)
            {
                int iret = Correction(src, contours[i], rectLogo);
                return iret;
            }
        }
    }

    return -2;
}

int ExtractWatermarkRect( cv::Mat& locImg, Rect& rectLogo )
{
    cv::Rect rectWatermark = cv::Rect( rectLogo.x * 0.99, rectLogo.y * 1.015 + rectLogo.height, 
        rectLogo.width * 1.025, rectLogo.height * 1.025 );

    Mat matWatermark = locImg( rectWatermark );

    ScrambleImage( matWatermark );
    // use method 2
    EnhanceWatermark2( matWatermark );

    return 0;
}


int ScrambledWMFilterRect( Mat &matBGRA, int height, int width, 
                          char* strOriginalFrame, char* strCorrFrame, 
                          char* strLocWatermark, char* strExtractResult )
{
	time_t t1, t2;
    Rect rectLogo;

    t1 = clock();
    int iret = RectBasedLocation(matBGRA, rectLogo);

    t2 = clock();
    // image is null
    // location failed
    if(iret == -3 || iret == -2 )
    {
    	LOGD("2 RectBasedLocation: %d    %ld ms", iret, (t2 - t1) / 1000);
    	return iret;
    }
    // affine transform failed
    else if(iret == -1)
    {
    	LOGD("2 RectBasedLocation: %d    %ld ms", iret, (t2 - t1) / 1000);
        return iret;
    }
    // location and correction success
    g_correctionImage.copyTo(matBGRA);
    ExtractWatermarkRect(matBGRA, rectLogo);

    LOGD("success");
    return 0;
}

}
