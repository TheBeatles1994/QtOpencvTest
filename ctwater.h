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
    Mat getIndexSkeletonMat(Mat srcMat);                    //索引表细化
    Mat getZhangSkeletonMat(Mat srcMat);                    //zhang快速并行细化
    Mat getDistanceTransformMat(Mat srcMat);                //距离变换
    Mat preTreatmentMat(Mat srcMat);                        //预处理种子图像
    void filterOver(cv::Mat thinSrc);                       //对骨骼化图数据进行过滤，实现两个点之间至少隔一个空白像素
    std::vector<cv::Point> getPoints(const cv::Mat &thinSrc, unsigned int raudis = 4, unsigned int thresholdMax = 6, unsigned int thresholdMin = 4);
    //void findPeaks(Mat srcMat);
    void findValleys(Mat srcMat);
    Mat multiMats(Mat skeMat, Mat distMat);                       //寻找骨骼和距离变换图像的共同区
private:
    bool thiningSkeletonByIndex(unsigned char *lpDIBBits, int lWidth, int lHeight); //索引表核心功能函数
    void thinningIteration(Mat& im, int iter);              //Zhang：Perform one thinning iteration.
    void thinning(Mat& im);                                 //Zhang：Function for thinning the given binary image
    Mat imageBinarizationThreshold(Mat srcMat);             //种子图像二值化,种子是黑色，背景是白色
    Mat imageBinarizationBorW(Mat srcMat);                  //种子图像二值化,非黑变黑，黑变白
    int doFindValley(Mat srcMat, int row, int col);
};

void testSkeleton();            //测试骨架提取

#endif // CTWATER_H
