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
    enum DIR
    {
        UPLEFT,
        UPMID,
        UPRIGHT,
        MIDRIGHT,
        DOWNRIGHT,
        DOWNMID,
        DOWNLEFT,
        MIDLEFT
    };
public:
    Mat getZhangSkeletonMat(Mat srcMat);                    //zhang快速并行细化
    Mat getDistanceTransformMat(Mat srcMat);                //距离变换
    Mat preTreatmentMat(Mat srcMat);                        //预处理种子图像
    Mat multiMats(Mat skeMat, Mat distMat);                 //生成骨骼和距离变换图像的共同区
    vector<Point> findEndPoints(Mat flagMat);              //寻找端点
    vector<Point> findCrossPoints(Point startPoint, Mat flagMat);            //寻找交叉点
    Mat waterDropSegment(Mat distMat, Point waterPoint);     //水滴分割
    Mat createFlagMat(Mat multiMat);                        //创建标志矩阵
private:
    void thinningIteration(Mat& im, int iter);              //Zhang：Perform one thinning iteration.
    void thinning(Mat& im);                                 //Zhang：Function for thinning the given binary image
    Mat imageBinarizationThreshold(Mat srcMat);             //种子图像二值化,种子是黑色，背景是白色
    Mat imageBinarizationBorW(Mat srcMat);                  //种子图像二值化,非黑变黑，黑变白
    int getNeighbourPointNum(Point point, const Mat &flagMat);      //返回一点周围非零点个数
    vector<Point> getRCrossNeighbourPoints(Point point, const Mat &flagMat);//返回一点周围非十字的非零点
    vector<Point> getCrossNeighbourPoints(Point point, const Mat &flagMat);//返回一点周围十字的非零点
    bool doFindCrossPoints(Point startPoint, Mat &flagMat);
    vector<Point> getNeighbourPoints(Point point, const Mat &flagMat);
    vector<Point> getCrossNeighbourGreyPoints(Point point, const Mat &flagMat);
    vector<Point> getRCrossNeighbourGreyPoints(Point point, const Mat &flagMat);
private:
    vector<Point> crossVec; //分割点集合

};

class CrossPoint
{
public:
    CrossPoint(Point point, vector<Point> vecPoint, Mat mat):crossPoint(point),crossPoints(vecPoint),flagMat(mat){findPath();}
    vector<Point> getValidCrossPoints();
    vector<vector<Point> > getRefPoints();
private:
    void findPath();   //开始处理
    bool doFindPath(Point startPoint,Mat &flagMat);
private:
    Point crossPoint;   //交叉点
    vector<Point> crossPoints;  //交叉点集合
    vector<Point> crossValidPoints; //可达到的交叉点
    vector<vector<Point> > refPoints;   //本交叉点到可到达交叉点的点集
    Mat flagMat;    //flagMat
};

void testSkeleton();            //测试骨架提取

#endif // CTWATER_H
