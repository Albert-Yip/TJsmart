#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "basichandle.h"
#include "prehandle.h"
using namespace cv;
using namespace std;
//resize把原图变小再操作？ 利用特征或已知把框取得更小
//加入阈值调节 注意模板阈值与数字阈值一致 阈值调节后则是否障碍的阈值也变了 模型制作函数完善
// 最后加入避障如果比赛现场测试发现双面胶贴障碍没用则使用
//rec_width调节
//是否是两位数判断的改进
//对棋子 挡板的判断
//函数的封装

ROIData number;

Mat g_srcImage, g_srcImage1, g_grayImage, g_pBinaryImage,g_pBinaryImage1,after,after2,Image;
extern Point2f leftup[20];//左上     红色
extern Point2f rightup[20];//右上    绿色
extern Point2f rightdown[20];//右下  蓝色
extern Point2f leftdown[20] ;//左下   黑色
extern int leftuppoint , rightuppoint , rightdownpoint , leftdownpoint ;
extern int rec_width;

ROIData maindoPicture()
{
	ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
	ROIData last;

	//连续图片读取处理
	/*int num = 0;
	do {
		char filename[70] = "D:/原D/车队/大三下/创意比赛/图像/";
		g_srcImage = Im_read(filename, num);
		if (g_srcImage.data == 0)
			break;*/

	g_srcImage = imread("../source/cam/a.jpg");

	if (!g_srcImage.data)
	{
		printf("error input!\n");
		return nothing;
	}
	g_srcImage1 = prospective(g_srcImage);

	CornerPoint(g_srcImage1);

	threshold(g_grayImage, g_pBinaryImage, 160, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图
	g_pBinaryImage1 = g_pBinaryImage.clone();

	thetaData thedata=getTheta(g_pBinaryImage);

	Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
	Point2f k;
	after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);//theta.x是旋转角度//thedata.theta-90
	float angle= thedata.theta;
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
			if ((a[n].x - rec_width + 10) > 0 && (a[n].y - rec_width + 10) > 0)
			{
				k = a[n];
				break;
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
			if ((a[n].x + rec_width - 10) <after.cols && (a[n].y - rec_width + 10) > 0)
			{
				//drawCenter(after, k, 0, 10);
				//drawROI(after, k.x+10, k.y - rec_width + 10, rec_width - 20, rec_width - 20);
				//imshow("2", after);
				//waitKey(0);
				k = a[n];
				break;
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
	        if ((a[n].x + rec_width - 10) <after.cols && (a[n].y + rec_width - 10) <after.rows)
			{
				k = a[n];
				break;
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
			if ((a[n].x - rec_width + 10) > 0 && (a[n].y + rec_width - 10) <after.rows)
			{
				k = a[n];
				break;
			}
		}
	}
	else
		return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
	float second=angleangin(after, k);
	if (second != 0)
	{
		after = ImageRotate(after, center, second);
		after2 = after.clone();
		k = getPointAffinedPos(k, center, second);
		angle = angle+second;
	}
	number= numbertest(k, after);
	if (number.theta < 1 || number.theta>64)
		return nothing;
	drawCenter(after2, number.center, 255, 5);
	Point2f afterpoint=getRealXY(number.center, 4);
	int shang = int(number.theta-1) / 8;
	int yushu = int(number.theta-1) % 8;
	Point2f location;
	location.x = yushu * 50 + 25 - afterpoint.x;
	location.y = shang * 50 + 25 + afterpoint.y;
	last.center.x = location.x; last.center.y = location.y;
	last.theta = angle;

	//char outputfile[70] = "D:/原D/车队/大三下/创意比赛/图像2/";
	//Im_write(outputfile, num, after2);
	//num++;
    return last;
	////if(num>300)
	////break;
	//} while (g_srcImage.data);

	cv::imshow("corner", g_srcImage1);
	cv::imshow("rotate", after2);
	cv::imshow("binary", g_pBinaryImage);
	waitKey(0);
}

int num = 0;

ROIData maindoCamera()
{

	ROIData nothing; nothing.center.x = 0; nothing.center.y = 0; nothing.theta = 0;
	ROIData last;



	if (Image.empty())
		return nothing;
	Image.copyTo(g_srcImage);

    //int num = 0; 如果存图函数外应有num
	char outputfile1[70] = "../memory/";
	//char outputfile2[70] = "D:/车队/大三上/德国赛/关于图像识别/新图左边界结果/";
	Im_write(outputfile1, num,g_srcImage );

	num++;

	g_srcImage1 = prospective(g_srcImage);

	CornerPoint(g_srcImage1);

	threshold(g_grayImage, g_pBinaryImage, 160, 255, CV_THRESH_BINARY);//在前面的CornerPoint里有转灰度图
	g_pBinaryImage1 = g_pBinaryImage.clone();

	thetaData thedata = getTheta(g_pBinaryImage);

	Point2f center; center.x = g_srcImage.cols / 2; center.y = g_srcImage.rows / 2;
	Point2f k;
	after = ImageRotate(g_pBinaryImage1, center, thedata.theta - 90);//theta.x是旋转角度//thedata.theta-90
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
			if ((a[n].x - rec_width + 10) > 0 && (a[n].y - rec_width + 10) > 0)
			{
				k = a[n];
				break;
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
			if ((a[n].x + rec_width - 10) <after.cols && (a[n].y - rec_width + 10) > 0)
			{
				k = a[n];
				break;
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
			if ((a[n].x + rec_width - 10) <after.cols && (a[n].y + rec_width - 10) <after.rows)
			{
				k = a[n];
				break;
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
			if ((a[n].x - rec_width + 10) > 0 && (a[n].y + rec_width - 10) <after.rows)
			{
				k = a[n];
				break;
			}
		}
	}
	else
		return nothing;//	break;//在开着摄像头时使用 没有找到合适点 则加载下一张图
	float second = angleangin(after, k);
	if (second != 0)
	{
		after = ImageRotate(after, center, second);
		after2 = after.clone();
		k = getPointAffinedPos(k, center, second);
		angle = angle + second;
	}
	ROIData number = numbertest(k, after);
	if (number.theta < 1 || number.theta>64)
		return nothing;
	drawCenter(after2, number.center, 255, 5);
	Point2f afterpoint = getRealXY(number.center, 4);
	int shang = int(number.theta - 1) / 8;
	int yushu = int(number.theta - 1) % 8;
	Point2f location;
	location.x = yushu * 50 + 25 - afterpoint.x;
	location.y = shang * 50 + 25 + afterpoint.y;
	last.center.x = location.x; last.center.y = location.y;
	last.theta = angle;
	//Im_write(outputfile1, num, after2);
	//num++;
	return last;

}

// int main()
// {
// 	//maindoPicture();
// 	//maindoCamera();
// 	for(int i=0;i<20;i++)
//     {
//         if(maindoCamera().center.x == 0)
//          {
//             printf("nothing!\n");

//         }
//     }
//     printf("%d\n",num );
// }

