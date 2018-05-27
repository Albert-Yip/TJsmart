#pragma GCC diagnostic error "-std=c++11"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
//#include "Windows.h" 
#include <stdio.h>  
#include"basichandle.h"
#include"prehandle.h"

using namespace cv;
using namespace std;
extern ROIData number;
extern Mat g_srcImage, g_srcImage1, g_grayImage,after,after2;

Mat mat[10];

Point2f leftup[20]; //= { 0 };//左上  红色  由于Linux下不能这样整体赋0 故在CornerPoint函数里全体赋0
Point2f rightup[20] ;//右上    绿色 全局变量未初始化的值是0  局部变量Debug版是填充字符, 比如可能是 0xCDCDCDCD 等
                                    //Release版是乱码, 也就是这个地方原来(在这之前被其他变量使用的时候)遗留下来的数据.
Point2f rightdown[20] ;//右下  蓝色
Point2f leftdown[20];//左下   黑色

int leftuppoint = 0, rightuppoint = 0, rightdownpoint = 0, leftdownpoint = 0;
int rec_width = 180;
//获得某点像素值
int get_pixel(Mat & img, Point pt) {
	if (pt.x < 0 || pt.y < 0 || pt.x >= img.cols || pt.y >= img.rows)
		return -1;
	int width = img.cols; //图片宽度
	int height = img.rows; //图片宽度t;//图片高度
	uchar* ptr = (uchar*)img.data + pt.y * width; //获得灰度值数据指针
	int intensity = ptr[pt.x];
	return intensity;
}

//延时拍照
void Fotographieren(int device, int time)
{
	VideoCapture capture(device);  //如果是笔记本，0打开的是自带的摄像头，1 打开外接的相机
	capture.set(CAP_PROP_SETTINGS, 1);	  //capture.set(CAP_PROP_SETTINGS, 1);//打开filter配置窗口，通常只要勾自动曝光就好了。
	Mat frame;
	int k = 0, num = 1;

	while (capture.isOpened())
	{
		capture >> frame;
		imshow("video", frame);

		int ch = cvWaitKey(20);//这里要定义一个不然后面重复赋值会出错，好像还不能定义在循环前,20ms拍一次
		char q[50] = "picture", *Pctur_name, Pctur_num[5];//q要大一点，不然数组越界


		sprintf(Pctur_num, "%d", num);
		Pctur_name = strcat(q, Pctur_num);//发现不能直接p = strcat("qqq" ,"0" )
		Pctur_name = strcat(Pctur_name, ".jpg");
		imwrite(Pctur_name, frame);//必须要有  .jpg不然出错，
		num++;


		if (ch == 27)//按ESC退出拍摄
			break;
		//Sleep(time);//time毫秒拍一次，可以自己设置
	}
}


//白像素统计
int bSums(Mat src)
{

	int counter = 0;
	//迭代器访问像素点
	Mat_<uchar>::iterator it = src.begin<uchar>();
	Mat_<uchar>::iterator itend = src.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)>0) counter += 1;//二值化后，像素点是0或者255
	}
	return counter;
}

//画出ROI区域
void drawROI(Mat & g_pBinaryImage, int x, int y, int width, int height) {
	rectangle(g_pBinaryImage, cvPoint(x, y), cvPoint(x + width, y + height), cvScalar(255, 255, 255), 1, 4, 0);
}

//画点:白点greyLevel=0；黑点greyLevel=255；
void drawCenter(Mat & g_pBinaryImage, Point2f center, int greyLevel, int size) {
	cv::circle(g_pBinaryImage, center, size, cv::Scalar(greyLevel, greyLevel, greyLevel), -1);
}

//读入图像
Mat Im_read(char q[], int num)
{
	char Pctur_name[100] = "55", Pctur_num[5];
	sprintf(Pctur_num, "%d", num);

	strcat(q, Pctur_num);
	strcat(q, ".jpg");
	return imread(q);
}

//读出保存图像
void Im_write(char p[], int num, Mat Image)
{
	char  *Pctur_name2, Pctur_num2[5], Pctur_num3[5], Pctur_num4[5];
	sprintf(Pctur_num2, "%d", num);
	//sprintf(Pctur_num4, "%d", num);
	sprintf(Pctur_num3, "%d", int(number.theta));
	strcat(p, Pctur_num2);
	strcat(p, "num");
	//strcat(p, Pctur_num4);
	strcat(p, Pctur_num3);
	strcat(p, ".jpg");
	imwrite(p, Image);

}

ROIData ROIset(Mat & g_pBinaryImage, int x, int y, int width, int height) {
	if (x < 0)x = 0;
	if (y < 0)y = 0;
	if (x >= g_pBinaryImage.cols)x = g_pBinaryImage.cols - 1;
	if (y >= g_pBinaryImage.rows)y = g_pBinaryImage.rows - 1;//可以改为越界不做或跳过而不是乱做
	if (x + width > g_pBinaryImage.cols)width = g_pBinaryImage.cols - x;
	if (y + height > g_pBinaryImage.rows)height = g_pBinaryImage.rows - y;
	Mat imageROI = g_pBinaryImage(Rect(x, y, width, height));
	ROIData A;
	Moments m2 = moments(imageROI, true);
	Point2f center2(m2.m10 / m2.m00, m2.m01 / m2.m00);
	double a2 = m2.m20 / m2.m00 - center2.x*center2.x;
	double b2 = m2.m11 / m2.m00 - center2.x*center2.y;
	double c2 = m2.m02 / m2.m00 - center2.y*center2.y;
	double theta2 = fastAtan2(2 * b2, (a2 - c2)) / 2;

	A.center.x = center2.x + x;
	A.center.y = center2.y + y;
	A.theta = theta2;
	return A;
}

