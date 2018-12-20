#ifndef ALGOC_H
#define ALGOC_H

//#include<QCoreApplication>
#include<opencv/highgui.h>
#include<opencv2/imgproc/imgproc.hpp>
#include"watersshedsegmenter.h"
#include<iostream>
#include<memory>

using namespace std;
using namespace cv;

/* 边缘腐蚀类 */
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

/* 透明操作类 */
class CTAlpha
{
    friend class CTRotate;
    friend class CTShrink;
public:
    Mat getAlphaPic(vector<Point> vecPoint);                    //得到透明种子Mat，输入为边缘点集
    Mat getAlphaPic(Mat srcMat, int alpha);                                //得到透明种子Mat，输入为Mat
private:
    static Mat imageBinarizationThreshold(Mat srcMat);          //种子图像二值化,种子是黑色，背景是白色
    static Mat imageBinarizationBorW(Mat srcMat);               //种子图像二值化,非黑变黑，黑变白
};

/* 点集操作类 */
class CTContour
{
public:
    static vector<vector<Point> > findImageContours(Mat greyMat);                               //寻找图像中的边缘点集
    static Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue,int edgeSpacing);             //将特定的点集变为Mat，背景为白色，点集内为指定色
    static Mat vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha,int edgeSpacing);  //将特定的点集变为Mat，背景为透明色(由alpha参数决定，越高越透明)
    static Mat vecPointToMat(Mat srcMat, vector<Point> vecPoint, Scalar backColor,int alpha,int edgeSpacing);
    static pair<Mat, Mat> vecPointToMat(Mat srcMat, Mat dstMat, vector<Point> vecPoint, Scalar backColor, int alpha, int edgeSpacing);                    //将特定的点集变为Mat，种子为原图颜色
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
    Mat getRotateMat(Mat srcMat, float degree, int alpha = 0);                 //将种子转到绝对degree角度(自动将种子变垂直，用到了uprightMat函数)
    Mat getRelativeRotateMat(Mat srcMat, float degree, int alpha);         //将种子转到相对degree角度
    pair<Mat, Mat> getRelativeRotateMatAndLabel(Mat srcMat, Mat labelMat, float degree, int alpha);
    Mat panningMat(Mat srcMat, Mat &dstMat, float x, float y);      //使种子图片移动到dstMat图的目标位置
    Mat getMirrorMat(Mat srcMat, int type);                         //镜像变换
private:
    Mat quadrateMat(Mat srcMat,int spacing=0);                      //使种子图片变成正方形
    float getRotateMatDegree(Mat srcMat);                           //得到种子图片计算后的旋转角度
    float getRotateUprightDegree(RotatedRect calculatedRect);       //得到使旋转矩形竖直向垂直的角度
    Mat removeEdge(Mat srcMat, int alpha);                                     //去除图片边缘空隙
    pair<Mat, Mat> removeEdge(Mat srcMat, Mat labelMat, int alpha);
};
/* 紧密排列类 */
class CTAlign
{
public:
    /* 排列方式：竖直、水平 */
    enum AlignMode
    {
        VERTICAL,
        HORIZONTAL
    };
    /* 排列类型：左对齐、居中、右对齐 */
    enum AlignType
    {
        LEFT,
        MID,
        RIGHT
    };
public:
    CTAlign(vector<vector<Mat> > vecVecMat);                            //构造函数
    Mat alignMat(int arrangeMode, int arrangeAlign, int spacing=0);  //紧密排列
    void setAlignMat(Mat srcMat,int x,int y);                       //设置单个Mat
    void setAlignMats(vector<vector<Mat> > vecVecMat);              //设置多排Mat
    vector<Point> getAlignPoint(int x,int y);                      //获取单个Mat坐标信息
    vector<vector<vector<Point> > > getAlignPoints();               //获取全部Mat坐标信息
    vector<vector<Mat> > getAlignMats();                            //获取全部Mat
    Mat getAlignMat(int x,int y);                                   //获取单个Mat
private:
    vector<vector<vector<Point> > > vecPoints;
    vector<vector<Mat> > vecMats;
};


void testAlign();                                               //测试紧密排列
void testShrink();                                              //测试边缘腐蚀
void testDataAugmentation(Mat srcMat, Mat labelMat, string filename);                                    //测试数据增广函数
void debugShowMat(Mat showMat,string strName = "debug");        //显示图片
void debugSaveMat(Mat saveMat,string saveName = "saveMat.png"); //保存图片

#endif // ALGOC_H
