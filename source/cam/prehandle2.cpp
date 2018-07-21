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
extern int thre;
Mat map_matrix2(3, 3, CV_32FC1);
Mat mapInv_matrix2, map_x2, map_y2;
extern Mat g_srcImage2, g_srcImage1, g_grayImage2;

/*@note srcTri[]���������
*/
void srcTri_config2(Point2f srcTri[], int dot[]) {
	srcTri[0].x = dot[0];//276;				271
	srcTri[0].y = dot[1];//226;				241
	srcTri[1].x = dot[2];//405;				396
	srcTri[1].y = dot[3];//229;				241
	srcTri[2].x = dot[4];//447;				426
	srcTri[2].y = dot[5];//323;				332
	srcTri[3].x = dot[6];//247;				229
	srcTri[3].y = dot[7];//316;				330
}


/*@note ��ȡ�任����
**@para Mat* src:ԭʼͼ��
**@note ������ͷλ�ñ䶯�����������ú����ڲ���
*/
void getmap_matrix2() {
	int dot[8] = { 261,323,387,324,422,470,231,469 };
	Size newCanvas = Size(640, 640);//�����»����ֱ���

	int ratio = 4;//4������1����
	float side_len = 3.1;//��640pixel=160cm,4pixel=1cm��ratio=4;С����߳�side_len=3.15cm 
	int bias = 1.2;//�����40cm��Ϊͼ��ױߣ�60cm��Ϊ�ڰ׸���ߣ���=20cm   //chuangyi 51.8  52.8

				   /*����һ���������*/
	Point2f srcTri[4], dstTri[4];
	srcTri_config2(srcTri, dot);

	int width = newCanvas.width, height = newCanvas.height;
	dstTri[0].x = width / 2 - side_len *3* ratio;
	dstTri[0].y = height - bias*ratio - side_len * 9 * ratio;
	dstTri[1].x = dstTri[0].x + side_len * 7 * ratio;
	dstTri[1].y = dstTri[0].y;
	dstTri[2].x = dstTri[1].x;
	dstTri[2].y = height - bias*ratio;
	dstTri[3].x = dstTri[0].x;
	dstTri[3].y = dstTri[2].y;

	map_matrix2 = getPerspectiveTransform(srcTri, dstTri);		//�任����
	mapInv_matrix2 = getPerspectiveTransform(dstTri, srcTri);  //��ȡ͸�ӱ任��

}


/*note ����(�任��)ͼ�����꣬����õ�೵�����ߵ�ʵ������Point.x(cm)�;೵���������Point.y(cm)
*/
Point2f getRealXY2(Point2f origin, int ratio = 4) {
	int width = 640, height = 640;//�����»����ֱ���
	float side_len = 3.1;//����ֵ��getmap_matrix2()��ͬ
	int bias =1.2;
	Point2f RealXY;
	RealXY.x = (origin.x - width / 2) / ratio;//-(origin.x - width / 2) / ratio;
	RealXY.y = ((height - bias*ratio) - origin.y) / ratio + 33.6;//-(((height - bias*ratio) - origin.y) / ratio + 38.3);	//�� �˴���ֱ��height-y+�����ᵽͼ��ױ߾���
																//ע��59.5��ʵ�ʱ�עʱ���ڰ׸�ױ߾೵��O��59.5cm��
	return RealXY;
}

Point2f getPoint2(Point2f src, Point2f center, double angle,int flag)
{
	Point2f dst;
	if (flag == 0)
	{
		src.x = -src.x;
		src.y = -src.y;
	}
	if (flag == 1)
	{
		float t = src.x;
		src.x = src.y;
		src.y = -t;
	}
	if (flag == 2)
	{
		float t = src.x;
		src.x = -src.y;
		src.y = t;
	}
	if (angle < 90) {
		double angle1 = angle * CV_PI / 180;

		dst.x = -(src.x+src.y / tan(angle1))*sin(angle1);
		dst.y = src.y / sin(angle1)-(src.x+ src.y / tan(angle1))*cos(angle1);
		return dst;
	}
	if (angle > 90)
	{
		double angle1 = (angle - 90) * CV_PI / 180;

		dst.x = (-src.x + src.y*tan(angle1))*cos(angle1);
		dst.y = src.y / cos(angle1) + (src.x - src.y*tan(angle1))*sin(angle1);
		return dst;
	}
	else {
		dst.x = -src.x;
		dst.y = src.y;
		return dst;
	}
}


