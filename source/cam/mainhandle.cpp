#include <opencv2/opencv.hpp>  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include"basichandle.h"
#include"prehandle.h"
using namespace cv;
using namespace std;
#include <time.h> 
//resize把原图变小再操作？ 利用特征或已知把框取得更小 
//加入阈值调节 注意模板阈值与数字阈值一致 阈值调节后则是否障碍的阈值也变了 模型制作函数完善
// 最后加入避障如果比赛现场测试发现双面胶贴障碍没用则使用
//rec_width调节   
//对棋子 挡板的判断
//函数的封装          上传前注意读存位置及把main注  second 两个摄像头的事 nothing是太糊 动态阈值？
//旋转后坐标变换 给点方式 ROI2的中心？

ROIData number;
extern int thre;
int richtig =0;int doright = 0; int comp = 0;
int dets = 4800;

Mat g_srcImage, g_srcImage1, g_grayImage, g_pBinaryImage,g_pBinaryImage1,after,after2,Image, Image2;
extern Point2f leftup[20];//左上     红色
extern Point2f rightup[20];//右上    绿色
extern Point2f rightdown[20];//右下  蓝色
extern Point2f leftdown[20] ;//左下   黑色
extern int leftuppoint , rightuppoint , rightdownpoint , leftdownpoint ;
extern int rec_width;
int num = 1;ROIData last;
ROIData maindoPicture()
{
	ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
	
	//连续图片读取处理
	
	for (; num < 675;num++) {
		double start, end, cost;
		start = clock();
		char filename[70] = "D:/原D/车队/大三下/创意比赛/图像/";
		//char filename[70] = "C:/Users/帮主/Desktop/新建文件夹 (2)/";
		g_srcImage = Im_read(filename, num);
		if (g_srcImage.data == 0)
			continue;
		/*VideoCapture cap1;
		VideoCapture cap2;
		cap2.open(1); cap1.open(2);

		while (1) {
			cap2 >> Image;
			cap1 >> g_srcImage1;
			imshow("1", Image); imshow("2", g_srcImage1);
			waitKey(2000);
		}*/
		


	//g_srcImage = imread("D:/原D/车队/大三下/创意比赛/图像/12.jpg");

	if (!g_srcImage.data)
	{
		printf("读取图片错误！ \n");
		continue;
		//return nothing;
	}
	g_srcImage1 = prospective(g_srcImage);
	
	//CornerPoint(g_srcImage1);

	//threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图
	//g_pBinaryImage1 = g_pBinaryImage.clone();

	cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);
	threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图
	g_pBinaryImage1 = g_pBinaryImage.clone();
	CornerPoint(g_pBinaryImage);

	thetaData thedata=getTheta(g_pBinaryImage);

	Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
	Point2f k;
	after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);//theta.x是旋转角度//thedata.theta-90
	float angle= thedata.theta;
	after2 = after.clone();
	//cv::imshow("旋转", after2);
	//waitKey(0);
	if (leftuppoint)
	{
		Point2f a[10];
		for (int i = 0; i < 10; i++)
		{
			a[i].x = 0; a[i].y = 0;
		}
		for (int n = 0; n < thedata.num; n++)
			a[n] = getPointAffinedPos(leftup[thedata.point[n]], center, thedata.theta - 90); 
		int i, j;
		Point2f t;
		for (j = 0; j < 10; j++)
		{
			for (i = 0; i < 10 - 1 - j; i++)
			{
				if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小< */
				{
					t = a[i];
					a[i] = a[i + 1];
					a[i + 1] = t;
				}
			}
		}
		if (thedata.num > 1)
		{
			if ((a[0].x < a[1].x)&&fabs(a[0].y-a[1].y)<50)
			{
				Point2f w;
				w = a[0];
				a[0] = a[1];
				a[1] = w;
			}
		}
		for (int n = 0; n < thedata.num; n++) 
		{
			if ((a[n].x - rec_width + 15) > 0 && (a[n].y - rec_width + 15) > 0)
			{
				Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
				//drawROI(after2, a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30);
				//cv::imshow("src", after2);
				//waitKey(0);
				int num = bSums(roi);
				if (num < dets)
				{
					k = a[n];
					break;
				}
			}
		}
	}
	else if (rightuppoint)
	{
		Point2f a[10];
		for (int i = 0; i < 10; i++)
		{
			a[i].x = 0; a[i].y = 0;
		}
		for (int n = 0; n < thedata.num; n++)
			a[n] = getPointAffinedPos(rightup[thedata.point[n]], center, thedata.theta - 90);
		int i, j;
		Point2f t;
		for (j = 0; j < 10; j++)
		{
			for (i = 0; i < 10 - 1 - j; i++)
			{
				if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
				{
					t = a[i];
					a[i] = a[i + 1];
					a[i + 1] = t;
				}
			}
		}
		if (thedata.num > 1)
		{
			if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
			{
				Point2f w;
				w = a[0];
				a[0] = a[1];
				a[1] = w;
			}
		}
		for (int n = 0; n < thedata.num; n++)
		{
			if ((a[n].x + rec_width - 15) <after.cols && (a[n].y - rec_width + 15) > 0)
			{
				Mat roi = after(Rect(a[n].x + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
				int num = bSums(roi);
				if (num < dets)
				{
					k = a[n];
					break;
				}
			}
		}
	}
	else if (rightdownpoint)
	{
		Point2f a[10];
		for (int i = 0; i < 10; i++)
		{
			a[i].x = 0; a[i].y = 0;
		}
		for (int n = 0; n < thedata.num; n++)
			a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
		int i, j;
		Point2f t;
		for (j = 0; j < 10; j++)
		{
			for (i = 0; i < 10 - 1 - j; i++)
			{
				if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
				{
					t = a[i];
					a[i] = a[i + 1];
					a[i + 1] = t;
				}
			}
		}
		if (thedata.num > 1)
		{
			if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
			{
				Point2f w;
				w = a[0];
				a[0] = a[1];
				a[1] = w;
			}
		}
		for (int n = 0; n < thedata.num; n++)
		{
	        if ((a[n].x + rec_width - 15) <after.cols && (a[n].y + rec_width - 15) <after.rows)
			{
				Mat roi = after(Rect(a[n].x + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
				int num = bSums(roi);
				if (num < dets)
				{
					k = a[n];
					break;
				}
			}
		}
	}
	else if (leftdownpoint)
	{
		Point2f a[10];
		for (int i = 0; i < 10; i++)
		{
			a[i].x = 0; a[i].y = 0;
		}
		for (int n = 0; n < thedata.num; n++)
			a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
		int i, j;
		Point2f t;
		for (j = 0; j < 10; j++)
		{
			for (i = 0; i < 10 - 1 - j; i++)
			{
				if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
				{
					t = a[i];
					a[i] = a[i + 1];
					a[i + 1] = t;
				}
			}
		}
		if (thedata.num > 1)
		{
			if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
			{
				Point2f w;
				w = a[0];
				a[0] = a[1];
				a[1] = w;
			}
		}
		for (int n = 0; n < thedata.num; n++)
		{
			if ((a[n].x - rec_width + 15) > 0 && (a[n].y + rec_width - 15) <after.rows)
			{
				Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
				int num = bSums(roi);
				if (num < dets)
				{
					k = a[n];
					break;
				}
			}
		}
	}
	else
		continue;
		//return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
	//float second=angleangin(after, k);
	/*if (second != 0)
	{
		after = ImageRotate(after, center, second);
		after2 = after.clone();
		k = getPointAffinedPos(k, center, second);
		angle = angle+second;
	}*/
	number= numbertest(k, after);
	//cv::imshow("旋转", after2);
	//waitKey(0);
	if (number.theta < 1 || number.theta>64)
		continue;
		//return nothing;
	drawCenter(g_srcImage1, number.center, 255, 5);
	drawCenter(after2, number.center, 255, 5);
	number.center = getPointBack(number.center, center, 90-angle);
	drawCenter(g_srcImage1, number.center, 0, 5);
	//cv::imshow("未旋转", g_srcImage1);
	//cv::imshow("旋转", after2);
	//waitKey(0);
	Point2f afterpoint=getRealXY(number.center, 4);
	int shang = int(number.theta-1) / 8;
	int yushu = int(number.theta-1) % 8;
	Point2f location;
	afterpoint = getPoint(afterpoint, center, angle);
	location.x = yushu * 50 + 25 +afterpoint.x;
	location.y = shang * 50 + 25 + afterpoint.y;
	last.center.x = location.x; last.center.y = location.y;
	last.theta = angle;
	//cv::imshow("旋转", after2);
	//waitKey(0);
	//char outputfile[70] = "D:/原D/车队/大三下/创意比赛/图像2/";
	char outputfile[70] = "C:/Users/帮主/Desktop/新建文件夹 (4)/";
	Im_write(outputfile, num, after2);
	num++;
	end = clock();
	cost = ((end - start)/CLOCKS_PER_SEC)*1000;
	printf("%f/n", cost);
     //return last;
	}

	cv::imshow("角点", g_srcImage1);
	cv::imshow("旋转", after2);
	cv::imshow("二值", g_pBinaryImage);
	waitKey(0);
	return nothing;
}


ROIData maindoCamerafront()
{
    ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
	ROIData cmp; cmp.center = last.center; cmp.theta = last.theta;
	if (Image.empty())
	{
		cout << "empty" << endl; return nothing;
	}
	Image.copyTo(g_srcImage);
		char outputfile1[70] = "../memory/";
		char outputfile2[70] = "../memory2/";
		Im_write(outputfile2, num, g_srcImage);
		num++;
		g_srcImage1 = prospective(g_srcImage);

		cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);
		threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图
		g_pBinaryImage1 = g_pBinaryImage.clone();
		CornerPoint(g_pBinaryImage);
		thetaData thedata = getTheta(g_pBinaryImage);

		Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
		Point2f k;
		after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);
		float angle = thedata.theta;
		after2 = after.clone();
		//imshow("6", after);
		//waitKey(0);
		if (leftuppoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(leftup[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x - rec_width + 15) > 0 && (a[n].y - rec_width + 15) > 0)
				{
					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
					//drawROI(after2, a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30);
					//cv::imshow("src", after2);
					//waitKey(0);
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else if (rightuppoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(rightup[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y - rec_width + 15) > 0)
				{
					Mat roi = after(Rect(a[n].x + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else if (rightdownpoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y + rec_width - 15) <after.rows)
				{
					Mat roi = after(Rect(a[n].x + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else if (leftdownpoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x - rec_width + 15) > 0 && (a[n].y + rec_width - 15) <after.rows)
				{
					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else
			return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
		//float second = angleangin(after, k);
		/*if (second != 0)
		{
			after = ImageRotate(after, center, second);
			after2 = after.clone();
			k = getPointAffinedPos(k, center, second);
			angle = angle + second;
		}*/
		number = numbertest(k, after);
		if (number.theta < 1 || number.theta>64)
			return nothing;
		drawCenter(after2, number.center, 255, 5);
		number.center = getPointBack(number.center, center, 90 - angle);
		Point2f afterpoint = getRealXY(number.center, 4);
		int shang = int(number.theta - 1) / 8;
		int yushu = int(number.theta - 1) % 8;
		Point2f location;
		afterpoint = getPoint(afterpoint, center, angle);
		location.x = yushu * 50 + 25 + afterpoint.x;
		location.y = shang * 50 + 25 + afterpoint.y;
		last.center.x = location.x; last.center.y = location.y;
		last.theta = angle;
		doright++;
		if (cmp.center.x != 0)
		{
			int x = abs(cmp.center.x - last.center.x); int y = abs(cmp.center.y - last.center.y);
				if (x > 35 || y > 35 && richtig == 0)
				{
					richtig++; comp = doright; 
					last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
					return nothing;
				}
				if ((doright - comp) == 1)
				{
					if (x > 35 || y > 35)
					{
						richtig++; comp = doright;
						last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
						if (richtig > 4)
						{
							richtig = 0; cmp.center.x = 0;
							last.center.x = cmp.center.x; 
							return nothing;
						}
						return nothing;
					}
					else
						richtig = 0;
				}
			}
		Im_write(outputfile1, num, after2);
		//namedWindow("旋转", 1); 
		//cv::imshow("旋转", after2);
		return last;
	}


ROIData maindoCamerabehind()
	{
		ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
		ROIData cmp; cmp.center = last.center; cmp.theta = last.theta;
		if (Image2.empty())
		{
			cout << "empty2" << endl; return nothing;
		}
		Image2.copyTo(g_srcImage);
		char outputfile1[70] = "../memory3/";
		char outputfile2[70] = "../memory4/";
		Im_write(outputfile2, num, g_srcImage);
		num++;
		g_srcImage1 = prospective2(g_srcImage);
		//imshow("6", g_srcImage1);
		//waitKey(0);
		Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
		cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);
		threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图
		g_pBinaryImage = ImageRotate(g_pBinaryImage, center, 180);
		g_grayImage = ImageRotate(g_grayImage, center, 180);
		/*imshow("6", g_pBinaryImage);
		waitKey(0);*/
		g_pBinaryImage1 = g_pBinaryImage.clone();
		CornerPoint(g_pBinaryImage);
		thetaData thedata = getTheta(g_pBinaryImage);

		Point2f k;
		after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);
		float angle = thedata.theta;
		after2 = after.clone();
		//imshow("6", after);
		//waitKey(0);
		if (leftuppoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(leftup[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x - rec_width + 15) > 0 && (a[n].y - rec_width + 15) > 0)
				{
					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
					//drawROI(after2, a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30);
					//cv::imshow("src", after2);
					//waitKey(0);
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else if (rightuppoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(rightup[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y - rec_width + 15) > 0)
				{
					Mat roi = after(Rect(a[n].x + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else if (rightdownpoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y + rec_width - 15) <after.rows)
				{
					Mat roi = after(Rect(a[n].x + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else if (leftdownpoint)
		{
			Point2f a[10];
			for (int i = 0; i < 10; i++)
			{
				a[i].x = 0; a[i].y = 0;
			}
			for (int n = 0; n < thedata.num; n++)
				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
			int i, j;
			Point2f t;
			for (j = 0; j < 10; j++)
			{
				for (i = 0; i < 10 - 1 - j; i++)
				{
					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
					{
						t = a[i];
						a[i] = a[i + 1];
						a[i + 1] = t;
					}
				}
			}
			if (thedata.num > 1)
			{
				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
				{
					Point2f w;
					w = a[0];
					a[0] = a[1];
					a[1] = w;
				}
			}
			for (int n = 0; n < thedata.num; n++)
			{
				if ((a[n].x - rec_width + 15) > 0 && (a[n].y + rec_width - 15) <after.rows)
				{
					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						break;
					}
				}
			}
		}
		else
			return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
						   //float second = angleangin(after, k);
						   /*if (second != 0)
						   {
						   after = ImageRotate(after, center, second);
						   after2 = after.clone();
						   k = getPointAffinedPos(k, center, second);
						   angle = angle + second;
						   }*/
		number = numbertest(k, after);
		if (number.theta < 1 || number.theta>64)
			return nothing;
		drawCenter(g_srcImage1, number.center, 255, 5);
		drawCenter(after2, number.center, 255, 5);
		number.center = getPointBack(number.center, center, 90 - angle);
		number.center = getPointBack(number.center, center, -180);
		drawCenter(g_srcImage1, number.center, 0, 5);
		/*cv::imshow("未旋转", g_srcImage1);
		cv::imshow("旋转", after2);
		waitKey(0);*/
		Point2f afterpoint = getRealXY2(number.center, 4);
		int shang = int(number.theta - 1) / 8;
		int yushu = int(number.theta - 1) % 8;
		Point2f location;
		afterpoint = getPoint(afterpoint, center, angle);
		location.x = yushu * 50 + 25 + afterpoint.x;
		location.y = shang * 50 + 25 + afterpoint.y;
		last.center.x = location.x; last.center.y = location.y;
		last.theta = angle;
		doright++;
		if (cmp.center.x != 0)
		{
			int x = abs(cmp.center.x - last.center.x); int y = abs(cmp.center.y - last.center.y);
			if (x > 35 || y > 35 && richtig == 0)
			{
				richtig++; comp = doright;
				last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
				return nothing;
			}
			if ((doright - comp) == 1)
			{
				if (x > 35 || y > 35)
				{
					richtig++; comp = doright;
					last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
					if (richtig > 4)
					{
						richtig = 0; cmp.center.x = 0; return nothing;
					}
					return nothing;
				}
				else
					richtig = 0;
			}
		}
		Im_write(outputfile1, num, after2);
		return last;
	}

ROIData maindoCamera()
{
	ROIData send = maindoCamerafront();
	if (send.center.x == 0)
	{
		send =maindoCamerabehind();
		return send;
	}
	else 
		return send;
}



//int main()
//{
//	//VideoCapture cap;
//	//cap.open(1);   //0是电脑摄像头
//	maindoCamerabehind();
//	//for (int l = 0; l < 20; l++) {
//	//cap >> Image;
//	//ROIData tt= maindoCamera();
//	//	}
//}