#include "ctwater.h"

/*
 * 函数功能：
 * 测试骨架提取
 */
void testSkeleton()
{
    Mat srcMat = imread("D://10.png");

    CTSkeleton ske;
    Mat skeMat= ske.getZhangSkeletonMat(srcMat);
    Mat distMat = ske.getDistanceTransformMat(srcMat);
    //debugShowMat(skeMat);
    //debugSaveMat(skeMat,"test1.jpg");
    //debugShowMat(distMat);
    //debugSaveMat(distMat,"test2.jpg");
    Mat mulMat = ske.multiMats(skeMat,distMat);
    debugShowMat(mulMat);
    debugSaveMat(mulMat,"test3.jpg");
    //ske.findValleys(distMat);

    return;
}
/*
 * 函数功能：
 * 索引表提取骨架方法
 * 提取骨架：用户级接口
 * 输入为二值灰度图，目标物为白色，背景为黑色
 */
Mat CTSkeleton::getIndexSkeletonMat(Mat srcMat)
{
    assert(srcMat.channels() == 3 ||srcMat.channels()==4);
    srcMat = imageBinarizationBorW(srcMat);

    IplImage* pBinary = new IplImage(srcMat);   //浅拷贝，拷贝指针，会对原图进行操作
    IplImage *input = cvCloneImage(pBinary);    //深拷贝

    unsigned char* imagedata;
    imagedata = new uchar[sizeof(char)*input->width*input->height]();
    for(int y=0;y<input->height;y++)
    {
        unsigned char* ptr  = (unsigned char*)(input->imageData + y*input->widthStep);
        for(int x=0;x<input->width;x++)
        {
            imagedata[y*input->width+x] = ptr[x] > 0 ? 1 : 0;
        }
    }

    if(thiningSkeletonByIndex(imagedata, input->width, input->height))
    {
        for(int y=0;y<input->height;y++)
        {
            unsigned char* ptr  = (unsigned char*)(input->imageData + y*input->widthStep);
            for(int x=0;x<input->width;x++)
            {
                ptr[x] = imagedata[y*input->width + x]>0? 255 : 0;
            }

        }
        return cvarrToMat(input);         //浅拷贝
    }
    else
    {
        return srcMat;
    }
}
/*
 * 函数功能：
 * zhang快速并行细化
 * 种子中接近胚的位置是白色，这样显示出的骨骼图有一个圆圈
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
 */
Mat CTSkeleton::getDistanceTransformMat(Mat srcMat)
{
    Mat greyMat = preTreatmentMat(srcMat);

    Mat outMat;                  //定义保存距离变换结果的Mat矩阵
    distanceTransform(greyMat,outMat,CV_DIST_L2,3);      //距离变换
    normalize(outMat,outMat,0,255,CV_MINMAX); //为了显示清晰，做了0~255归一化
    return outMat;
}
/*
 * 函数功能：
 * 预处理种子图像
 */
