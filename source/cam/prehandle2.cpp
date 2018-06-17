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

/*@note srcTri[]数组的配置
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


/*@note 获取变换矩阵
**@para Mat* src:原始图像
**@note 在摄像头位置变动后，请自行设置函数内参数
*/
void getmap_matrix2() {
	int dot[8] = { 213,301,342,304,362,440,160,437 };
	Size newCanvas = Size(640, 640);//设置新画布分辨率

	int ratio = 4;//4个像素1厘米
	float side_len = 3.15;//∵640pixel=160cm,4pixel=1cm∴ratio=4;小方格边长side_len=3.15cm 
	int bias = 4.8;//距后轴40cm处为图像底边，60cm处为黑白格底线，Δ=20cm   //chuangyi 51.8  52.8

				   /*以下一般无需更改*/
	Point2f srcTri[4], dstTri[4];
	srcTri_config2(srcTri, dot);

	int width = newCanvas.width, height = newCanvas.height;
	dstTri[0].x = width / 2 - side_len * 4 * ratio;
	dstTri[0].y = height - bias*ratio - side_len * 9 * ratio;
	dstTri[1].x = dstTri[0].x + side_len * 7 * ratio;
	dstTri[1].y = dstTri[0].y;
	dstTri[2].x = dstTri[1].x;
	dstTri[2].y = height - bias*ratio;
	dstTri[3].x = dstTri[0].x;
	dstTri[3].y = dstTri[2].y;

	map_matrix2 = getPerspectiveTransform(srcTri, dstTri);		//变换矩阵
	mapInv_matrix2 = getPerspectiveTransform(dstTri, srcTri);  //获取透视变换阵

}


/*note 输入(变换后)图像坐标，输出该点距车中心线的实际坐标Point.x(cm)和距车后轴的坐标Point.y(cm)
*/
Point2f getRealXY2(Point2f origin, int ratio = 4) {
	int width = 640, height = 640;//设置新画布分辨率
	float side_len = 3.15;//参数值与getmap_matrix2()相同
	int bias = 4.8;
	Point2f RealXY;
	RealXY.x = -(origin.x - width / 2) / ratio;
	RealXY.y = -(((height - bias*ratio) - origin.y) / ratio + 34.7);	//刘 此处可直接height-y+车后轴到图像底边距离
																//注：59.5：实际标注时，黑白格底边距车轴O点59.5cm；
	return RealXY;
}



/*@brief 一个完整的单图片变换例子
*@para sImag:输入图像
*@retun Mat:变换后图像
*/
Mat prospective2(Mat sImg) {
	Mat ImageIPM(640, 640, CV_32FC1);
	getmap_matrix2();
	warpPerspective(sImg, ImageIPM, map_matrix2, Size(640, 640));
	return ImageIPM;
}



