#ifndef ALGOC_H
#define ALGOC_H

#include<QCoreApplication>
#include<opencv/highgui.h>
#include<opencv2/imgproc/imgproc.hpp>
#include"watersshedsegmenter.h"
#include<iostream>

using namespace std;
using namespace cv;

#define TESTMARIO 0

class CvTest
{
public:
    CvTest();
    void startEdgeShrink(Mat srcMat);                       //执行边缘腐蚀操作
    Mat alphaPic(vector<Point> vecPoint);                   //执行透明化操作
    Mat alphaPic(Mat srcMat);                               //执行透明化操作
    Mat rotatePic(Mat srcMat, int degree);                  //执行图像旋转
    float getRotateDegree(Mat srcMat);                      //得到种子图片旋转角度
    float getRotateRectDegree(RotatedRect calculatedRect);  //得到旋转矩形真实角度
    void debugShowMat(Mat showMat);                         //显示图片
    void debugShowMat(string strName, Mat showMat);         //显示图片
    Mat removeEdge(Mat srcMat);                             //去除图片边缘空隙

private:
    Mat backgroundMark(Mat srcMat);                         //生成背景标签
    Mat foregroundMark(Mat srcMat);                         //生成前景标签
    Mat watershedProc(Mat mymat, Mat srcMat);               //分水岭算法
    Mat edgeShrink(Mat srcMat);                             //边缘腐蚀
    Mat edgeGenerate(Mat back, Mat fore);                   //边缘生成
    Mat imageBinarizationThreshold(Mat srcMat);             //种子图像二值化,种子黑色，背景白色
    Mat imageBinarizationBorW(Mat srcMat);                  //种子图像二值化,种子白色，背景黑色
    Mat quadrateMat(Mat srcMat);                            //使图片变成正方形
    vector<vector<Point> > findImageContours(Mat greyMat);  //寻找图像中的边缘点集
    Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue);            //将特定的点集变为Mat，背景为白色，点集内为指定色
    Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha); //将特定的点集变为Mat，背景为透明色(由alpha参数决定，越高越透明)
    Point2f rotPoint(const Mat &R, const Point2f &p);
    Size rotatedImageBB(const Mat &R, const Rect &bb);
};

#endif // ALGOC_H
