using namespace std;
#include <opencv2/opencv.hpp>  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include"basichandle.h"
#include"prehandle.h"
using namespace cv;
#include <time.h>
//using namespace cv::gpu;
//#include "opencv2/gpu/gpu.hpp"
//resize把原图变小再操作？ 利用特征或已知把框取得更小 
//加入阈值调节 注意模板阈值与数字阈值一致 阈值调节后则是否障碍的阈值也变了
// 最后加入避障如果比赛现场测试发现双面胶贴障碍没用则使用 
//函数的封装          上传前注意读存位置及把main注 后摄像头程序里读图那句话注 flag=1注 
//second 
//现在后摄像头永远小2？
//检查画图或取点 为什么出现那个超出错误

ROIData number;
extern int thre;
int richtig =0;int doright = 0; int comp = 0;
int dets = 4800;
int flag = 0;

Mat g_srcImage, g_srcImage1, g_grayImage, g_pBinaryImage,g_pBinaryImage1,after,after2,Image, Image2, Image3, Image4;
extern Point2f leftup[20];//左上     红色
extern Point2f rightup[20];//右上    绿色
extern Point2f rightdown[20];//右下  蓝色
extern Point2f leftdown[20] ;//左下   黑色
extern int leftuppoint , rightuppoint , rightdownpoint , leftdownpoint ;
extern int rec_width;
int num =1;ROIData last;
//int flag = 1;//这个flag要加到外面 每个新的回合要是1 

