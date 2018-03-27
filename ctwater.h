#ifndef CTWATER_H
#define CTWATER_H

#include <opencv2/core/core.hpp>
#include"opencv/highgui.h"
#include"opencv2/imgproc/imgproc.hpp"
#include"ctalign.h"

using namespace cv;

class CTSkeleton
{
public:
    Mat getIndexSkeletonMat(Mat srcMat);                    //索引表细化
    Mat getZhangSkeletonMat(Mat srcMat);                    //zhang快速并行细化
private:
    bool thiningSkeletonByIndex(unsigned char *lpDIBBits, int lWidth, int lHeight); //索引表核心功能函数
    void thinningIteration(Mat& im, int iter);              //Zhang：Perform one thinning iteration.
    void thinning(Mat& im);                                 //Zhang：Function for thinning the given binary image
    Mat imageBinarizationThreshold(Mat srcMat);             //种子图像二值化,种子是黑色，背景是白色
    Mat imageBinarizationBorW(Mat srcMat);                  //种子图像二值化,非黑变黑，黑变白
};

void testSkeleton();            //测试骨架提取

#endif // CTWATER_H
