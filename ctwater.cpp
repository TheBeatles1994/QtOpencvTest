#include "ctwater.h"

/*
 * 函数功能：
 * 测试骨架提取
 * mulMat未解决
 */
void testSkeleton()
{
    Mat srcMat = imread("D://10.png");  //8UC3

    CTSkeleton ske;
    Mat skeMat= ske.getZhangSkeletonMat(srcMat);        //8UC1
    Mat distMat = ske.getDistanceTransformMat(srcMat);  //8UC1
    Mat mulMat = ske.multiMats(skeMat,distMat);
    cout<<"mulMat type:"<<mulMat.type()<<" mulMat ch:"<<mulMat.channels()<<endl;
    debugShowMat(mulMat);
    debugSaveMat(mulMat,"test3.jpg");
    int temp = ske.findValleyPoints(mulMat).size();
    cout << temp<<endl;
    return;
}
/*
 * 函数功能：
 * zhang快速并行细化
 * 种子中接近胚的位置有白色的“气泡”，这样显示出的骨骼图有一个圆圈，故先预处理
 */
Mat CTSkeleton::getZhangSkeletonMat(Mat srcMat)
{
    Mat outputImage = preTreatmentMat(srcMat);

    thinning(outputImage);

    return outputImage;
}
/*
 * 函数功能：
 * 距离变换
 * 输入为灰度图
 * 发现distanceTransform函数中的第三个参数DistanceTypes换成CV_DIST_C（distance = max(|x1-x2|,|y1-y2|)）后效果挺好
 * 当DistanceTypes用CV_DIST_C即距离，得到的outMat类型为CV_32FC1
 * 当DistanceTypes用CV_DIST_L2即求欧式距离，得到的outMat类型为CV_32FC1
 */
Mat CTSkeleton::getDistanceTransformMat(Mat srcMat)
{
    Mat greyMat = preTreatmentMat(srcMat);

    Mat outMat;                  //定义保存距离变换结果的Mat矩阵
    distanceTransform(greyMat,outMat,CV_DIST_C,3);      //距离变换，第四个参数不知道啥意思
    //cout<<"type: "<<outMat.type()<<endl;
    normalize(outMat,outMat,0,255,CV_MINMAX, 0); //为了显示清晰，做了0~255归一化，第六个参数为0表示为8UC1
    return outMat;
}
/*
 * 函数功能：
 * 预处理种子图像，三通道或四通道彩色图
 * 问题：当处理三个种子黏连时候，中间的会被全部填满！
 * 先解决两个种子的，即先不考虑三个的情况，以后解决这个问题
 */
Mat CTSkeleton::preTreatmentMat(Mat srcMat)
{
    assert(srcMat.channels() == 3 ||srcMat.channels()==4);
    Mat outputImage = imageBinarizationBorW(srcMat);    //输出8UC1
    /* 图像预处理，先去掉种子内部的白点 */
    /* 先找到包络，去掉边缘毛刺 */
//    vector<Point> vPoint = CTContour::findImageContours(outputImage)[0];
//    outputImage = CTContour::vecPointToMat(vPoint,0,0,0,4);
//    outputImage = imageBinarizationBorW(outputImage);
    /* 闭操作，去掉内部无关点 */
    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
    morphologyEx(outputImage, outputImage, MORPH_CLOSE,element);

    return outputImage;
}
/*
 * 函数功能：
 * 寻找骨骼和距离变换图像的共同区
 */
Mat CTSkeleton::multiMats(Mat skeMat, Mat distMat)
{
    assert(skeMat.size == distMat.size);
    Mat rstMat = Mat::zeros(distMat.rows,distMat.cols,skeMat.type());
    //下面函数不知道为什么一半以上都没了。。。。
    //cout<<skeMat.type()<<" "<<distMat.type()<<endl;
    //cout<<skeMat.channels()<<" "<<distMat.channels()<<endl;
    for(int i=0;i<rstMat.rows;i++)
    {
        for(int j=0;j<rstMat.cols;j++)
        {
            if((int)skeMat.at<uchar>(i,j) >= 240)
            {
                if(distMat.at<float>(i,j)>=20)
                    rstMat.at<uchar>(i,j) = distMat.at<float>(i,j);
            }
            else
            {
                rstMat.at<uchar>(i,j) = 0;
            }
        }
    }

    return rstMat;
}
/*
 * 函数功能：
 * 寻找分叉点
 */