ROIData maindoPicture(Point2f yuanxin,int orientation)
{
	ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
	number.theta = 0; last.center2.x = 0; last.center2.y = 0;
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
	Point2f center; center.x = g_pBinaryImage.cols / 2; center.y = g_pBinaryImage.rows / 2;
	if (orientation == 1)
	{
		g_pBinaryImage = ImageRotate(g_pBinaryImage, center, 90);
		g_grayImage = ImageRotate(g_grayImage, center, 90);
	}
	if (orientation == 2)
	{
		g_pBinaryImage = ImageRotate(g_pBinaryImage, center, -90);
		g_grayImage = ImageRotate(g_grayImage, center, -90);
	}
	g_pBinaryImage1 = g_pBinaryImage.clone();
	CornerPoint(g_pBinaryImage);

	thetaData thedata=getTheta(g_pBinaryImage);

	
	Point2f k;
	after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);//theta.x是旋转角度//thedata.theta-90
	float angle= thedata.theta;
	after2 = after.clone();
	/*cv::imshow("旋转", g_srcImage1);
	waitKey(0);*/
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
						   number = numbertest(k, after);
						   if (number.theta >0 &&number.theta<65)
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
					number = numbertest(k, after);
					if (number.theta >0 && number.theta<65)
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
					number = numbertest(k, after);
					if (number.theta >0 && number.theta<65)
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
					number = numbertest(k, after);
					if (number.theta >0 && number.theta<65)
						break;
				}
			}
		}
	}
	else
		continue;
		//return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
	/*float second=angleangin(after, k);
	if (second != 0)
	{
		after = ImageRotate(after, center, second);
		after2 = after.clone();
		k = getPointAffinedPos(k, center, second);
		angle = angle+second;
	}*/
	//number= numbertest(k, after);
	/*cv::imshow("旋转", after2);
	waitKey(0);*/
	if (number.theta < 1 || number.theta>64)
		continue;
		//return nothing;

	float zuo = k.x / 4; float you = (640 - k.x) / 4; float shangmian = k.y / 4; float xia = (640 - k.y) / 4;
	Point2f kk; kk.x = k.x; kk.y = k.y;
	
	drawCenter(g_srcImage1, number.center, 255, 5);
	drawCenter(after2, number.center, 255, 5);
	number.center = getPointBack(number.center, center, 90-angle);
	drawCenter(g_srcImage1, number.center, 0, 5);
	k= getPointBack(k, center, 90 - angle);
	if (orientation == 1)
	{
		number.center = getPointBack(number.center, center, -90);
		k = getPointBack(k, center, -90);
	}
	if (orientation == 2)
	{
		number.center = getPointBack(number.center, center, 90);
		k = getPointBack(k, center, 90);
	}
	/*cv::imshow("未旋转", after);
	cv::imshow("旋转", after2);
	cv::imshow("旋转", g_srcImage1);
	waitKey(0);*/
	Point2f afterpoint=getRealXY(number.center, 4);
	k= getRealXY(k, 4);
	int shang = int(number.theta-1) / 8;
	int yushu = int(number.theta-1) % 8;
	Point2f location;
	afterpoint = getPoint(afterpoint, center, angle, orientation);
	k = getPoint(k, center, angle, orientation);;
	location.x = yushu * 50 + 25 +afterpoint.x;
	location.y = shang * 50 + 25 + afterpoint.y;

	if (leftuppoint) {
		k.x = yushu * 50 + 50 - 2.5 + k.x;
		k.y = shang * 50 + 50 - 2.5 + k.y;
		zuo = yushu * 50 + 50 - 2.5 - zuo + 10; you = yushu * 50 + 50 - 2.5 + you - 10;
		shangmian = shang * 50 + 50 - 2.5 - shangmian + 10; xia = shang * 50 + 50 - 2.5 + xia - 20;
	}if (rightuppoint) {
		k.x = yushu * 50 + k.x + 2.5;
		k.y = shang * 50 + 50 - 2.5 + k.y;
		zuo = yushu * 50 +2.5 - zuo + 10; you = yushu * 50 +2.5 + you - 10;
		shangmian = shang * 50 + 50 - 2.5 - shangmian + 10; xia = shang * 50 + 50 - 2.5 + xia - 20;
	}
	if (rightdownpoint) {
		k.x = yushu * 50 + k.x + 2.5;
		k.y = shang * 50 + 2.5 + k.y;
		zuo = yushu * 50 + 2.5 - zuo + 10; you = yushu * 50 + 2.5 + you - 10;
		shangmian = shang * 50 +2.5 - shangmian + 10; xia = shang * 50 + 2.5 + xia - 20;
	}if (leftdownpoint) {
		k.x = yushu * 50 + k.x + 50 - 2.5;
		k.y = shang * 50 + 2.5 + k.y;
		zuo = yushu * 50 + 50 - 2.5 - zuo + 10; you = yushu * 50 + 50 - 2.5 + you - 10;
		shangmian = shang * 50 + 2.5 - shangmian + 10; xia = shang * 50 + 2.5 + xia - 20;
	}
	Point2f circlecenter;circlecenter.x = 0; circlecenter.y = 0;
	if (flag == 1) {
		if (yuanxin.x > zuo&&yuanxin.x<you&&yuanxin.y>shangmian&&yuanxin.y < xia)
		{
			GaussianBlur(after, after, Size(9, 9), 2, 2);
			vector<Vec3f> circles;
			HoughCircles(after, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 35, 45);  //运用霍夫变换识别圆
			for (size_t i = 0; i < circles.size(); i++)
			{
				Point center(cvRound(circles[i][0]), cvRound(circles[i][1])); //获取圆心
				int radius = cvRound(circles[i][2]);
				circle(g_srcImage1, center, radius, Scalar(0, 0, 0), 3, 8, 0); //绘制圆的边缘
			}
			if (circles.size() > 0)
			{
				circlecenter.x = circles[0][0]; circlecenter.y = circles[0][1];
				circlecenter.x = (circlecenter.x - kk.x) / 4;
				circlecenter.y = (circlecenter.y - kk.y) / 4;
				if (leftuppoint) {
					circlecenter.x = yushu * 50 + 50 - 2.5 + circlecenter.x;
					circlecenter.y = circlecenter.y + shang * 50 + 50 - 2.5;
				}if (rightuppoint) {
					circlecenter.x = yushu * 50 + 2.5 + circlecenter.x;
					circlecenter.y = circlecenter.y + shang * 50 + 50 - 2.5;
				}
				if (rightdownpoint) {
					circlecenter.x = yushu * 50 + 2.5 + circlecenter.x;
					circlecenter.y = circlecenter.y + shang * 50 + 2.5;
				}if (leftdownpoint) {
					circlecenter.x = yushu * 50 + 50 - 2.5 + circlecenter.x;
					circlecenter.y = circlecenter.y + shang * 50 + 2.5;
				}
				if (fabs(circlecenter.x - yuanxin.x) < 20 && fabs(circlecenter.y - yuanxin.y) < 20)
				{
					last.center2.x = circlecenter.x; last.center2.y = circlecenter.y;
					flag = 0;
				}
			}
			
		}
	}

	last.center.x = k.x; last.center.y = k.y;
	last.theta = angle;
	cv::imshow("旋转", after2);
	waitKey(0);
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


