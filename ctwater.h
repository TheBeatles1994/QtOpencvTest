#ifndef CTWATER_H
#define CTWATER_H

#include <opencv2/core/core.hpp>
#include"opencv/highgui.h"
#include"opencv2/imgproc/imgproc.hpp"
#include"ctalign.h"

using namespace cv;
/*
 * 备注：
 * 提取极值点
 * http://matlab.izmiran.ru/help/toolbox/images/morph13.html
 * Finding Peaks and Valleys
 */
class CTSkeleton
{
public:
    Mat getZhangSkeletonMat(Mat srcMat);                    //zhang快速并行细化
    Mat getDistanceTransformMat(Mat srcMat);                //距离变换
    Mat preTreatmentMat(Mat srcMat);                        //预处理种子图像
    Mat multiMats(Mat skeMat, Mat distMat);                 //生成骨骼和距离变换图像的共同区
    Mat flagMats(Mat mulMat);                               //生成flag Mat
    vector<Point> findEndPoints(Mat multiMat);              //寻找端点
    vector<Point> findValleyPoints(Mat multiMat);           //寻找谷点
    vector<Point> findCrossPoints(Mat multiMat);            //寻找交叉点
    Mat medianFilter(Mat multiMat);                         //中指滤波两点间的骨骼线

private:
    void thinningIteration(Mat& im, int iter);              //Zhang：Perform one thinning iteration.
    void thinning(Mat& im);                                 //Zhang：Function for thinning the given binary image
    Mat imageBinarizationThreshold(Mat srcMat);             //种子图像二值化,种子是黑色，背景是白色
    Mat imageBinarizationBorW(Mat srcMat);                  //种子图像二值化,非黑变黑，黑变白
    vector<Point> findForks(Mat multiMat);                  //寻找分叉点
    int getNeighbourPointNum(Mat srcMat, Point point);      //返回一点周围非零点个数
};

void testSkeleton();            //测试骨架提取

#endif // CTWATER_H
