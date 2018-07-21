#pragma once
#include <opencv2/opencv.hpp>  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
using namespace cv;
using namespace std;


void CornerPoint(Mat g_srcImage1);

void Fotographieren(int device, int time);

typedef struct
{
	Point2f center;
	Point2f center2;
	double theta;
} ROIData;

typedef struct
{
	float theta;
	int point[10];
	int num;
} thetaData;

void drawROI(Mat & g_pBinaryImage, int x, int y, int width, int height);


thetaData getTheta(Mat g_pBinaryImage);

Mat ImageRotate(Mat src, Point2f center, double angle);

Point2f getPointAffinedPos(Point2f src, Point2f center, double angle);

void drawCenter(Mat & g_pBinaryImage, Point2f center, int greyLevel, int size);

ROIData numbertest(Point2f k, Mat after);

void ModeMake(Mat src);

float angleangin(Mat after, Point2f k);

Mat Im_read(char q[], int num);

void Im_write(char p[], int num, Mat Image);

int upline(Mat roi);

int leftline(Mat roi);

int downline(Mat roi);

int rightline(Mat roi);

int leftuprect(Mat roi);

int leftdownrect(Mat roi);

int rightuprect(Mat roi);

int rightdownrect(Mat roi);

int centercross(Mat roi);

int bSums(Mat src);

Point2f getPointBack(Point2f src, Point2f center, double angle);

Point2f getPoint(Point2f src, Point2f center, double angle, int flag);