Mat CTSkeleton::preTreatmentMat(Mat srcMat)
{
    assert(srcMat.channels() == 3 ||srcMat.channels()==4);
    Mat outputImage = imageBinarizationBorW(srcMat);
    /* 图像预处理，先去掉种子内部的白点 */
    /* 先找到包络，去掉边缘毛刺 */
    vector<Point> vPoint = CTContour::findImageContours(outputImage)[0];
    outputImage = CTContour::vecPointToMat(vPoint,0,0,0,4);
    outputImage = imageBinarizationBorW(outputImage);
    /* 闭操作，去掉内部无关点 */
    dilate(outputImage, outputImage, Mat());
    erode(outputImage, outputImage, Mat());

    return outputImage;
}
/**
* @brief 对骨骼化图数据进行过滤，实现两个点之间至少隔一个空白像素
* @param thinSrc为输入的骨骼化图像,8位灰度图像格式，元素中只有0与1,1代表有元素，0代表为空白
*/
void CTSkeleton::filterOver(cv::Mat thinSrc)
{
    assert(thinSrc.type() == CV_8UC1);
    thinSrc /= 255;
    int width = thinSrc.cols;
    int height = thinSrc.rows;
    for (int i = 0; i < height; ++i)
    {
        uchar * p = thinSrc.ptr<uchar>(i);
        for (int j = 0; j < width; ++j)
        {
            // 实现两个点之间至少隔一个像素
            //  p9 p2 p3
            //  p8 p1 p4
            //  p7 p6 p5
            uchar p1 = p[j];
            if (p1 != 1) continue;
            uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
            uchar p8 = (j == 0) ? 0 : *(p + j - 1);
            uchar p2 = (i == 0) ? 0 : *(p - thinSrc.step + j);
            uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - thinSrc.step + j + 1);
            uchar p9 = (i == 0 || j == 0) ? 0 : *(p - thinSrc.step + j - 1);
            uchar p6 = (i == height - 1) ? 0 : *(p + thinSrc.step + j);
            uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + thinSrc.step + j + 1);
            uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + thinSrc.step + j - 1);
            if (p2 + p3 + p8 + p9 >= 1)
            {
                p[j] = 0;
            }
        }
    }
    thinSrc *= 255;
}
/**
* @brief 从过滤后的骨骼化图像中寻找端点和交叉点
* @param thinSrc为输入的过滤后骨骼化图像,8位灰度图像格式，元素中只有0与1,1代表有元素，0代表为空白
* @param raudis卷积半径，以当前像素点位圆心，在圆范围内判断点是否为端点或交叉点
* @param thresholdMax交叉点阈值，大于这个值为交叉点
* @param thresholdMin端点阈值，小于这个值为端点
* @return 为对src细化后的输出图像,格式与src格式相同，元素中只有0与1,1代表有元素，0代表为空白
*/
std::vector<cv::Point> CTSkeleton::getPoints(const cv::Mat &thinSrc, unsigned int raudis, unsigned int thresholdMax, unsigned int thresholdMin)
{
    assert(thinSrc.type() == CV_8UC1);
    thinSrc/=255;
    int width = thinSrc.cols;
    int height = thinSrc.rows;
    cv::Mat tmp;
    thinSrc.copyTo(tmp);
    std::vector<cv::Point> points;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (*(tmp.data + tmp.step * i + j) == 0)
            {
                continue;
            }
            int count=0;
            for (int k = i - raudis; k < i + raudis+1; k++)
            {
                for (int l = j - raudis; l < j + raudis+1; l++)
                {
                    if (k < 0 || l < 0||k>height-1||l>width-1)
                    {
                        continue;

                    }
                    else if (*(tmp.data + tmp.step * k + l) == 1)
                    {
                        count++;
                    }
                }
            }

            if (count > thresholdMax||count<thresholdMin)
            {
                Point point(j, i);
                points.push_back(point);
            }
        }
    }
    thinSrc*=255;
    return points;
}
/*
 * 函数功能：
 * 寻找谷点
 */
