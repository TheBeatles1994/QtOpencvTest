#ifndef ALGOC_H
#define ALGOC_H

#include<QCoreApplication>
#include<opencv/highgui.h>
#include<opencv2/imgproc/imgproc.hpp>
#include"watersshedsegmenter.h"
#include<iostream>

using namespace std;
using namespace cv;

class CvTest
{
public:
    CvTest();
    CvTest(Mat mymat);
    void startEdgeShrink();                                 //执行边缘腐蚀操作
    Mat startAlphaPic(vector<Point> vecPoint);              //执行透明化操作
    Mat startAlphaPic(Mat srcMat);                          //执行透明化操作
    Mat startRotatePic(Mat srcMat, int degree);             //执行图像旋转
    float getRotateDegree(Mat srcMat);                      //得到种子旋转角度
    void debugShowMat(Mat showMat);                         //显示图片

private:
    Mat backgroundMark(Mat srcMat);                         //生成背景标签
    Mat foregroundMark(Mat srcMat);                         //生成前景标签
    Mat watershedProc(Mat mymat, Mat srcMat);               //分水岭算法
    Mat edgeShrink(Mat srcMat);                             //边缘腐蚀
    Mat edgeGenerate(Mat back, Mat fore);                   //边缘生成
    Mat imageBinarizationThreshold(Mat srcMat);             //种子图像二值化,种子黑色，背景白色
    Mat imageBinarizationBorW(Mat srcMat);                  //种子图像二值化,种子白色，背景黑色
    vector<vector<Point> > findImageContours(Mat greyMat);  //寻找图像中的边缘点集
    Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue);            //将特定的点集变为Mat，背景为白色，点集内为指定色
    Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha); //将特定的点集变为Mat，背景为透明色(由alpha参数决定，越高越透明)
};

#endif // ALGOC_H
