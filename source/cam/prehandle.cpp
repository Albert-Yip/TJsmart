//#include "cv.h"
//#include "highgui.h"
//#include "cxcore.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
//#include "Windows.h" 
#include <stdio.h>  
#include"basichandle.h"

using namespace cv;
using namespace std;
extern Mat mat[10];

Mat map_matrix(3, 3, CV_32FC1);
Mat mapInv_matrix, map_x, map_y;
extern Mat g_srcImage, g_srcImage1, g_grayImage;

/*@note srcTri[]数组的配置
*/
void srcTri_config(Point2f srcTri[], int dot[]) {
	srcTri[0].x = dot[0];//276;				271
	srcTri[0].y = dot[1];//226;				241
	srcTri[1].x = dot[2];//405;				396
	srcTri[1].y = dot[3];//229;				241
	srcTri[2].x = dot[4];//447;				426
	srcTri[2].y = dot[5];//323;				332
	srcTri[3].x = dot[6];//247;				229
	srcTri[3].y = dot[7];//316;				330
}


/*@note 获取变换矩阵
**@para Mat* src:原始图像
**@note 在摄像头位置变动后，请自行设置函数内参数
*/
void getmap_matrix() {
	int dot[8] = { 276,256,402,256,438,378,248,378 };												
	Size newCanvas = Size(640, 640);//设置新画布分辨率

	int ratio = 4;//4个像素1厘米
	float side_len = 3.15;//∵640pixel=160cm,4pixel=1cm∴ratio=4;小方格边长side_len=3.15cm 
	int bias = 12.2;//距后轴40cm处为图像底边，60cm处为黑白格底线，Δ=20cm   //chuangyi 47.3  59.5

				  /*以下一般无需更改*/
	Point2f srcTri[4], dstTri[4];
	srcTri_config(srcTri, dot);

	int width = newCanvas.width, height = newCanvas.height;
	dstTri[0].x = width / 2 - side_len * 3 * ratio;
	dstTri[0].y = height - bias*ratio - side_len * 9 * ratio;
	dstTri[1].x = dstTri[0].x + side_len * 7 * ratio;
	dstTri[1].y = dstTri[0].y;
	dstTri[2].x = dstTri[1].x;
	dstTri[2].y = height - bias*ratio;
	dstTri[3].x = dstTri[0].x;
	dstTri[3].y = dstTri[2].y;

	map_matrix = getPerspectiveTransform(srcTri, dstTri);		//变换矩阵
	mapInv_matrix = getPerspectiveTransform(dstTri, srcTri);  //获取透视变换阵

}


/*note 输入(变换后)图像坐标，输出该点距车中心线的实际坐标Point.x(cm)和距车后轴的坐标Point.y(cm)
*/
Point2f getRealXY(Point2f origin, int ratio = 4) {
	int width = 640, height = 640;//设置新画布分辨率
	float side_len = 3.15;//参数值与getmap_matrix()相同
	int bias = 12.2;
	Point2f RealXY;
	RealXY.x = (origin.x - width / 2) / ratio;
	RealXY.y = ((height - bias*ratio) - origin.y) / ratio + 59.5;	//刘 此处可直接height-y+车后轴到图像底边距离
	//注：59.5：实际标注时，黑白格底边距车轴O点59.5cm；
	return RealXY;
}


/*@brief 由原始点获得对应变换点
*@para Point2f src_p:原始点
*@para Mat map_matrix:变换矩阵([3x3])
*@return Point2f:对应变换点
*/
Point2f GetCorrespondingPoint(Point2f src_p, Mat map_matrix) {
	float point[] = { src_p.x,src_p.y,1 };
	Mat warped_point(3, 1, CV_32FC1, point);				//构造[x,y,1]

	float homo[3];
	Mat homogeneous(3, 1, CV_32FC1, homo);					//对应[x`,y`,1];

	map_matrix.convertTo(map_matrix, CV_32FC1);			//  mat1*mat2:only float types are accepted
	homogeneous = map_matrix*warped_point;				//[3x3]*[3x1]

	return Point2f(homo[0] / homo[2], homo[1] / homo[2]);
}

/*@brief 一个完整的单图片变换例子
*@para sImag:输入图像
*@retun Mat:变换后图像
*/
Mat prospective(Mat sImg) {
	Mat ImageIPM(640, 640, CV_32FC1);
	getmap_matrix();
	warpPerspective(sImg, ImageIPM, map_matrix, Size(640, 640));
	return ImageIPM;
}

Mat picture_prehandle(Mat srcImage)
{
	srcImage = prospective(srcImage);   //可根据变换后观察可用清晰区域只进行特定区域逆透视
	cvtColor(srcImage, g_grayImage, COLOR_BGR2GRAY);
	//blur(grayImage, grayImage2, Size(3, 3));
	threshold(g_grayImage, g_grayImage, 180, 255, CV_THRESH_BINARY);   //阈值待调

	return g_grayImage;
}



