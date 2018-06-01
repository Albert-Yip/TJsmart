#pragma once
#include <opencv2/opencv.hpp>  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
using namespace cv;
using namespace std;

Mat prospective(Mat sImg);

Point2f getRealXY(Point2f origin, int ratio = 4);

int getnumber(Mat roi);