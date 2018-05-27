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

/*@note srcTri[]���������
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


/*@note ��ȡ�任����
**@para Mat* src:ԭʼͼ��
**@note ������ͷλ�ñ䶯�����������ú����ڲ���
*/
void getmap_matrix() {
	int dot[8] = { 276,256,402,256,438,378,248,378 };												
	Size newCanvas = Size(640, 640);//�����»����ֱ���

	int ratio = 4;//4������1����
	float side_len = 3.15;//��640pixel=160cm,4pixel=1cm��ratio=4;С����߳�side_len=3.15cm 
	int bias = 12.2;//�����40cm��Ϊͼ��ױߣ�60cm��Ϊ�ڰ׸���ߣ���=20cm   //chuangyi 47.3  59.5

				  /*����һ���������*/
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

	map_matrix = getPerspectiveTransform(srcTri, dstTri);		//�任����
	mapInv_matrix = getPerspectiveTransform(dstTri, srcTri);  //��ȡ͸�ӱ任��

}


/*note ����(�任��)ͼ�����꣬����õ�೵�����ߵ�ʵ������Point.x(cm)�;೵���������Point.y(cm)
*/
Point2f getRealXY(Point2f origin, int ratio = 4) {
	int width = 640, height = 640;//�����»����ֱ���
	float side_len = 3.15;//����ֵ��getmap_matrix()��ͬ
	int bias = 12.2;
	Point2f RealXY;
	RealXY.x = (origin.x - width / 2) / ratio;
	RealXY.y = ((height - bias*ratio) - origin.y) / ratio + 59.5;	//�� �˴���ֱ��height-y+�����ᵽͼ��ױ߾���
	//ע��59.5��ʵ�ʱ�עʱ���ڰ׸�ױ߾೵��O��59.5cm��
	return RealXY;
}


/*@brief ��ԭʼ���ö�Ӧ�任��
*@para Point2f src_p:ԭʼ��
*@para Mat map_matrix:�任����([3x3])
*@return Point2f:��Ӧ�任��
*/
Point2f GetCorrespondingPoint(Point2f src_p, Mat map_matrix) {
	float point[] = { src_p.x,src_p.y,1 };
	Mat warped_point(3, 1, CV_32FC1, point);				//����[x,y,1]

	float homo[3];
	Mat homogeneous(3, 1, CV_32FC1, homo);					//��Ӧ[x`,y`,1];

	map_matrix.convertTo(map_matrix, CV_32FC1);			//  mat1*mat2:only float types are accepted
	homogeneous = map_matrix*warped_point;				//[3x3]*[3x1]

	return Point2f(homo[0] / homo[2], homo[1] / homo[2]);
}

/*@brief һ�������ĵ�ͼƬ�任����
*@para sImag:����ͼ��
*@retun Mat:�任��ͼ��
*/
Mat prospective(Mat sImg) {
	Mat ImageIPM(640, 640, CV_32FC1);
	getmap_matrix();
	warpPerspective(sImg, ImageIPM, map_matrix, Size(640, 640));
	return ImageIPM;
}

Mat picture_prehandle(Mat srcImage)
{
	srcImage = prospective(srcImage);   //�ɸ��ݱ任��۲������������ֻ�����ض�������͸��
	cvtColor(srcImage, g_grayImage, COLOR_BGR2GRAY);
	//blur(grayImage, grayImage2, Size(3, 3));
	threshold(g_grayImage, g_grayImage, 180, 255, CV_THRESH_BINARY);   //��ֵ����

	return g_grayImage;
}



//������
void man()
{
	int a[10] = { 10,2,3,4,5,6,9,8,7,1 };
	int i, j, t;
	for (j = 0; j<10; j++)
		for (i = 0; i<10 - 1 - j; i++)
			if (a[i]<a[i + 1]) /* ��С����>,�ɴ�Сʱ��Ϊ< */
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
	int i = 0, sumi = 0, left = 0, right = 0, left2 = 0, right2 = 0, mode = 0;//����ɨ��
	for (; i < roi.cols; i++)
	{
		for (int j = 0; j < roi.rows; j++)
		{
			if (roi.at<uchar>(j, i) > 0)
				sumi++;
		}
		if (sumi > 3) //��������
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
		if (sumi < 3)   //��������
		{
			right = i;
			break;
		}
		sumi = 0;
	}
	if (right < (roi.cols / 2 - 2))//��������
	{
		i++; sumi = 0, mode = 1;
		for (; i < roi.cols; i++)
		{
			for (int j = 0; j < roi.rows; j++)
			{
				if (roi.at<uchar>(j, i) > 0)
					sumi++;
			}
			if (sumi > 3) //��������
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
			if (sumi < 3)   //��������
			{
				right2 = i;
				break;
			}
			sumi = 0;
		}
	}
	Mat roi1 = roi(Rect(left, 0, right - left, roi.rows));
	int j = 0, sumj = 0, top = 0, bottom = 0;//����ɨ��
	for (; j < roi1.rows; j++)
	{
		for (int i = 0; i < roi1.cols; i++)
		{
			if (roi1.at<uchar>(j, i) > 0)
				sumj++;
		}
		if (sumj > 3) //��������
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
		if (sumj < 3) //��������
		{
			bottom = j;
			break;
		}
		sumj = 0;
	}
	Mat roi2 = roi1(Rect(0, top, roi1.cols, bottom - top));
	int number1 = 2, t = 0, number2 = 0, min = 0;
	if (roi2.cols < 35)     //�ɵ���ֵ
	{
		number1 = 1;
	}
	else {
		for (int p = 2; p < 10; p++)
		{
			//char outputfile[70] = "D:/ԭD/����/������/�������/ͼ��/pic";
			//Im_write(outputfile, p, roi2);
			Mat res;
			resize(roi2, roi2, mat[p].size(), 0, 0, INTER_LINEAR);
			//char outputfile1[70] = "D:/ԭD/����/������/�������/ͼ��/pic2";
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
		if (roi4.cols < 35)     //�ɵ���ֵ
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
				//char outputfile2[70] = "D:/ԭD/����/������/�������/ͼ��/pic3";
				//Im_write(outputfile2, p, roi4);
				resize(roi4, roi4, mat[p].size(), 0, 0, INTER_LINEAR);
				//char outputfile3[70] = "D:/ԭD/����/������/�������/ͼ��/pic4";
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