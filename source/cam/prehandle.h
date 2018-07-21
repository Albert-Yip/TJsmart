#pragma once
#include <opencv2/opencv.hpp>  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
using namespace cv;
using namespace std;

Mat prospective(Mat sImg);

Point2f getRealXY(Point2f origin, int ratio = 4);

int getnumber(Mat roi);

Mat prospective2(Mat sImg);

Point2f getRealXY2(Point2f origin, int ratio = 4);

Mat prospective3(Mat sImg);

Mat prospective4(Mat sImg);

Point2f getRealXY3(Point2f origin, int ratio = 4);

Point2f getRealXY4(Point2f origin, int ratio = 4);

Point2f getPoint2(Point2f src, Point2f center, double angle, int flag);