vector<Point> CTSkeleton::findForks(Mat multiMat)
{

}
/*
 * 函数功能：
 * 寻找端点
 */
vector<Point> CTSkeleton::findEndPoints(Mat multiMat)
{
    vector<Point> vPoint;
    //除去边缘一圈的点
    for(int i=1;i<multiMat.rows-1;i++)
    {
        for(int j=1;j<multiMat.cols-1;j++)
        {
            if((multiMat.at<uchar>(i,j)>=20))
            {
                if(getNeighbourPointNum(multiMat, Point(i, j)) == 1)
                {
                    vPoint.push_back(Point(i, j));
                    cout <<"point: "<< i <<" "<<j<<endl;
                }
            }
        }
    }
    return vPoint;
}
/*
 * 函数功能：
 * 寻找谷点
 * 待写，需要先写一个矩阵，用来记录每一网格的状态（骨骼上未走过为0，走过为1，背景为1）
 */
vector<Point> CTSkeleton::findValleyPoints(Mat multiMat)
{

}
/*
 * 函数功能：
 * 中指滤波两点间的骨骼线
 */
Mat CTSkeleton::medianFilter(Mat multiMat)
{

}
/*
 * 函数功能：
 * 返回一点周围非零点个数
 */
int CTSkeleton::getNeighbourPointNum(Mat srcMat, Point point)
{
    int count=0;

    for(int i=point.x-1;i<=point.x+1;i++)
    {
        for(int j=point.y-1;j<=point.y+1;j++)
        {
            if(srcMat.at<uchar>(i,j)>=20)
                count++;
        }
    }
    if(srcMat.at<uchar>(point.x, point.y)>=20)
        count--;

    return count;
}
/*
 * 函数功能：
 * 细化方法中的一次迭代，其中iter为0或1
 * 算法的一次迭代需要调用两次thinningIteration函数
 */
void CTSkeleton::thinningIteration(Mat &im, int iter)
{
    Mat marker = Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            /* 除去边缘点 */
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            /* A为P2到P9顺时针0到1变化次数：要求1 */
            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
                    (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
                    (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                    (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            /* B为P1的八邻域非零点个数 */
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            /* 按照迭代次序分为两种不同的m1和m2，要求两指均为0 */
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;  //原图中除去所有标记了的点
}
/**
 * Function for thinning the given binary image
 */
void CTSkeleton::thinning(Mat &im)
{
    im /= 255;                      //图像中的像素值变为0，1分布（原来就是二值图像，即0和255）

    Mat prev = Mat::zeros(im.size(), CV_8UC1);
    Mat diff;

    do {
        thinningIteration(im, 0);
        thinningIteration(im, 1);
        absdiff(im, prev, diff);    //帧差法
        im.copyTo(prev);
    }
    while (countNonZero(diff) > 0);

    im *= 255;
}
/*
 * 函数功能：
 * 图像二值化
 * 输入必须是三通道或四通道图，输出是灰度图
 * 灰度值大于SEEDGRAYVALUE的为白色，小于SEEDGRAYVALUE的为黑色
 * 种子是黑色，背景是白色
 */
#define SEEDGRAYVALUE 75                //定义灰度值阈值
Mat CTSkeleton::imageBinarizationThreshold(Mat srcMat)
{
    assert(srcMat.channels() == 3 || srcMat.channels() == 4);
    Mat tempMat;
    cvtColor(srcMat,tempMat,CV_BGR2GRAY);
    threshold(tempMat, tempMat, SEEDGRAYVALUE, 255, CV_THRESH_BINARY);
    return tempMat;
}
/*
 * 函数功能：
 * 图像二值化
 * 输入是原图，输出是灰度图
 * 非黑变黑，黑变白
 * 当种子是白色，背景是黑色时
 * 才能找到边缘点集，若用imageBinarization()函数则不行
 */
Mat CTSkeleton::imageBinarizationBorW(Mat srcMat)
{
    assert(srcMat.channels() == 3||srcMat.channels() == 4);
    Mat greyMat;
    greyMat = imageBinarizationThreshold(srcMat);
    threshold(greyMat, greyMat, 0, 255, CV_THRESH_BINARY_INV);
    return greyMat;
}