//排序函数
void man()
{
	int a[10] = { 10,2,3,4,5,6,9,8,7,1 };
	int i, j, t;
	for (j = 0; j<10; j++)
		for (i = 0; i<10 - 1 - j; i++)
			if (a[i]<a[i + 1]) /* 由小到大>,由大到小时改为< */
			{
				t = a[i];
				a[i] = a[i + 1];
				a[i + 1] = t;
			}
	for (i = 0; i<10; i++)
		printf("%d ", a[i]);
	system("pause");
}

int getnumber(Mat roi)
{
	int mynumber;
	int i = 0, sumi = 0, left = 0, right = 0, left2 = 0, right2 = 0, mode = 0;//左右扫描
	for (; i < roi.cols; i++)
	{
		for (int j = 0; j < roi.rows; j++)
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
	for (; i < roi.cols; i++)
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
	if (right < (roi.cols / 2 - 2))//待调！！
	{
		i++; sumi = 0, mode = 1;
		for (; i < roi.cols; i++)
		{
			for (int j = 0; j < roi.rows; j++)
			{
				if (roi.at<uchar>(j, i) > 0)
					sumi++;
			}
			if (sumi > 3) //待调！！
			{
				left2 = i;
				break;
			}
			sumi = 0;
		}
		i += 10; sumi = 0;
		for (; i < roi.cols; i++)
		{
			for (int j = 0; j < roi.rows; j++)
			{
				if (roi.at<uchar>(j, i) > 0)
					sumi++;
			}
			if (sumi < 3)   //待调！！
			{
				right2 = i;
				break;
			}
			sumi = 0;
		}
	}
	Mat roi1 = roi(Rect(left, 0, right - left, roi.rows));
	int j = 0, sumj = 0, top = 0, bottom = 0;//上下扫描
	for (; j < roi1.rows; j++)
	{
		for (int i = 0; i < roi1.cols; i++)
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
	for (; j < roi1.rows; j++)
	{
		for (int i = 0; i < roi1.cols; i++)
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
	int number1 = 2, t = 0, number2 = 0, min = 0;
	if (roi2.cols < 35)     //可调阈值
	{
		number1 = 1;
	}
	else {
		for (int p = 2; p < 10; p++)
		{
			//char outputfile[70] = "D:/原D/车队/大三下/创意比赛/图像/pic";
			//Im_write(outputfile, p, roi2);
			Mat res;
			resize(roi2, roi2, mat[p].size(), 0, 0, INTER_LINEAR);
			//char outputfile1[70] = "D:/原D/车队/大三下/创意比赛/图像/pic2";
			//Im_write(outputfile1, p, roi2);
			absdiff(roi2, mat[p], res);
			int k = bSums(res);
			if (p == 2)
				min = k;
			if (k < min)
			{
				number1 = p;
				min = k;
			}
		}
	}
	if (number1 == 3)
	{
		if (leftuprect(roi2) || leftdownrect(roi2))
			;
		else
			number1 = 8;
	}
	if (number1 == 2)
	{
		if (downline(roi2) || leftuprect(roi2))
			;
		else
			number1 = 9;
	}
	if (mode == 1)
	{
		t = 0;
		Mat roi3 = roi(Rect(left2, 0, right2 - left2, roi.rows));
		Mat roi4 = roi3(Rect(0, top, roi3.cols, bottom - top));
		if (roi4.cols < 35)     //可调阈值
		{
			number2 = 1;
		}
		else {
			for (int p = 0; p < 10; p++)
			{
				if (p == 1)
				{
					continue;
				}
				Mat res;
				//char outputfile2[70] = "D:/原D/车队/大三下/创意比赛/图像/pic3";
				//Im_write(outputfile2, p, roi4);
				resize(roi4, roi4, mat[p].size(), 0, 0, INTER_LINEAR);
				//char outputfile3[70] = "D:/原D/车队/大三下/创意比赛/图像/pic4";
				//Im_write(outputfile3, p, roi4);
				absdiff(roi4, mat[p], res);
				//imshow("1", roi4);
				//waitKey(5000);
				//imshow("1", mat[p]);
				//waitKey(5000);
				int k = bSums(res);
				if (p == 0)
					min = k;
				if (k < min)
				{
					number2 = p;
					min = k;
				}

			}
		}
		if (number2 == 0)
		{
			if (leftline(roi4) || rightline(roi4))
				;
			else
				number2 = 8;
		}
		if (number2 == 3)
		{
			if (leftuprect(roi4) || leftdownrect(roi4))
				;
			else
				number2 = 8;
		}
		if (number2 == 2)
		{
			if (downline(roi4) || leftuprect(roi4))
				;
			else
				number2 = 9;
		}

		mynumber = number1 * 10 + number2;
		return mynumber;
	}
	mynumber = number1;
	return mynumber;
}