void CTSkeleton::findValleys(Mat srcMat)
{
    for(int i=1;i<srcMat.rows-1;i++)
    {
        for(int j=1;j<srcMat.cols-1;j++)
        {
            int vCount = doFindValley(srcMat, i, j);
            if(vCount == 8)
                cout<<"Valley: "<< i<<" "<<j<<" value: "<< (int)srcMat.at<uchar>(i,j)<< endl;
        }
    }
    cout << srcMat.rows<<" "<< srcMat.cols<<endl;
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
    cout<<skeMat.type()<<" "<<distMat.type()<<endl;
    cout<<skeMat.channels()<<" "<<distMat.channels()<<endl;
    for(int i=0;i<rstMat.rows;i++)
    {
        for(int j=0;j<rstMat.cols;j++)
        {
            if((int)skeMat.at<uchar>(i,j) >= 240)
            {
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
 * 提取骨架：类的用户
 * 方法：八连通－查表法
 * 链接：https://blog.csdn.net/lu597203933/article/details/14397605  输入时char *
 * 链接：https://blog.csdn.net/zhubaohua_bupt/article/details/76850555  两种图像骨架提取算法的研究
 * 该函数细化后有太多毛刺！
 * 索引表的原理？？？https://www.cnblogs.com/zhazhiqiang/p/4487950.html
 */
bool CTSkeleton::thiningSkeletonByIndex (unsigned char* lpDIBBits, int lWidth, int lHeight)
{
    //循环变量
    long i;
    long j;
    long lLength;

    unsigned char deletemark[256] = {      // 这个即为前人据8领域总结的是否可以被删除的256种情况
        0,0,0,0,0,0,0,1,    0,0,1,1,0,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,1,1,1,0,1,1,
        0,0,0,0,0,0,0,0,    1,0,0,0,1,0,1,1,
        0,0,0,0,0,0,0,0,    1,0,1,1,1,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,    0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,    1,0,0,0,1,0,1,1,
        1,0,0,0,0,0,0,0,    1,0,1,1,1,0,1,1,
        0,0,1,1,0,0,1,1,    0,0,0,1,0,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,0,1,0,0,1,1,
        1,1,0,1,0,0,0,1,    0,0,0,0,0,0,0,0,
        1,1,0,1,0,0,0,1,    1,1,0,0,1,0,0,0,
        0,1,1,1,0,0,1,1,    0,0,0,1,0,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,0,0,0,1,1,1,
        1,1,1,1,0,0,1,1,    1,1,0,0,1,1,0,0,
        1,1,1,1,0,0,1,1,    1,1,0,0,1,1,0,0
    };//索引表

    unsigned char p0, p1, p2, p3, p4, p5, p6, p7;
    unsigned char *pmid, *pmidtemp;    // pmid 用来指向二值图像  pmidtemp用来指向存放是否为边缘
    unsigned char sum;
    bool bStart = true;
    lLength = lWidth * lHeight;
    unsigned char *pTemp = new uchar[sizeof(unsigned char) * lWidth * lHeight]();  //动态创建数组 并且初始化

    //    P0 P1 P2
    //    P7    P3
    //    P6 P5 P4

    while(bStart)
    {
        bStart = false;

        //首先求边缘点
        pmid = (unsigned char *)lpDIBBits + lWidth + 1;
        memset(pTemp,  0, lLength);
        pmidtemp = (unsigned char *)pTemp + lWidth + 1; //  如果是边缘点 则将其设为1
        for(i = 1; i < lHeight -1; i++)
        {
            for(j = 1; j < lWidth - 1; j++)
            {
                if( *pmid == 0)                   //是0 不是我们需要考虑的点
                {
                    pmid++;
                    pmidtemp++;
                    continue;
                }
                p3 = *(pmid + 1);
                p2 = *(pmid + 1 - lWidth);
                p1 = *(pmid - lWidth);
                p0 = *(pmid - lWidth -1);
                p7 = *(pmid - 1);
                p6 = *(pmid + lWidth - 1);
                p5 = *(pmid + lWidth);
                p4 = *(pmid + lWidth + 1);
                sum = p0 & p1 & p2 & p3 & p4 & p5 & p6 & p7;
                if(sum == 0)
                {
                    *pmidtemp = 1;
                }

                pmid++;
                pmidtemp++;
            }
            pmid++;
            pmid++;
            pmidtemp++;
            pmidtemp++;
        }

        //现在开始删除
        pmid = (unsigned char *)lpDIBBits + lWidth + 1;
        pmidtemp = (unsigned char *)pTemp + lWidth + 1;

        for(i = 1; i < lHeight -1; i++)   // 不考虑图像第一行 第一列 最后一行 最后一列
        {
            for(j = 1; j < lWidth - 1; j++)
            {
                if( *pmidtemp == 0)
                {
                    pmid++;
                    pmidtemp++;
                    continue;
                }

                p3 = *(pmid + 1);
                p2 = *(pmid + 1 - lWidth);
                p1 = *(pmid - lWidth);
                p0 = *(pmid - lWidth -1);
                p7 = *(pmid - 1);
                p6 = *(pmid + lWidth - 1);
                p5 = *(pmid + lWidth);
                p4 = *(pmid + lWidth + 1);

                p1 *= 2;
                p2 *= 4;
                p3 *= 8;
                p4 *= 16;
                p5 *= 32;
                p6 *= 64;
                p7 *= 128;

                sum = p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7;
            //  sum = p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7;
                if(deletemark[sum] == 1)
                {
                    *pmid = 0;
                    bStart = true;      //  表明本次扫描进行了细化
                }
                pmid++;
                pmidtemp++;
            }

            pmid++;
            pmid++;
            pmidtemp++;
            pmidtemp++;
        }
    }
    delete []pTemp;
    return true;
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
/*
 * 函数功能：
 * 查找谷点私有函数
 */
int CTSkeleton::doFindValley(Mat srcMat, int row, int col)
{
    int count = 0;

    for(int i : { -1, 0, 1 })
    {
        for(int j : { -1, 0, 1 })
        {
            count += (srcMat.at<uchar>(row+i,col+j) > srcMat.at<uchar>(row,col));
        }
    }
//    if(count == 8)
//    {
//        for(int i : { -1, 0, 1 })
//        {
//            for(int j : { -1, 0, 1 })
//            {
//                cout<< (int)srcMat.at<uchar>(row+i,col+j) <<" ";
//            }
//            cout<<endl;
//        }
//    }

    return count;
}