ROIData maindoCamerafront(Point2f yuanxin,int orientation)
{
    ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
	last.center2.x = 0; last.center2.y = 0;
	ROIData cmp; cmp.center = last.center; cmp.theta = last.theta;
	number.theta = 0;
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
		Point2f center; center.x = g_pBinaryImage.cols / 2; center.y = g_pBinaryImage.rows / 2;
		if (orientation == 1)
		{
			g_pBinaryImage = ImageRotate(g_pBinaryImage, center, 90);
			g_grayImage = ImageRotate(g_grayImage, center, 90);
		}
		if (orientation == 2)
		{
			g_pBinaryImage = ImageRotate(g_pBinaryImage, center, -90);
			g_grayImage = ImageRotate(g_grayImage, center, -90);
		}
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
		//number = numbertest(k, after);
		if (number.theta < 1 || number.theta>64)
			return nothing;

		float zuo = k.x / 4; float you = (640 - k.x) / 4; float shangmian = k.y / 4; float xia = (640 - k.y) / 4;
		Point2f kk; kk.x = k.x; kk.y = k.y;

		drawCenter(after2, number.center, 255, 5);
		number.center = getPointBack(number.center, center, 90 - angle);
		k= getPointBack(k, center, 90 - angle);
		if (orientation == 1)
		{
			number.center = getPointBack(number.center, center, -90);
			k = getPointBack(k, center, -90);
		}
		if (orientation == 2)
		{
			number.center = getPointBack(number.center, center, 90);
			k = getPointBack(k, center, 90);
		}
		Point2f afterpoint = getRealXY(number.center, 4);
		k= getRealXY(k, 4);
		int shang = int(number.theta - 1) / 8;
		int yushu = int(number.theta - 1) % 8;
		Point2f location;
		afterpoint = getPoint(afterpoint, center, angle, orientation);
		k= getPoint(k, center, angle, orientation);
		location.x = yushu * 50 + 25 + afterpoint.x;
		location.y = shang * 50 + 25 + afterpoint.y;

		if (leftuppoint) {
			k.x = yushu * 50 + 50 - 2.5 + k.x;
			k.y = shang * 50 + 50 - 2.5 + k.y;
			zuo = yushu * 50 + 50 - 2.5 - zuo + 10; you = yushu * 50 + 50 - 2.5 + you - 10;
			shangmian = shang * 50 + 50 - 2.5 - shangmian + 10; xia = shang * 50 + 50 - 2.5 + xia - 20;
		}if (rightuppoint) {
			k.x = yushu * 50 + k.x + 2.5;
			k.y = shang * 50 + 50 - 2.5 + k.y;
			zuo = yushu * 50 + 2.5 - zuo + 10; you = yushu * 50 + 2.5 + you - 10;
			shangmian = shang * 50 + 50 - 2.5 - shangmian + 10; xia = shang * 50 + 50 - 2.5 + xia - 20;
		}
		if (rightdownpoint) {
			k.x = yushu * 50 + k.x + 2.5;
			k.y = shang * 50 + 2.5 + k.y;
			zuo = yushu * 50 + 2.5 - zuo + 10; you = yushu * 50 + 2.5 + you - 10;
			shangmian = shang * 50 + 2.5 - shangmian + 10; xia = shang * 50 + 2.5 + xia - 20;
		}if (leftdownpoint) {
			k.x = yushu * 50 + k.x + 50 - 2.5;
			k.y = shang * 50 + 2.5 + k.y;
			zuo = yushu * 50 + 50 - 2.5 - zuo + 10; you = yushu * 50 + 50 - 2.5 + you - 10;
			shangmian = shang * 50 + 2.5 - shangmian + 10; xia = shang * 50 + 2.5 + xia - 20;
		}
		Point2f circlecenter; circlecenter.x = 0; circlecenter.y = 0;
		if (flag == 1) {
			if (yuanxin.x > zuo&&yuanxin.x<you&&yuanxin.y>shangmian&&yuanxin.y < xia)
			{
				GaussianBlur(after, after, Size(9, 9), 2, 2);
				vector<Vec3f> circles;
				HoughCircles(after, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 35, 45);  //运用霍夫变换识别圆
				//for (size_t i = 0; i < circles.size(); i++)
				//{
				//	Point center(cvRound(circles[i][0]), cvRound(circles[i][1])); //获取圆心
				//	int radius = cvRound(circles[i][2]);
				//	circle(g_srcImage1, center, radius, Scalar(0, 0, 0), 3, 8, 0); //绘制圆的边缘
				//}
				if (circles.size() > 0)
				{
					circlecenter.x = circles[0][0]; circlecenter.y = circles[0][1];
					circlecenter.x = (circlecenter.x - kk.x) / 4;
					circlecenter.y = (circlecenter.y - kk.y) / 4;
					if (leftuppoint) {
						circlecenter.x = yushu * 50 + 50 - 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 50 - 2.5;
					}if (rightuppoint) {
						circlecenter.x = yushu * 50 + 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 50 - 2.5;
					}
					if (rightdownpoint) {
						circlecenter.x = yushu * 50 + 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 2.5;
					}if (leftdownpoint) {
						circlecenter.x = yushu * 50 + 50 - 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 2.5;
					}
					if (fabs(circlecenter.x - yuanxin.x) < 20 && fabs(circlecenter.y - yuanxin.y) < 20)
					{
						last.center2.x = circlecenter.x; last.center2.y = circlecenter.y;
						flag = 0;
					}
				}

			}
		}

		last.center.x = k.x; last.center.y =k.y;
		last.theta = angle;
		doright++;
		if (cmp.center.x != 0)
		{
			int x = abs(cmp.center.x - last.center.x); int 
				y = abs(cmp.center.y - last.center.y);
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
						if (richtig >4)
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


ROIData maindoCamerabehind(Point2f yuanxin, int orientation)
	{
		ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
		ROIData cmp; cmp.center = last.center; cmp.theta = last.theta;
		last.center2.x = 0; last.center2.y = 0;
		number.theta = 0;
		//Image2 = imread("D:/原D/车队/大三下/创意比赛/图像/picture8.jpg");
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
		/*imshow("6", g_srcImage1);
		waitKey(0);*/
		
		cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);
		threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);
		Point2f center; center.x = g_pBinaryImage.cols / 2; center.y = g_pBinaryImage.rows /2;
		if (orientation == 0)
		{
			g_pBinaryImage = ImageRotate(g_pBinaryImage, center, 180);
			g_grayImage = ImageRotate(g_grayImage, center, 180);
		}
		if (orientation == 1)
		{
			g_pBinaryImage = ImageRotate(g_pBinaryImage, center, -90);
			g_grayImage = ImageRotate(g_grayImage, center, -90);
		}
		if (orientation == 2)
		{
			g_pBinaryImage = ImageRotate(g_pBinaryImage, center, 90);
			g_grayImage = ImageRotate(g_grayImage, center, 90);
		}
		/*imshow("6", g_grayImage);
		imshow("7", g_srcImage1);
		waitKey(0);*/
		g_pBinaryImage1 = g_pBinaryImage.clone();
		CornerPoint(g_pBinaryImage);
		thetaData thedata = getTheta(g_pBinaryImage);

		Point2f k;
		after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);
		float angle = thedata.theta;
		after2 = after.clone();
		/*imshow("6", g_pBinaryImage1);
		imshow("5", after2);
		waitKey(0);*/
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
			for (j = 0; j < thedata.num; j++)
			{
				for (i = 0; i < thedata.num - 1 - j; i++)
				{
					if (a[i].y > a[i + 1].y) /* 由小到大>,由大到小< */
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
					drawROI(after2, a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30);
					/*cv::imshow("src", after2);
					waitKey(2000);*/
					int num = bSums(roi);
					if (num < dets)
					{
						k = a[n];
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
			for (j = 0; j < thedata.num; j++)
			{
				for (i = 0; i < thedata.num - 1 - j; i++)
				{
					if (a[i].y > a[i + 1].y) /* 由小到大>,由大到小时改为< */
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
			for (j = 0; j < thedata.num; j++)
			{
				for (i = 0; i < thedata.num - 1 - j; i++)
				{
					if (a[i].y > a[i + 1].y) /* 由小到大>,由大到小时改为< */
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
			for (j = 0; j <thedata.num; j++)
			{
				for (i = 0; i < thedata.num - 1 - j; i++)
				{
					if (a[i].y > a[i + 1].y) /* 由小到大>,由大到小时改为< */
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
						number = numbertest(k, after);
						if (number.theta >0 && number.theta<65)
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
		//number = numbertest(k, after);
		if (number.theta < 1 || number.theta>64)
			return nothing;

		float zuo = k.x / 4; float you = (640 - k.x) / 4; float shangmian = k.y / 4; float xia = (640 - k.y) / 4;
		Point2f kk; kk.x = k.x; kk.y = k.y;

		drawCenter(g_srcImage1, k, 255, 5);
		drawCenter(after2,k, 255, 5);
		number.center = getPointBack(number.center, center, 90 - angle);
		k = getPointBack(k, center, 90 - angle);
		if (orientation == 0)
		{
		  number.center = getPointBack(number.center, center, -180);
		  k = getPointBack(k, center, -180);
		}
		if (orientation == 1)
		{
			number.center = getPointBack(number.center, center, 90);
			k = getPointBack(k, center,90);
		}
		if (orientation == 2)
		{
			number.center = getPointBack(number.center, center, -90);
			k = getPointBack(k, center, -90);
		}
		drawCenter(g_srcImage1, k, 0, 5);
		/*cv::imshow("未旋转", g_srcImage1);
		cv::imshow("旋转", after2);
		waitKey(0);*/
		Point2f afterpoint = getRealXY2(number.center, 4);
		k = getRealXY2(k, 4);
		int shang = int(number.theta - 1) / 8;
		int yushu = int(number.theta - 1) % 8;
		Point2f location;
		afterpoint = getPoint2(afterpoint, center, angle, orientation);
		k = getPoint2(k, center, angle, orientation);

		if (leftuppoint) {
			k.x = yushu * 50 + 50 - 2.5 + k.x;
			k.y = shang * 50 + 50 - 2.5 + k.y;
			zuo = yushu * 50 + 50 - 2.5 - zuo + 10; you = yushu * 50 + 50 - 2.5 + you - 10;
			shangmian = shang * 50 + 50 - 2.5 - shangmian + 10; xia = shang * 50 + 50 - 2.5 + xia - 20;
		}if (rightuppoint) {
			k.x = yushu * 50 + k.x + 2.5;
			k.y = shang * 50 + 50 - 2.5 + k.y;
			zuo = yushu * 50 + 2.5 - zuo + 10; you = yushu * 50 + 2.5 + you - 10;
			shangmian = shang * 50 + 50 - 2.5 - shangmian + 10; xia = shang * 50 + 50 - 2.5 + xia - 20;
		}
		if (rightdownpoint) {
			k.x = yushu * 50 + k.x + 2.5;
			k.y = shang * 50 + 2.5 + k.y;
			zuo = yushu * 50 + 2.5 - zuo + 10; you = yushu * 50 + 2.5 + you - 10;
			shangmian = shang * 50 + 2.5 - shangmian + 10; xia = shang * 50 + 2.5 + xia - 20;
		}if (leftdownpoint) {
			k.x = yushu * 50 + k.x + 50 - 2.5;
			k.y = shang * 50 + 2.5 + k.y;
			zuo = yushu * 50 + 50 - 2.5 - zuo + 10; you = yushu * 50 + 50 - 2.5 + you - 10;
			shangmian = shang * 50 + 2.5 - shangmian + 10; xia = shang * 50 + 2.5 + xia - 20;
		}

		location.x = yushu * 50 + 25 + afterpoint.x;
		location.y = shang * 50 + 25 + afterpoint.y;
		Point2f circlecenter; circlecenter.x = 0; circlecenter.y = 0;
		if (flag == 1) {
			if (yuanxin.x > zuo&&yuanxin.x<you&&yuanxin.y>shangmian&&yuanxin.y < xia)
			{
				GaussianBlur(after, after, Size(9, 9), 2, 2);
				vector<Vec3f> circles;
				HoughCircles(after, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 35, 45);  //运用霍夫变换识别圆
				//for (size_t i = 0; i < circles.size(); i++)
				//{
				//	Point center(cvRound(circles[i][0]), cvRound(circles[i][1])); //获取圆心
				//	int radius = cvRound(circles[i][2]);
				//	circle(g_srcImage1, center, radius, Scalar(0, 0, 0), 3, 8, 0); //绘制圆的边缘
				//}
				if (circles.size() > 0)
				{
					circlecenter.x = circles[0][0]; circlecenter.y = circles[0][1];
					circlecenter.x = (circlecenter.x - kk.x) / 4;
					circlecenter.y = (circlecenter.y - kk.y) / 4;
					if (leftuppoint) {
						circlecenter.x = yushu * 50 + 50 - 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 50 - 2.5;
					}if (rightuppoint) {
						circlecenter.x = yushu * 50 + 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 50 - 2.5;
					}
					if (rightdownpoint) {
						circlecenter.x = yushu * 50 + 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 2.5;
					}if (leftdownpoint) {
						circlecenter.x = yushu * 50 + 50 - 2.5 + circlecenter.x;
						circlecenter.y = circlecenter.y + shang * 50 + 2.5;
					}
					if (fabs(circlecenter.x - yuanxin.x) < 20 && fabs(circlecenter.y - yuanxin.y) < 20)
					{
						last.center2.x = circlecenter.x; last.center2.y = circlecenter.y;
						flag = 0;
					}
				}

			}
		}

		last.center.x = k.x; last.center.y = k.y;
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

//ROIData maindoCameraleft()
//	{
//		ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
//		ROIData cmp; cmp.center = last.center; cmp.theta = last.theta;
//		number.theta = 0;
//		//Image3 = imread("D:/原D/车队/大三下/创意比赛/图像/18.jpg");
//		if (Image3.empty())
//		{
//			cout << "empty3" << endl; return nothing;
//		}
//		Image3.copyTo(g_srcImage);
//		char outputfile1[70] = "../memory5/";
//		char outputfile2[70] = "../memory6/";
//		Im_write(outputfile2, num, g_srcImage);
//		num++;
//		g_srcImage1 = prospective3(g_srcImage);
//		//imshow("6", g_srcImage1);
//		//waitKey(0);
//		Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
//		cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);
//		threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图 
//		g_pBinaryImage = ImageRotate(g_pBinaryImage, center, 90);
//		g_grayImage = ImageRotate(g_grayImage, center, 90);
//		/*imshow("6", g_pBinaryImage);
//		waitKey(0);*/
//		g_pBinaryImage1 = g_pBinaryImage.clone();
//		CornerPoint(g_pBinaryImage);
//		thetaData thedata = getTheta(g_pBinaryImage);
//
//		Point2f k;
//		after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);
//		float angle = thedata.theta;
//		after2 = after.clone();
//		/*imshow("6", g_pBinaryImage1);
//		imshow("5", after2);
//		waitKey(0);*/
//		if (leftuppoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(leftup[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x - rec_width + 15) > 0 && (a[n].y - rec_width + 15) > 0)
//				{
//					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
//					//drawROI(after2, a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30);
//					//cv::imshow("src", after2);
//					//waitKey(0);
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else if (rightuppoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(rightup[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y - rec_width + 15) > 0)
//				{
//					Mat roi = after(Rect(a[n].x + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else if (rightdownpoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y + rec_width - 15) <after.rows)
//				{
//					Mat roi = after(Rect(a[n].x + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else if (leftdownpoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x - rec_width + 15) > 0 && (a[n].y + rec_width - 15) <after.rows)
//				{
//					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else
//			return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
//						   //float second = angleangin(after, k);
//						   /*if (second != 0)
//						   {
//						   after = ImageRotate(after, center, second);
//						   after2 = after.clone();
//						   k = getPointAffinedPos(k, center, second);
//						   angle = angle + second;
//						   }*/
//						   //number = numbertest(k, after);
//		if (number.theta < 1 || number.theta>64)
//			return nothing;
//		drawCenter(g_srcImage1, number.center, 255, 5);
//		drawCenter(after2, number.center, 255, 5);
//		number.center = getPointBack(number.center, center, 90 - angle);
//		number.center = getPointBack(number.center, center, -90);
//		drawCenter(g_srcImage1, number.center, 0, 5);
//		cv::imshow("未旋转", g_srcImage1);
//		cv::imshow("旋转", after2);
//		waitKey(0);
//		Point2f afterpoint = getRealXY4(number.center, 4);
//		int shang = int(number.theta - 1) / 8;
//		int yushu = int(number.theta - 1) % 8;
//		Point2f location;
//		afterpoint = getPoint(afterpoint, center, angle);
//		location.x = yushu * 50 + 25 - afterpoint.x;
//		location.y = shang * 50 + 25 - afterpoint.y;
//		last.center.x = location.x; last.center.y = location.y;
//		last.theta = angle;
//		cv::imshow("未旋转", g_srcImage1);
//		cv::imshow("旋转", after2);
//		waitKey(0);
//		doright++;
//		if (cmp.center.x != 0)
//		{
//			int x = abs(cmp.center.x - last.center.x); int y = abs(cmp.center.y - last.center.y);
//			if (x > 35 || y > 35 && richtig == 0)
//			{
//				richtig++; comp = doright;
//				last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
//				return nothing;
//			}
//			if ((doright - comp) == 1)
//			{
//				if (x > 35 || y > 35)
//				{
//					richtig++; comp = doright;
//					last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
//					if (richtig > 4)
//					{
//						richtig = 0; cmp.center.x = 0; return nothing;
//					}
//					return nothing;
//				}
//				else
//					richtig = 0;
//			}
//		}
//		Im_write(outputfile1, num, after2);
//		return last;
//	}
//
//ROIData maindoCameraright()
//	{
//		ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
//		ROIData cmp; cmp.center = last.center; cmp.theta = last.theta;
//		number.theta = 0;
//		//Image4 = imread("D:/原D/车队/大三下/创意比赛/图像/19.jpg");
//		if (Image4.empty())
//		{
//			cout << "empty4" << endl; return nothing;
//		}
//		Image4.copyTo(g_srcImage);
//		char outputfile1[70] = "../memory7/";
//		char outputfile2[70] = "../memory8/";
//		Im_write(outputfile2, num, g_srcImage);
//		num++;
//		g_srcImage1 = prospective4(g_srcImage);
//		//imshow("6", g_srcImage1);
//		//waitKey(0);
//		Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
//		cvtColor(g_srcImage1, g_grayImage, CV_BGR2GRAY);
//		threshold(g_grayImage, g_pBinaryImage, thre, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图 
//		g_pBinaryImage = ImageRotate(g_pBinaryImage, center, -90);
//		g_grayImage = ImageRotate(g_grayImage, center, -90);
//		/*imshow("6", g_pBinaryImage);
//		waitKey(0);*/
//		g_pBinaryImage1 = g_pBinaryImage.clone();
//		CornerPoint(g_pBinaryImage);
//		thetaData thedata = getTheta(g_pBinaryImage);
//
//		Point2f k;
//		after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);
//		float angle = thedata.theta;
//		after2 = after.clone();
//		/*imshow("6", g_pBinaryImage1);
//		imshow("5", after2);
//		waitKey(0);*/
//		if (leftuppoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(leftup[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x - rec_width + 15) > 0 && (a[n].y - rec_width + 15) > 0)
//				{
//					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
//					//drawROI(after2, a[n].x - rec_width + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30);
//					//cv::imshow("src", after2);
//					//waitKey(0);
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else if (rightuppoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(rightup[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y - rec_width + 15) > 0)
//				{
//					Mat roi = after(Rect(a[n].x + 15, a[n].y - rec_width + 15, rec_width - 30, rec_width - 30));
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else if (rightdownpoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x > a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x + rec_width - 15) <after.cols && (a[n].y + rec_width - 15) <after.rows)
//				{
//					Mat roi = after(Rect(a[n].x + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else if (leftdownpoint)
//		{
//			Point2f a[10];
//			for (int i = 0; i < 10; i++)
//			{
//				a[i].x = 0; a[i].y = 0;
//			}
//			for (int n = 0; n < thedata.num; n++)
//				a[n] = getPointAffinedPos(rightdown[thedata.point[n]], center, thedata.theta - 90);
//			int i, j;
//			Point2f t;
//			for (j = 0; j < 10; j++)
//			{
//				for (i = 0; i < 10 - 1 - j; i++)
//				{
//					if (a[i].y < a[i + 1].y) /* 由小到大>,由大到小时改为< */
//					{
//						t = a[i];
//						a[i] = a[i + 1];
//						a[i + 1] = t;
//					}
//				}
//			}
//			if (thedata.num > 1)
//			{
//				if ((a[0].x < a[1].x) && fabs(a[0].y - a[1].y)<50)
//				{
//					Point2f w;
//					w = a[0];
//					a[0] = a[1];
//					a[1] = w;
//				}
//			}
//			for (int n = 0; n < thedata.num; n++)
//			{
//				if ((a[n].x - rec_width + 15) > 0 && (a[n].y + rec_width - 15) <after.rows)
//				{
//					Mat roi = after(Rect(a[n].x - rec_width + 15, a[n].y + 15, rec_width - 30, rec_width - 30));
//					int num = bSums(roi);
//					if (num < dets)
//					{
//						k = a[n];
//						number = numbertest(k, after);
//						if (number.theta >0 && number.theta<65)
//							break;
//					}
//				}
//			}
//		}
//		else
//			return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
//						   //float second = angleangin(after, k);
//						   /*if (second != 0)
//						   {
//						   after = ImageRotate(after, center, second);
//						   after2 = after.clone();
//						   k = getPointAffinedPos(k, center, second);
//						   angle = angle + second;
//						   }*/
//						   //number = numbertest(k, after);
//		if (number.theta < 1 || number.theta>64)
//			return nothing;
//		drawCenter(g_srcImage1, number.center, 255, 5);
//		drawCenter(after2, number.center, 255, 5);
//		number.center = getPointBack(number.center, center, 90 - angle);
//		number.center = getPointBack(number.center, center, 90);
//		drawCenter(g_srcImage1, number.center, 0, 5);
//		/*cv::imshow("未旋转", g_srcImage1);
//		cv::imshow("旋转", after2);
//		waitKey(0);*/
//		Point2f afterpoint = getRealXY4(number.center, 4);
//		int shang = int(number.theta - 1) / 8;
//		int yushu = int(number.theta - 1) % 8;
//		Point2f location;
//		afterpoint = getPoint(afterpoint, center, angle);
//		location.x = yushu * 50 + 25 + afterpoint.x;
//		location.y = shang * 50 + 25 + afterpoint.y;
//		last.center.x = location.x; last.center.y = location.y;
//		last.theta = angle;
//		doright++;
//		if (cmp.center.x != 0)
//		{
//			int x = abs(cmp.center.x - last.center.x); int y = abs(cmp.center.y - last.center.y);
//			if (x > 35 || y > 35 && richtig == 0)
//			{
//				richtig++; comp = doright;
//				last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
//				return nothing;
//			}
//			if ((doright - comp) == 1)
//			{
//				if (x > 35 || y > 35)
//				{
//					richtig++; comp = doright;
//					last.center.x = cmp.center.x; last.center.y = cmp.center.y; last.theta = cmp.theta;
//					if (richtig > 4)
//					{
//						richtig = 0; cmp.center.x = 0; return nothing;
//					}
//					return nothing;
//				}
//				else
//					richtig = 0;
//			}
//		}
//		Im_write(outputfile1, num, after2);
//		return last;
//	}

ROIData maindoCamera(Point2f yuanxin, int orientation)
{
	ROIData send = maindoCamerafront(yuanxin, orientation);
	if (send.center.x == 0&& send.center.y == 0)
	{
		send =maindoCamerabehind(yuanxin, orientation);
		/*if (send.center.x == 0 && send.center.y == 0)
		{
			send = maindoCameraleft();
			if (send.center.x == 0 && send.center.y == 0)
			{
				send = maindoCameraright();
				return send;
			}
			else 
				return send;
		}
		else */
			return send;
	}
	else 
		return send;
}


//int main()
//{
//	//VideoCapture cap;
//	//cap.open(1);   //0是电脑摄像头
//	//int num_devices = cv::cuda::getCudaEnabledDeviceCount();
//	Point2f q(325,325);
//	int orientation = 2;//0前 1左 2右
//	maindoPicture(q, orientation);
//	//for (int l = 0; l < 20; l++) {
//	//cap >> Image;
//	//ROIData tt= maindoCamera();
//	//	}
//}
