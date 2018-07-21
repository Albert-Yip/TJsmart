#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>
using namespace cv;
typedef struct
{
    Point2f center;
    Point2f center2;
    double theta;
} ROIData;

ROIData maindoCamera(Point2f yuanxin, int orientation);
ROIData maindoPicture();


