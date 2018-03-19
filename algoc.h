#ifndef ALGOC_H
#define ALGOC_H

#include<QCoreApplication>
#include<opencv/highgui.h>
#include<opencv2/imgproc/imgproc.hpp>
#include"watersshedsegmenter.h"
#include<iostream>

using namespace std;
using namespace cv;

class CTShrink
{
public:
    void startEdgeShrink(Mat srcMat);                       //执行边缘腐蚀操作
private:
    Mat backgroundMark(Mat srcMat);                         //生成背景标签
    Mat foregroundMark(Mat srcMat);                         //生成前景标签
    Mat watershedProc(Mat mymat, Mat srcMat);               //分水岭算法
    Mat edgeShrink(Mat srcMat);                             //边缘腐蚀
    Mat edgeGenerate(Mat back, Mat fore);                   //边缘生成
};

/* 点集操作类 */
class CTContour
{
    friend class CTRotate;
    friend class CTAlpha;
private:
    static vector<vector<Point> > findImageContours(Mat greyMat);                               //寻找图像中的边缘点集
    static Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue);             //将特定的点集变为Mat，背景为白色，点集内为指定色
    static Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha);  //将特定的点集变为Mat，背景为透明色(由alpha参数决定，越高越透明)
};
/* 旋转操作类 */
class CTRotate
{
public:
    enum RotateDegree
    {
        DEGREE0 = 0,
        DEGREE90 = 90,
        DEGREE180 = 180,
        DEGREE270 = 270
    };
    enum MirrorType
    {
        MIRRORX = 0,
        MIRRORY = 1,
        MIRRORXY = -1
    };
public:
    Mat getRotateMat(Mat srcMat, float degree = 0);             //将种子转到固定degree角度(自动将种子变垂直，用到了uprightMat函数)
    Mat panningMat(Mat srcMat, Mat &dstMat, float x, float y);  //使种子图片移动到dstMat图的目标位置
    Mat mirrorMat(Mat srcMat, int type);                        //镜像变换
private:
    Mat quadrateMat(Mat srcMat);                                //使种子图片变成正方形
    float getRotateMatDegree(Mat srcMat);                       //得到种子图片计算后的旋转角度
    float getRotateUprightDegree(RotatedRect calculatedRect);   //得到使旋转矩形竖直向垂直的角度
    Mat removeEdge(Mat srcMat);                                 //去除图片边缘空隙
};
/* 透明操作类 */
class CTAlpha
{
    friend class CTRotate;
    friend class CTShrink;
public:
    Mat getAlphaPic(vector<Point> vecPoint);                    //得到透明种子Mat，输入为边缘点集
    Mat getAlphaPic(Mat srcMat);                                //得到透明种子Mat，输入为Mat
private:
    static Mat imageBinarizationThreshold(Mat srcMat);          //种子图像二值化,种子是黑色，背景是白色
    static Mat imageBinarizationBorW(Mat srcMat);               //种子图像二值化,非黑变黑，黑变白
};

void testClass(Mat srcMat);                                     //测试ROI区域
void debugShowMat(Mat showMat,string strName = "debug");        //显示图片
void debugSaveMat(Mat saveMat,string saveName = "saveMat.png"); //保存图片

#endif // ALGOC_H