int upline(Mat roi) //上横线检测 有返回1无返回0  1 5 7有
{
	int y = 0;
	int m = 0, n = 0;
	for (int j = 0; j < roi.rows / 4; j++)
	{   
		int k = 0,b=0;
		for(int i=0;i<roi.cols;i++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 50 && k == 0)
			{
				k++; b = i;
			}
			if ((i-b)==1)
			{
				if (f > 50)
				{
					k++; b = i;
					float p = float(k) / float(roi.cols);
					if (p > 0.74)
						break;
				}
				else
					k = 0;
			}
		}
		float p = float(k) / float(roi.cols);
		if (p > 0.74&&y == 0)
		{
			y++; m = j;
		}
		if ((j - m) == 1&&y == 1)
		{
			if (p > 0.74)
			{
				y++; n = j;
			}
			else
				y = 0;
		}
		if ((j - n) == 1&&y == 2 )
		{
			if (p > 0.74)
			{
				return 1;
			}
			else
				y = 0;
			
		}
	}
	return 0;
}

int leftline(Mat roi) //左竖线检测    0有
{
	int y = 0;
	int m = 0, n = 0;
	for (int i = 0; i < roi.cols / 3; i++)
	{
		int k = 0,b=0;
		for (int j = 0; j<roi.rows; j++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 50 && k == 0)
			{
				k++; b = j;
			}
			if ((j - b) == 1)
			{
				if (f >50)
				{
					k++; b = j;
					float p = float(k) / float(roi.rows);
					if (p > 0.65)
						break;
				}
				else
					k = 0;
			}
		}
		float p = float(k) / float(roi.rows);
		if (p > 0.65&&y == 0)
		{
			y++; m = i;
		}
		if ((i - m) == 1 && y == 1)
		{
			if (p > 0.65)
			{
				y++; n = i;
			}
			else
				y = 0;
		}
		if ((i - n) == 1 && y == 2)
		{
			if (p > 0.65)
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

int downline(Mat roi) //下横线检测      2、4有
{
	int y = 0;
	int m = 0, n = 0;
	for (int j = 2*roi.rows / 3; j < roi.rows; j++)
	{
		int k = 0,b=0;
		for (int i = 0; i<roi.cols; i++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 50 && k == 0)
			{
				k++; b = i;
			}
			if ((i - b) == 1)
			{
				if (f > 50)
				{
					k++; b = i;
					float p = float(k) / float(roi.cols);
					if (p > 0.89)
						break;
				}
				else
					k = 0;
			}
		}
		float p = float(k) / float(roi.cols);
		if (p > 0.89&&y == 0)
		{
			y++; m = j;
		}
		if ((j - m) == 1 && y == 1)
		{
			if (p > 0.89)
			{
				y++; n = j;
			}
			else
				y = 0;
		}
		if ((j - n) == 1 && y == 2)
		{
			if (p > 0.89)
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

int rightline(Mat roi) //右竖线检测    0 1 4有
{
	int y = 0;
	int m = 0, n = 0;
	for (int i = 2*roi.cols / 3; i < roi.cols; i++)
	{
		int k = 0, b = 0;
		for (int j = 0; j<roi.rows; j++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 50 && k == 0)
			{
				k++; b = j;
			}
			if ((j - b) == 1)
			{
				if (f > 50)
				{
					k++; b = j;
					float p = float(k) / float(roi.rows);
					if (p > 0.65)
						break;
				}
				else
					k = 0;
			}
		}
		float p = float(k) / float(roi.rows);
		if (p > 0.65&&y == 0)
		{
			y++; m = i;
		}
		if ((i - m) == 1 && y == 1)
		{
			if (p > 0.65)
			{
				y++; n = i;
			}
			else
				y = 0;
		}
		if ((i - n) == 1 && y == 2)
		{
			if (p > 0.65)
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

int leftuprect(Mat roi) //左上横贯间隙  1 2 3 4 7有
{
	int y = 0;
	int m = 0, n = 0;
	for (int j = 5; j < roi.rows / 2; j++)
	{
		int k = 0, b = 0,i=0;
		for ( ; i<roi.cols/2; i++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 0 )
			{
				k++; 
			}
			if (k > 2)
				break;
		}
		if (i> (roi.cols / 2-2)&&y==0)
		{
			y++; m = j;
		}
		if ((j - m) == 1)
		{
			if (i> (roi.cols / 2 - 2))
			{
				y++; n = j;
			}
			else
				y = 0;
		}
		if ((j - n) == 1 )
		{
			if (i> (roi.cols / 2 - 2))
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

int leftdownrect(Mat roi) //左下横贯间隙  1 2 3 4 5 7有
{
	int y = 0;
	int m = 0, n = 0;
	for (int j = roi.rows / 2; j < roi.rows ; j++)
	{
		int k = 0, b = 0, i = 0;
		for (; i<roi.cols / 2; i++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 0)
			{
				k++;
			}
			if (k > 2)
				break;
		}
		if (i> (roi.cols / 2 - 2) && y == 0)
		{
			y++; m = j;
		}
		if ((j - m) == 1)
		{
			if (i> (roi.cols / 2 - 2))
			{
				y++; n = j;
			}
			else
				y = 0;
		}
		if ((j - n) == 1)
		{
			if (i> (roi.cols / 2 - 2))
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

int rightuprect(Mat roi) //右上横贯间隙  5 6有
{
	int y = 0;
	int m = 0, n = 0;
	for (int j = 5; j < roi.rows / 2; j++)
	{
		int k = 0, b = 0, i = roi.cols / 2+2;
		for (; i<roi.cols ; i++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 0)
			{
				k++;
			}
			if (k > 2)
				break;
		}
		if (i> (roi.cols-2) && y == 0)
		{
			y++; m = j;
		}
		if ((j - m) == 1)
		{
			if (i> (roi.cols - 2))
			{
				y++; n = j;
			}
			else
				y = 0;
		}
		if ((j - n) == 1)
		{
			if (i> (roi.cols- 2))
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

int rightdownrect(Mat roi) //右下横贯间隙  2 7有
{
	int y = 0;
	int m = 0, n = 0;
	for (int j = roi.rows / 2; j < roi.rows; j++)
	{
		int k = 0, b = 0, i = roi.cols / 2+2;
		for (; i<roi.cols; i++)
		{
			int f = roi.at<uchar>(j, i);
			if (f > 0)
			{
				k++;
			}
			if (k > 2)
				break;
		}
		if (i> (roi.cols - 2) && y == 0)
		{
			y++; m = j;
		}
		if ((j - m) == 1)
		{
			if (i> (roi.cols- 2))
			{
				y++; n = j;
			}
			else
				y = 0;
		}
		if ((j - n) == 1)
		{
			if (i> (roi.cols - 2))
			{
				return 1;
			}
			else
				y = 0;

		}
	}
	return 0;
}

void CornerPoint(Mat g_srcImage1)
{
	cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);

	//设置角点检测参数  
	std::vector<cv::Point2f> corners;
	int max_corners = 200;
	double quality_level = 0.01;
	double min_distance = 20;//300   待调！！
	int block_size = 3;
	bool use_harris = 1;
	double k = 0.04;

	for (int i = 0; i < 20; i++)
	{
		leftup[i].x = 0; leftup[i].y = 0;
		rightup[i].x = 0; rightup[i].y = 0;
		rightdown[i].x = 0; rightdown[i].y = 0;
		leftdown[i].x = 0; leftdown[i].y = 0;
	}
	int Thre = 165;  //调



	//角点检测  
	cv::goodFeaturesToTrack(g_grayImage,
		corners,
		max_corners,
		quality_level,
		min_distance,
		cv::Mat(),
		block_size,
		use_harris,
		k);

	//分类，将检测到的角点绘制到原图上  
	int a = 0; int b = 0; int c = 0; int d = 0;
	for (int i = 0; i < corners.size(); i++)
	{   
		Point2f A, B, C, D;
		A.x = corners[i].x - 5; A.y = corners[i].y - 5;
		B.x = corners[i].x + 5; B.y = corners[i].y - 5;
		C.x = corners[i].x + 5; C.y = corners[i].y + 5;
		D.x = corners[i].x - 5; D.y = corners[i].y + 5;
		if (0 < A.x&&A.x < g_grayImage.cols && 0 < B.x&&B.x < g_grayImage.cols && 0 < C.x&&C.x < g_grayImage.cols && 0 < D.x&&D.x < g_grayImage.cols) {
			if (0 < A.y&&A.y < g_grayImage.rows && 0 < B.y&&B.y < g_grayImage.rows && 0 < C.y&&C.y < g_grayImage.rows && 0 < D.y&&D.y < g_grayImage.rows) {
				//cv::circle(g_srcImage1, corners[i], 2, cv::Scalar(0, 0, 255), 2, 8, 0);
				//cv::imshow("角点", g_srcImage1);
				//waitKey(2000);
				int k = g_grayImage.at<uchar>(A.y, A.x);
				int p = g_grayImage.at<uchar>(B.y, B.x);
				int q = g_grayImage.at<uchar>(C.y, C.x);
				int z = g_grayImage.at<uchar>(D.y, D.x);
				if (k < Thre&& p >= Thre&& q >= Thre&&z >= Thre)     //此处为一个较好看k p q z值的调试断点
				{
					leftup[a] = corners[i];
					cv::circle(g_srcImage1, corners[i], 2, cv::Scalar(0, 0, 255), 2, 8, 0);
					a++;
				}

				if (p < Thre&& k >= Thre&& q >= Thre&&z >= Thre)
				{
					rightup[b] = corners[i];
					cv::circle(g_srcImage1, corners[i], 2, cv::Scalar(0, 255, 0), 2, 8, 0);
					b++;
				}

				if (q < Thre&& k >= Thre&& p >= Thre&& z>= Thre)
				{
					rightdown[c] = corners[i];
					cv::circle(g_srcImage1, corners[i], 2, cv::Scalar(255, 0, 0), 2, 8, 0);
					c++;
				}

				if (z < Thre&&k >= Thre&&p >= Thre&&q >= Thre)
				{
					leftdown[d] = corners[i];
					cv::circle(g_srcImage1, corners[i], 2, cv::Scalar(0, 0, 0), 2, 8, 0);
					d++;
				}
			}
		}
		/*cv::circle(g_srcImage1, corners[i], 2, cv::Scalar(0, 0, 255), 2, 8, 0);
		cv::imshow("角点", g_srcImage1);
		waitKey(2000);*/
	}
}

//返回的是角度 和相应下标数组
thetaData getTheta(Mat g_pBinaryImage) {

	ROIData dataA[50];ROIData dataA1[10];ROIData dataA2[10];
	ROIData dataB[50]; ROIData dataB1[10]; ROIData dataB2[10];
	ROIData dataC[50]; ROIData dataC1[10]; ROIData dataC2[10];
	ROIData dataD[50]; ROIData dataD1[10]; ROIData dataD2[10];
	for (int i = 0; i < 50; i++)
	{
		dataA[i].center.x = 0; dataA[i].center.y = 0; dataB[i].center.x = 0; dataB[i].center.y = 0;
		dataC[i].center.y = 0; dataC[i].center.y = 0; dataA[i].theta = 0; dataB[i].theta = 0; dataC[i].theta = 0;
		dataD[i].center.x = 0; dataD[i].center.y = 0; dataD[i].theta = 0;
	}
	for (int i = 0; i < 10; i++)
	{
		dataA1[i].center.x = 0; dataA1[i].center.y = 0; dataB1[i].center.x = 0; dataB1[i].center.y = 0;
		dataC1[i].center.y = 0; dataC1[i].center.y = 0; dataA1[i].theta = 0; dataB1[i].theta = 0; dataC1[i].theta = 0;
		dataA2[i].center.x = 0; dataA2[i].center.y = 0; dataB2[i].center.x = 0; dataB2[i].center.y = 0;
		dataC2[i].center.y = 0; dataC2[i].center.y = 0; dataA2[i].theta = 0; dataB2[i].theta = 0; dataC2[i].theta = 0;
		dataD1[i].center.x = 0; dataD1[i].center.y = 0; dataD2[i].center.x = 0; dataD2[i].center.y = 0; dataD2[i].theta = 0; dataD1[i].theta = 0;
	}
	int a[10] = { 0 };	int aa[10] = { 0 };
	int b[10] = { 0 };	int bb[10] = { 0 };
	int c[10] = { 0 };	int cc[10] = { 0 };
	int d[10] = { 0 };	int dd[10] = { 0 };
	int det = 100;  //两ROI间角度差，待调节!!!
	leftuppoint = 0, rightuppoint = 0, rightdownpoint = 0, leftdownpoint = 0;
	Mat g_pBinaryImage2 = g_pBinaryImage.clone();

	int goodpointa = 0, mm = 0, goodpointb = 0,goodpointc = 0, goodpointd = 0;
	thetaData thetapointa = { 0 };
	thetaData thetapointb = { 0 };
	thetaData thetapointc = { 0 };
	thetaData thetapointd = { 0 };
	for (int qq = 0; leftup[qq].x != 0; qq++)
	{
		ROIData A;
		int z = 0;
		int i = leftup[qq].x + 5, j = leftup[qq].y - 2;

		for (; z < 40; z++)
		{
			A = ROIset(g_pBinaryImage, i - 5, j - 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
			dataA[z] = A;
			if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
				break;
			if ((dataA[z].center == dataA[z - 1].center) && z > 0)    //重心重复
				break;
			if (z > 0) 
			{
				int mm = abs(dataA[z].theta - dataA[z - 1].theta);
				if (mm > 15)              //此处为小框总数及角度差阈值调试的好断点
					break;
			}
			//drawROI(g_pBinaryImage2, i - 10, j - 10, 10, 10);		
			i = A.center.x; j = A.center.y;
		}
		if (z > 26&&z<43)
		{
			//cv::imshow("角点", g_pBinaryImage2);
			//waitKey(0);
			float averge = 0;
			for (int i = 0; i < z - 2; i++)
			{
				averge += dataA[i].theta;
			}
			averge = averge/(z - 2);
			dataA1[goodpointa].theta = averge;
			a[goodpointa] = qq;
			goodpointa++;
		}
		//cv::imshow("角点", g_pBinaryImage2);
		//waitKey(0);
	}
	if (goodpointa >= 2)
		{
			float averge=0,sum=0;
			int zhijiao = 0;
			for(int kk=0;kk<goodpointa;kk++)
			{ 
				if (fabs(dataA1[kk].theta - 90) < 0.8)
					zhijiao++;
				averge += dataA1[kk].theta;
			}
			averge = averge / goodpointa;
			for (int hh = 0; hh < goodpointa; hh++)
			{
				if (fabs(dataA1[hh].theta - averge) < 4)//5这个阈值待调
				{
					dataA2[mm] = dataA1[hh];
					thetapointa.point[mm] = a[hh];
					mm++;
					sum += dataA1[hh].theta;
				}
			}
			if (mm > 1)
			{
				sum = sum / mm; 
				leftuppoint = 1;
				if (zhijiao > 0)
				{
					thetapointa.theta = 90;
				}
				else
				thetapointa.theta = sum;
				thetapointa.num = mm;
				return thetapointa;
			}
		}
	
	if (mm < 2|| goodpointa<2)
	{
		float averge = 0, sum = 0;
		int aaa = 0, bbb = 0;
		for (int qq = 0; rightup[qq].x != 0; qq++)
	{
		ROIData A;
		int z = 0;
		int i = rightup[qq].x -1, j = rightup[qq].y - 2;

		for (; z < 40; z++)
		{
			A = ROIset(g_pBinaryImage, i-5, j - 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
			dataB[z] = A;
			if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
				break;
			if ((dataB[z].center == dataB[z - 1].center) && z > 0)    //重心重复
				break;
			if (z > 0)
			{
				int mm = abs(dataB[z].theta - dataB[z - 1].theta);
				if (mm > 15)              
					break;
			}
			//drawROI(g_pBinaryImage2, i - 5, j - 10, 10, 10);
			
			i = A.center.x; j = A.center.y; int controlNum = 0;
			while (g_pBinaryImage.at<uchar>(j, i) > 50)//为了降低杂点出现的情况
			{
				i++;
				controlNum++;
				if (controlNum > 5)
					break;
			}
		}//cv::imshow("角点", g_pBinaryImage2);
			//waitKey(10000);
		if (z >24 && z<43) //待调 ！！！  //原为33但角度较大时即框较斜时数目变大
		{
			float averge = 0;
			for (int i = 0; i < z - 2; i++)
			{
				averge += dataB[i].theta;
			}
			averge = averge / (z - 2);
			dataB1[goodpointb].theta = averge;
			b[goodpointb] = qq;
			goodpointb++;
		}
	}
		if ((goodpointa + goodpointb) > 1)
		{
			float averge = 0, sum = 0;
				int aaa=0,bbb=0,zhijiao=0;
			for (int kk = 0; kk<goodpointa; kk++)
			{
				if (fabs(dataA1[kk].theta - 90) < 0.8)
					zhijiao++;
				averge += dataA1[kk].theta;
			}
			for (int kk = 0; kk<goodpointb; kk++)
			{
				if (fabs(dataB1[kk].theta - 90) < 0.8)
					zhijiao++;
				averge += dataB1[kk].theta;
			}
			averge = averge / (goodpointa+goodpointb);
			for (int hh = 0; hh < goodpointa; hh++)
			{
				if (abs(dataA1[hh].theta - averge) < 5)//5这个阈值待调
				{
					dataA2[aaa] = dataA1[hh];
					thetapointa.point[aaa] = a[hh];
					aaa++;
					sum += dataA1[hh].theta;
				}
			}
			for (int hh = 0; hh < goodpointb; hh++)
			{
				if (abs(dataB1[hh].theta - averge) < 5)//5这个阈值待调
				{
					dataB2[bbb] = dataB1[hh];
					thetapointb.point[bbb] = b[hh];
					bbb++;
					sum += dataB1[hh].theta;
				}
			}
		
			if ((aaa+bbb)>1)
			{
				sum = sum / (aaa + bbb);
				if (zhijiao > 0)
				{
					thetapointa.theta = 90; thetapointb.theta = 90;
				}
				else
				{
					thetapointa.theta = sum; thetapointb.theta = sum;
				}
				if (aaa >= bbb)
				{
					leftuppoint = 1; 
					thetapointa.num = aaa;
					return thetapointa;
					
				}
				else
				{
					rightuppoint = 1;
					thetapointb.num = bbb;
					return thetapointb;
				}
			}
		}
		if ((goodpointa + goodpointb) < 2 || (aaa + bbb) < 2) 
		{
			for (int qq = 0; rightdown[qq].x != 0; qq++)
			{
				ROIData A;
				int z = 0;
				int i = rightdown[qq].x - 4, j = rightdown[qq].y + 2;

				for (; z < 40; z++)
				{
					A = ROIset(g_pBinaryImage, i - 1, j + 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
					dataC[z] = A;
					if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
						break;
					if ((dataC[z].center == dataC[z - 1].center) && z > 0)    //重心重复
						break;
					if (z > 0)
					{
						int mm = abs(dataC[z].theta - dataC[z - 1].theta);
						if (mm > 15)
							break;
					}
					//drawROI(g_pBinaryImage, i - 10, j - 10, 10, 10);
					//cv::imshow("角点", g_pBinaryImage);
					i = A.center.x; j = A.center.y;
				}
				if (z > 24 && z<43) //待调 ！！！
				{
					float averge = 0;
					for (int i = 0; i < z - 2; i++)
					{
						averge += dataC[i].theta;
					}
					averge = averge / (z - 2);
					dataC1[goodpointc].theta = averge;
					c[goodpointc] = qq;
					goodpointc++;
				}
			}
			int mm = goodpointa + goodpointb + goodpointc; int a1 = 0, b1 = 0, c1 = 0;
			if (mm > 1)
			{
			float averge = 0, sum = 0;
				 int zhijiao = 0;
				for (int kk = 0; kk<goodpointa; kk++)
				{
					if (fabs(dataA1[kk].theta - 90) < 0.8)
						zhijiao++;
					averge += dataA1[kk].theta;
				}
				for (int kk = 0; kk<goodpointb; kk++)
				{
					if (fabs(dataB1[kk].theta - 90) < 0.8)
						zhijiao++;
					averge += dataB1[kk].theta;
				}
				for (int kk = 0; kk<goodpointc; kk++)
				{
					if (fabs(dataC1[kk].theta - 90) < 0.8)
						zhijiao++;
					averge += dataC1[kk].theta;
				}
				averge = averge / mm;
				for (int hh = 0; hh < goodpointa; hh++)
				{
					if (abs(dataA1[hh].theta - averge) < 5)//5这个阈值待调
					{
						dataA2[a1] = dataA1[hh];
						thetapointa.point[a1] = a[hh];
						a1++;
						sum += dataA1[hh].theta;
					}
				}
				for (int hh = 0; hh < goodpointb; hh++)
				{
					if (abs(dataB1[hh].theta - averge) < 5)//5这个阈值待调
					{
						dataB2[b1] = dataB1[hh];
						thetapointb.point[b1] = b[hh];
						b1++;
						sum += dataB1[hh].theta;
					}
				}
				for (int hh = 0; hh < goodpointc; hh++)
				{
					if (abs(dataC1[hh].theta - averge) < 5)//5这个阈值待调
					{
						dataC2[b1] = dataC1[hh];
						thetapointc.point[c1] = c[hh];
						c1++;
						sum += dataC1[hh].theta;
					}
				}

				if ((a1 + b1+c1)>1)
				{
					Point2i t;
					sum = sum / (a1 + b1+c1);
					if (zhijiao > 0)
					{
						thetapointa.theta = 90; thetapointb.theta = 90; thetapointc.theta = 90;
					}
					else
					{
						thetapointa.theta = sum; thetapointb.theta = sum; thetapointc.theta = sum;
					}
					if (a1 >= b1&&a1 >= c1)
					{
						leftuppoint = 1; thetapointa.num = a1;
						return thetapointa;
					}
					else if(b1 >= a1&&b1 >= c1)
					{
						rightuppoint = 1; thetapointb.num = b1;
						return thetapointb;
					}
					else
					{
						rightdownpoint = 1; thetapointc.num = c1;
						return thetapointc;
					}
					
				}
			}
			if (mm <= 1 || (a1 + b1 + c1) <= 1)
			{
				for (int qq = 0; leftdown[qq].x != 0; qq++)
				{
					ROIData A;
					int z = 0;
					int i = leftdown[qq].x, j = leftdown[qq].y - 3;

					for (; z < 40; z++)
					{
						A = ROIset(g_pBinaryImage, i - 5, j + 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
						dataD[z] = A;
						if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
							break;
						if ((dataD[z].center == dataD[z - 1].center) && z > 0)    //重心重复
							break;
						if (z > 0)
						{
							int mm = abs(dataD[z].theta - dataD[z - 1].theta);
							if (mm > 15)
								break;
						}
						//drawROI(after2, i - 5, j + 10, 10, 10);
						
						i = A.center.x; j = A.center.y;
					}//cv::imshow("角点", after2); waitKey(0);
					if (z > 24 && z<43) //待调 ！！！
					{
						float averge = 0;
						for (int i = 0; i < z - 2; i++)
						{
							averge += dataD[i].theta;
						}
						averge = averge / (z - 2);
						dataD1[goodpointd].theta = averge;
						d[goodpointd] = qq;
						goodpointd++;
					}
				}
				int nn = goodpointa + goodpointb + goodpointc+goodpointd; int aa1 = 0, bb1 = 0, cc1 = 0,dd1=0;
				if (nn > 1)
				{
					float averge = 0, sum = 0;
					int zhijiao = 0;
					for (int kk = 0; kk<goodpointa; kk++)
					{
						if (fabs(dataA1[kk].theta - 90) < 0.8)
							zhijiao++;
						averge += dataA1[kk].theta;
					}
					for (int kk = 0; kk<goodpointb; kk++)
					{
						if (fabs(dataB1[kk].theta - 90) < 0.8)
							zhijiao++;
						averge += dataB1[kk].theta;
					}
					for (int kk = 0; kk<goodpointc; kk++)
					{
						if (fabs(dataC1[kk].theta - 90) < 0.8)
							zhijiao++;
						averge += dataC1[kk].theta;
					}
					for (int kk = 0; kk<goodpointd; kk++)
					{
						if (fabs(dataD1[kk].theta - 90) < 0.8)
							zhijiao++;
						averge += dataD1[kk].theta;
					}
					averge = averge / nn;
					for (int hh = 0; hh < goodpointa; hh++)
					{
						if (abs(dataA1[hh].theta - averge) < 5)//5这个阈值待调
						{
							dataA2[a1] = dataA1[hh];
							thetapointa.point[aa1] = a[hh];
							aa1++;
							sum += dataA1[hh].theta;
						}
					}
					for (int hh = 0; hh < goodpointb; hh++)
					{
						if (abs(dataB1[hh].theta - averge) < 5)//5这个阈值待调
						{
							dataB2[b1] = dataB1[hh];
							thetapointb.point[bb1] = b[hh];
							bb1++;
							sum += dataB1[hh].theta;
						}
					}
					for (int hh = 0; hh < goodpointc; hh++)
					{
						if (abs(dataC1[hh].theta - averge) < 5)//5这个阈值待调
						{
							dataC2[b1] = dataC1[hh];
							thetapointc.point[cc1] = c[hh];
							cc1++;
							sum += dataC1[hh].theta;
						}
					}
					for (int hh = 0; hh < goodpointd; hh++)
					{
						if (abs(dataD1[hh].theta - averge) < 5)//5这个阈值待调
						{
							dataD2[b1] = dataD1[hh];
							thetapointd.point[dd1] = d[hh];
							dd1++;
							sum += dataD1[hh].theta;
						}
					}

					if ((aa1 + bb1 + cc1+dd1)>1)
					{
						Point2i t;
						sum = sum / (aa1 + bb1 + cc1 + dd1);
						if (zhijiao > 0)
						{
							thetapointa.theta = 90; thetapointb.theta = 90; thetapointc.theta = 90; thetapointd.theta = 90;
						}
						else
						{
							thetapointa.theta = sum; thetapointb.theta = sum; thetapointc.theta = sum; thetapointd.theta = sum;
						}
						if (aa1 >= bb1&&aa1 >= cc1&&aa1>=dd1)
						{
							leftuppoint = 1; thetapointa.num = aa1;
							return thetapointa;
						}
						else if (bb1 >= aa1&&bb1 >= cc1&&bb1 >= dd1)
						{
							rightuppoint = 1; thetapointb.num = bb1;
							return thetapointb;
						}
						else if (cc1 >= aa1&&cc1 >= bb1&&cc1 >= dd1)
						{
							rightdownpoint = 1; thetapointc.num = cc1;
							return thetapointc;
						}
						else
						{
							leftdownpoint = 1; thetapointd.num = dd1;
							return thetapointd;
						}

					}
				}
			}
		}
	}
}
			
float angleangin(Mat after, Point2f k)
{
	if (leftuppoint)
	{
		ROIData A; ROIData dataA[40];
		int z = 0;
		int i = k.x + 2, j = k.y - 2;

		for (; z < 40; z++)
		{
			A = ROIset(after, i - 5, j - 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
			dataA[z] = A;
			if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
				break;
			if ((dataA[z].center == dataA[z - 1].center) && z > 0)    //重心重复
				break;
			if (z > 0)
			{
				int mm = abs(dataA[z].theta - dataA[z - 1].theta);
				if (mm > 15)              //此处为小框总数及角度差阈值调试的好断点
					break;
			}
			//drawROI(after2, i - 5, j - 10, 10, 10);		
			//cv::imshow("角点", after2);
			//waitKey(0);
			i = A.center.x; j = A.center.y;
		}
		float averge = 0;
		if (z > 26 && z<43)
		{
			//cv::imshow("角点", g_pBinaryImage2);
			//waitKey(0);
			
			for (int i = 0; i < z - 2; i++)
			{
				averge += dataA[i].theta;
			}
			averge = averge / (z - 2);
		}
		//cv::imshow("角点", g_pBinaryImage2);
		//waitKey(0);
		if (averge > 88 && averge < 92)
			return 0;
		else
			return averge - 90;
	}
	else if (rightuppoint)
	{
		ROIData A;ROIData dataB[40];
		int i =k.x - 1, j = k.y - 2,z=0;

		for (; z < 40; z++)
		{
			A = ROIset(after, i - 5, j - 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
			dataB[z] = A;
			if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
				break;
			if ((dataB[z].center == dataB[z - 1].center) && z > 0)    //重心重复
				break;
			if (z > 0)
			{
				int mm = abs(dataB[z].theta - dataB[z - 1].theta);
				if (mm > 15)
					break;
			}
			//drawROI(after2, i - 5, j - 10, 10, 10);

			i = A.center.x; j = A.center.y; int controlNum = 0;
			while (after.at<uchar>(j, i) > 50)//为了降低杂点出现的情况
			{
				i++;
				controlNum++;
				if (controlNum > 5)
					break;
			}
		}//cv::imshow("角点", after2);
		 //waitKey(5000);
		float averge = 0;
		if (z >24 && z<43) //待调 ！！！  //原为33但角度较大时即框较斜时数目变大
		{
			for (int i = 0; i < z - 2; i++)
			{
				averge += dataB[i].theta;
			}
			averge = averge / (z - 2);
		}
		if (averge > 88 && averge < 92)
			return 0;
		else
			return averge - 90;
	}
	else if (rightdownpoint)
	{
		ROIData A; ROIData dataC[40];
		int z = 0;
		int i =k.x - 4, j = k.y + 2;

		for (; z < 40; z++)
		{
			A = ROIset(after, i - 1, j + 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
			dataC[z] = A;
			if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
				break;
			if ((dataC[z].center == dataC[z - 1].center) && z > 0)    //重心重复
				break;
			if (z > 0)
			{
				int mm = abs(dataC[z].theta - dataC[z - 1].theta);
				if (mm > 15)
					break;
			}
			//drawROI(g_pBinaryImage, i - 10, j - 10, 10, 10);
			//cv::imshow("角点", g_pBinaryImage);
			i = A.center.x; j = A.center.y;
		}
		float averge = 0;
		if (z > 24 && z<43) //待调 ！！！
		{
			
			for (int i = 0; i < z - 2; i++)
			{
				averge += dataC[i].theta;
			}
			averge = averge / (z - 2);
		}
		if (averge > 88 && averge < 92)
			return 0;
		else
			return averge - 90;
	}
	else if (leftdownpoint)
	{
		ROIData A; ROIData dataD[40];
		int z = 0;
		int i = k.x , j = k.y -3;

		for (; z < 40; z++)
		{
			A = ROIset(after, i - 5, j + 10, 10, 10);//A = ROIset(g_pBinaryImage, i - 10, j - 20, 20, 20);
			dataD[z] = A;
			if (!(isnormal((float)A.center.x)) || !(isnormal((float)A.center.y)))//防止nan和-nan的干扰 
				break;
			if ((dataD[z].center == dataD[z - 1].center) && z > 0)    //重心重复
				break;
			if (z > 0)
			{
				int mm = abs(dataD[z].theta - dataD[z - 1].theta);
				if (mm > 15)
					break;
			}
			//drawROI(g_pBinaryImage, i - 5, j + 10, 10, 10);
			//cv::imshow("角点", g_pBinaryImage);
			i = A.center.x; j = A.center.y;
		}
		float averge = 0;
		if (z > 24 && z<43) //待调 ！！！
		{
			
			for (int i = 0; i < z - 2; i++)
			{
				averge += dataD[i].theta;
			}
			averge = averge / (z - 2);
		}
		if (averge > 88 && averge < 92)
			return 0;
		else
			return averge - 90;
	}
}
		
Mat ImageRotate(Mat src, Point2f center, double angle)
{
	//计算二维旋转的仿射变换矩阵  
	Mat M = getRotationMatrix2D(center, angle, 1);
	Mat dst;
	warpAffine(src, dst, M, cvSize(src.cols, src.rows), CV_INTER_LINEAR);
	return dst;
}

// 获取指定像素点放射变换后的新的坐标位置    
Point2f getPointAffinedPos(Point2f src, Point2f center, double angle)
{
	Point2f dst;
	int x = src.x - center.x;
	int y = src.y - center.y;
	double angle1 = angle * CV_PI / 180;

	dst.x = cvRound(x * cos(angle1) + y * sin(angle1) + center.x);
	dst.y = cvRound(-x * sin(angle1) + y * cos(angle1) + center.y);
	return dst;
}

//返回的是数字所在位置中心和数字的值
ROIData numbertest(Point2f k,Mat after) {
	int dets = 3500; //待调！！3500
	mat[0] = imread("0.jpg", CV_LOAD_IMAGE_UNCHANGED); mat[1]= imread("1.jpg", CV_LOAD_IMAGE_UNCHANGED);mat[2] = imread("2.jpg", CV_LOAD_IMAGE_UNCHANGED);  //注意不加flag默认以三通道读入！！
	mat[3] = imread("3.jpg", CV_LOAD_IMAGE_UNCHANGED); mat[4] = imread("4.jpg", CV_LOAD_IMAGE_UNCHANGED); mat[5] = imread("5.jpg", CV_LOAD_IMAGE_UNCHANGED);
	mat[6] = imread("6.jpg", CV_LOAD_IMAGE_UNCHANGED); mat[7] = imread("7.jpg", CV_LOAD_IMAGE_UNCHANGED); mat[8] = imread("8.jpg", CV_LOAD_IMAGE_UNCHANGED); mat[9] = imread("9.jpg", CV_LOAD_IMAGE_UNCHANGED);
	ROIData place;
	/*for (int i = 0; i < 10; i++)
	{
		int downlin = leftuprect(mat[i]);
		cout << downlin << endl;
	}*/
	if (leftuppoint)
	{
		if ((k.x - rec_width + 10) > 0 && (k.y - rec_width + 10) > 0)
		{
			Mat roi = after(Rect(k.x - rec_width + 10, k.y - rec_width + 10, rec_width - 20, rec_width - 20));
			 drawROI( after2, k.x - rec_width + 10, k.y - rec_width + 10, rec_width - 20, rec_width - 20);
			//cv::imshow("src", after2);
			//waitKey(2000);
			int num=bSums(roi);
			if (num < dets)
			{
				place.center.x = k.x - rec_width / 2;
				place.center.y = k.y - rec_width / 2;
				place.theta = getnumber(roi);
				return place;
			}
		}
	}
	else if (rightuppoint)
	{
		if ((k.x+rec_width-10) <after.cols && (k.y - rec_width + 10) > 0)
		{
			Mat roi = after(Rect(k.x + 10, k.y - rec_width + 10, rec_width - 20, rec_width - 20));
			drawROI(after2, k.x + 10, k.y - rec_width + 10, rec_width - 20, rec_width - 20);
			//cv::imshow("src", after2);
			//waitKey(3000);
			int num = bSums(roi);
			if (num < dets)
			{
				place.center.x = k.x + rec_width / 2;
				place.center.y = k.y - rec_width / 2;
				place.theta = getnumber(roi);
				return place;
			}
		}
	}
	else if (rightdownpoint)
	{
		if ((k.x + rec_width - 10) <after.cols && (k.y + rec_width - 10) <after.rows)
		{
			Mat roi = after(Rect(k.x +10, k.y +10, rec_width - 20, rec_width - 20));
			drawROI(after2, k.x + 10, k.y + 10, rec_width - 20, rec_width - 20);
			//cv::imshow("src", after);
			//waitKey(2000);
			int num = bSums(roi);
			if (num < dets)
			{
				place.center.x = k.x + rec_width / 2;
				place.center.y = k.y + rec_width / 2;
				place.theta = getnumber(roi);
				return place;
			}
		}
	}
	else if (leftdownpoint)
	{
		if ((k.x - rec_width + 10) >0 && (k.y + rec_width - 10) <after.rows)
		{
			Mat roi = after(Rect(k.x - rec_width + 10, k.y + 10, rec_width - 20, rec_width - 20));
			drawROI(after2, k.x - rec_width + 10, k.y + 10, rec_width - 20, rec_width - 20);
			//cv::imshow("src", after);
			//waitKey(2000);
			int num = bSums(roi);
			if (num < dets)
			{
				place.center.x = k.x + rec_width / 2;
				place.center.y = k.y + rec_width / 2;
				place.theta = getnumber(roi);
				return place;
			}
		}
	}
}

void ModeMake(Mat src)
{
	Mat  gar, bas;
	//src = imread("mode.jpg");
	//src=prospective(src);
	 cvtColor(src, gar, CV_BGR2GRAY);
	 threshold(gar, bas, 160, 255, CV_THRESH_BINARY);     //注意调阈值
	 int a = 325, b = 365, c = 60, d = 100;
	/* drawROI(bas, a,b, c, d);
	 cv::imshow("src",bas);
	 waitKey(0);*/
	 Mat roi = bas(Rect(a, b, c, d));
	// int t= roi.at<uchar>(80, 46);
	// cout << t;
	 int i = 0, sumi = 0, left = 0, right = 0;
	 for (; i < roi.cols;i++ )
	 {
		 for (int j = 0;  j < roi.rows;j++)
		 {
			 if (roi.at<uchar>(j, i) > 0)
				 sumi++;
		 }
		 if (sumi > 3) //待调！！
		 {
			 left = i;
			 break;
		 }
		 sumi = 0;
	 }
	 i += 10; sumi = 0;
	 for (; i < roi.cols;i++ )
	 {
		 for (int j = 0; j < roi.rows; j++)
		 {
			 if (roi.at<uchar>(j, i) > 0)
				 sumi++;
		 }
		 if (sumi < 3)   //待调！！
		 {
			 right = i;
			 break;
		 }
		 sumi = 0;
	 }
	 Mat roi1 = roi(Rect(left, 0, right - left, roi.rows));
	/* drawROI(bas, a+left, b, right - left, roi.rows);
	 cv::imshow("src", bas);
	 waitKey(0);*/
	 int j = 0, sumj = 0, top = 0, bottom = 0;//上下扫描
	 for (;  j<roi1.rows;j++)
	 {
		 for (int i = 0; i < roi1.cols;i++ )
		 {
			 if (roi1.at<uchar>(j, i) > 0)
				 sumj++;
		 }
		 if (sumj > 3) //待调！！
		 {
			 top = j;
			 break;
		 }
		 sumj = 0;
	 }
	 j += 30; sumj = 0;
	 for (; j<roi1.rows; j++)
	 {
		 for (int i = 0; i < roi1.cols;i++ )
		 {
			 if (roi1.at<uchar>(j, i) > 0)
				 sumj++;
		 }
		 if (sumj < 3) //待调！！
		 {
			 bottom = j;
			 break;
		 }
		 sumj = 0;
	 }
	 Mat roi2 = roi1(Rect(0, top, roi1.cols, bottom - top));
	 /*drawROI(bas, a + left, b+top, right - left, bottom - top);
	 cv::imshow("src", bas);
	 waitKey(0);*/
	 imwrite("modeafter.jpg", roi2);

}