/*@brief һ�������ĵ�ͼƬ�任����
*@para sImag:����ͼ��
*@retun Mat:�任��ͼ��
*/
Mat prospective2(Mat sImg) {
	Mat ImageIPM(640, 640, CV_32FC1);
	getmap_matrix2();
	warpPerspective(sImg, ImageIPM, map_matrix2, Size(640, 640));
	return ImageIPM;
}



Mat map_matrix3(3, 3, CV_32FC1);
Mat mapInv_matrix3, map_x3, map_y3;

/*@note srcTri[]���������
*/
void srcTri_config3(Point2f srcTri[], int dot[]) {
	srcTri[0].x = dot[0];//276;				271
	srcTri[0].y = dot[1];//226;				241
	srcTri[1].x = dot[2];//405;				396
	srcTri[1].y = dot[3];//229;				241
	srcTri[2].x = dot[4];//447;				426
	srcTri[2].y = dot[5];//323;				332
	srcTri[3].x = dot[6];//247;				229
	srcTri[3].y = dot[7];//316;				330
}


/*@note ��ȡ�任����
**@para Mat* src:ԭʼͼ��
**@note ������ͷλ�ñ䶯�����������ú����ڲ���
*/
void getmap_matrix3() {
	int dot[8] = { 270,273,396,274,423,413,234,407 };
	Size newCanvas = Size(640, 640);//�����»����ֱ���

	int ratio = 4;//4������1����
	float side_len = 3.15;//��640pixel=160cm,4pixel=1cm��ratio=4;С����߳�side_len=3.15cm 
	int bias = 8.4;//�����40cm��Ϊͼ��ױߣ�60cm��Ϊ�ڰ׸���ߣ���=20cm   //chuangyi 51.8  52.8

				   /*����һ���������*/
	Point2f srcTri[4], dstTri[4];
	srcTri_config3(srcTri, dot);

	int width = newCanvas.width, height = newCanvas.height;
	dstTri[0].x = width / 2 - side_len * 3 * ratio;
	dstTri[0].y = height - bias*ratio - side_len * 9 * ratio;
	dstTri[1].x = dstTri[0].x + side_len * 7 * ratio;
	dstTri[1].y = dstTri[0].y;
	dstTri[2].x = dstTri[1].x;
	dstTri[2].y = height - bias*ratio;
	dstTri[3].x = dstTri[0].x;
	dstTri[3].y = dstTri[2].y;

	map_matrix3 = getPerspectiveTransform(srcTri, dstTri);		//�任����
	mapInv_matrix3 = getPerspectiveTransform(dstTri, srcTri);  //��ȡ͸�ӱ任��

}


/*note ����(�任��)ͼ�����꣬����õ�೵�����ߵ�ʵ������Point.x(cm)�;೵���������Point.y(cm)
*/
Point2f getRealXY3(Point2f origin, int ratio = 4) {
	int width = 640, height = 640;//�����»����ֱ���
	float side_len = 3.15;//����ֵ��getmap_matrix()��ͬ
	int bias = 8.4;
	Point2f RealXY;
	RealXY.y = (origin.x - width / 2) / ratio;
	RealXY.x = -(((height - bias*ratio) - origin.y) / ratio + 35.3);	//�� �˴���ֱ��height-y+�����ᵽͼ��ױ߾���
																	//ע��59.5��ʵ�ʱ�עʱ���ڰ׸�ױ߾೵��O��59.5cm��
	return RealXY;
}



/*@brief һ�������ĵ�ͼƬ�任����
*@para sImag:����ͼ��
*@retun Mat:�任��ͼ��
*/
Mat prospective3(Mat sImg) {
	Mat ImageIPM(640, 640, CV_32FC1);
	getmap_matrix3();
	warpPerspective(sImg, ImageIPM, map_matrix3, Size(640, 640));
	return ImageIPM;
}


//������ͷ
Mat map_matrix4(3, 3, CV_32FC1);
Mat mapInv_matrix4, map_x4, map_y4;

/*@note srcTri[]���������
*/
void srcTri_config4(Point2f srcTri[], int dot[]) {
	srcTri[0].x = dot[0];//276;				271
	srcTri[0].y = dot[1];//226;				241
	srcTri[1].x = dot[2];//405;				396
	srcTri[1].y = dot[3];//229;				241
	srcTri[2].x = dot[4];//447;				426
	srcTri[2].y = dot[5];//323;				332
	srcTri[3].x = dot[6];//247;				229
	srcTri[3].y = dot[7];//316;				330
}


/*@note ��ȡ�任����
**@para Mat* src:ԭʼͼ��
**@note ������ͷλ�ñ䶯�����������ú����ڲ���
*/
void getmap_matrix4() {
	int dot[8] = { 298,259,420,256,465,379,286,382 };
	Size newCanvas = Size(640, 640);//�����»����ֱ���

	int ratio = 4;//4������1����
	float side_len = 3.15;//��640pixel=160cm,4pixel=1cm��ratio=4;С����߳�side_len=3.15cm 
	int bias = 6.7;//�����40cm��Ϊͼ��ױߣ�60cm��Ϊ�ڰ׸���ߣ���=20cm   //chuangyi 51.8  52.8

				   /*����һ���������*/
	Point2f srcTri[4], dstTri[4];
	srcTri_config4(srcTri, dot);

	int width = newCanvas.width, height = newCanvas.height;
	dstTri[0].x = width / 2 - side_len * 4 * ratio;
	dstTri[0].y = height - bias*ratio - side_len * 9 * ratio;
	dstTri[1].x = dstTri[0].x + side_len * 7 * ratio;
	dstTri[1].y = dstTri[0].y;
	dstTri[2].x = dstTri[1].x;
	dstTri[2].y = height - bias*ratio;
	dstTri[3].x = dstTri[0].x;
	dstTri[3].y = dstTri[2].y;

	map_matrix4 = getPerspectiveTransform(srcTri, dstTri);		//�任����
	mapInv_matrix4 = getPerspectiveTransform(dstTri, srcTri);  //��ȡ͸�ӱ任��

}


/*note ����(�任��)ͼ�����꣬����õ�೵�����ߵ�ʵ������Point.x(cm)�;೵���������Point.y(cm)
*/
Point2f getRealXY4(Point2f origin, int ratio = 4) {
	int width = 640, height = 640;//�����»����ֱ���
	float side_len = 3.15;//����ֵ��getmap_matrix()��ͬ
	int bias = 6.7;
	Point2f RealXY;
	RealXY.y = -(origin.x - width / 2) / ratio;
	RealXY.x = ((height - bias*ratio) - origin.y) / ratio + 40.8;	//�� �˴���ֱ��height-y+�����ᵽͼ��ױ߾���
																	//ע��59.5��ʵ�ʱ�עʱ���ڰ׸�ױ߾೵��O��59.5cm��
	return RealXY;
}



/*@brief һ�������ĵ�ͼƬ�任����
*@para sImag:����ͼ��
*@retun Mat:�任��ͼ��
*/
Mat prospective4(Mat sImg) {
	Mat ImageIPM(640, 640, CV_32FC1);
	getmap_matrix4();
	warpPerspective(sImg, ImageIPM, map_matrix4, Size(640, 640));
	return ImageIPM;
}



