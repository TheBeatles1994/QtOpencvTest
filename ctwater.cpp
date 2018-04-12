#include "ctwater.h"

/*
 * 函数功能：
 * 测试骨架提取
 */
bool compDValue(Point point, vector<Point> vecPoint)
{
    int min=(-1)>>1;

    for(int i=0;i<vecPoint.size();i++)
    {
        Point temp = vecPoint[i];
        int tempDValue = (temp.x-point.x)*(temp.x-point.x) + (temp.y-point.y)*(temp.y-point.y);
        if(tempDValue<min)
            min = tempDValue;
    }

    if(min<10)
        return true;
    else
        return false;
}

void testSkeleton()
{
    /* 读取原图片 */
    Mat srcMat = imread("doc/10.png");  //8UC3
    //debugShowMat(srcMat);
    CTSkeleton ske;
    /* 得到骨骼图像 */
    Mat skeMat= ske.getZhangSkeletonMat(srcMat);        //8UC1
    //debugShowMat(skeMat);
    debugSaveMat(skeMat, "outimage/test1.jpg");
    /* 得到距离变换图像 */
    Mat distMat = ske.getDistanceTransformMat(srcMat);  //8UC1
    //debugShowMat(distMat);
    debugSaveMat(distMat, "outimage/test2.jpg");
    /* 寻找骨骼和距离变换图像的共同区 */
    Mat mulMat = ske.multiMats(skeMat,distMat);         //8UC1
    //debugShowMat(mulMat,"skeleton");
    //debugSaveMat(mulMat,"outimage/test3.jpg");
    /* 创建标志矩阵 */
    ske.createFlagMat(mulMat);
    /* 找端点 */
    vector<Point> endVec = ske.findEndPoints();
    cout <<"end point:"<<endl;
    for(auto i:endVec)
        cout<<i<<endl;
    /* 水滴法测试 */
//    Mat dropMat = ske.waterDropSegment(distMat, Point(55,70));
//    debugShowMat(dropMat);
    //debugSaveMat(dropMat,"outimage/test4.jpg");
    /* 寻找分叉点坐标 */
    cout <<"cross point:"<<endl;
    vector<Point> crossVec;
    crossVec = ske.findCrossPoints(endVec[0]);
    for(int i=1;i<endVec.size();i++)
    {
        ske.createFlagMat(mulMat);
        vector<Point> crossTempVec = ske.findCrossPoints(endVec[i]);
        for(int j=0;j<crossTempVec.size();j++)
        {
            if(!compDValue(crossTempVec[j], crossVec))
                crossVec.push_back(crossTempVec[j]);
        }
    }
    for(auto j:crossVec)
        cout<<j<<" ";

    /* 查看标志矩阵 */
//    Mat flagMat = ske.getFlagMat();
//    debugShowMat(flagMat);
//    debugSaveMat(flagMat, "outimage/flagMatPro.jpg");

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
                if(distMat.at<uchar>(i,j)>=20)
                    rstMat.at<uchar>(i,j) = distMat.at<uchar>(i,j);
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
 * 寻找端点
 */
vector<Point> CTSkeleton::findEndPoints()
{
    vector<Point> vPoint;
    //除去边缘一圈的点
    for(int i=1;i<flagMat.rows-1;i++)
    {
        for(int j=1;j<flagMat.cols-1;j++)
        {
            if((flagMat.at<uchar>(i,j)==0))
            {
                if(getNeighbourPointNum(Point(j, i)) == 1)
                {
                    vPoint.push_back(Point(j, i));
                }
            }
        }
    }
    return vPoint;
}
/*
 * 函数功能：
 * 寻找谷点
 */
vector<Point> CTSkeleton::findCrossPoints(Point startPoint)
{
    doFindCrossPoints(startPoint);
    return crossVec;
}
/*
 * 函数功能：
 * 水滴分割
 * 这里的灰度跟imageJ中的不一样，感觉不对啊（现在影响不大）
 * 现在不确定的是单个斜点有没有达到分割效果？？需不需要十字的分割方式
 */
Mat CTSkeleton::waterDropSegment(Mat distMat, Point waterPoint)
{
    assert(waterPoint.x<distMat.rows && waterPoint.y<distMat.cols);
    Mat rstMat(distMat);                     //结果图
    int flagI;                              //记录i的值的flag
    enum DIR dir[2];                        //表示山脊一边下一步方向
    Point point[2];
    int greyData[8], centerData, dValue;
    Point tempPoint[8];
    Point nextPoint;                        //记录下一点的坐标
    vector<Point> dropVec;
    int side;                               //搜索一边或两边
/*
 * 0 1 2
 * 7   3
 * 6 5 4
 * 注意斜角是偶数，上下左右是奇数
 */
    flagI = 0;          //记录最小下降点的i
    dValue = -255;      //记录最小差值
    centerData = distMat.at<uchar>(waterPoint);

    tempPoint[0] = Point(waterPoint.x-1, waterPoint.y-1);
    tempPoint[1] = Point(waterPoint.x, waterPoint.y-1);
    tempPoint[2] = Point(waterPoint.x+1, waterPoint.y-1);
    tempPoint[3] = Point(waterPoint.x+1, waterPoint.y);
    tempPoint[4] = Point(waterPoint.x+1, waterPoint.y+1);
    tempPoint[5] = Point(waterPoint.x, waterPoint.y+1);
    tempPoint[6] = Point(waterPoint.x-1, waterPoint.y+1);
    tempPoint[7] = Point(waterPoint.x-1, waterPoint.y);
    greyData[0] = (int)rstMat.at<uchar>(tempPoint[0]);
    greyData[1] = (int)rstMat.at<uchar>(tempPoint[1]);
    greyData[2] = (int)rstMat.at<uchar>(tempPoint[2]);
    greyData[3] = (int)rstMat.at<uchar>(tempPoint[3]);
    greyData[4] = (int)rstMat.at<uchar>(tempPoint[4]);
    greyData[5] = (int)rstMat.at<uchar>(tempPoint[5]);
    greyData[6] = (int)rstMat.at<uchar>(tempPoint[6]);
    greyData[7] = (int)rstMat.at<uchar>(tempPoint[7]);
    //先找到下降最陡的方向
    for(int i=0;i<8;i++)
    {
        if(centerData-greyData[i]>=dValue && centerData-greyData[i]<=150 && centerData-greyData[i]>=0)
        {
            dValue = centerData-greyData[i];
            flagI = i;
            dir[0] = (enum DIR)flagI;
            point[0] = tempPoint[i];
        }
    }
    //找到最陡方向的反方向
    if(flagI<=3)
    {
        point[1] = tempPoint[flagI+4];
        dir[1] = (enum DIR)(flagI+4);
    }
    else
    {
        point[1] = tempPoint[flagI-4];
        dir[1] = (enum DIR)(flagI-4);
    }
    //决定搜索两边还是一边
    if(greyData[dir[1]]>centerData)
    {
        side = 1;
    }
    else
    {
        side = 2;
    }
    //一方向
    for(int i=0;i<side;i++)
    {
        centerData = distMat.at<uchar>(point[i]);
        nextPoint = point[i];
        while(centerData != 0)
        {
            dropVec.push_back(nextPoint);

            flagI = -1;         //记录最小下降点的i
            dValue = -255;      //记录最小差值
            //tempPoint记录顺时针左中右三点
            switch (dir[i]) {
            case UPLEFT:
                tempPoint[0] = Point(nextPoint.x, nextPoint.y-1);
                tempPoint[1] = Point(nextPoint.x-1, nextPoint.y-1);
                tempPoint[2] = Point(nextPoint.x-1, nextPoint.y);
                break;
            case UPMID:
                tempPoint[0] = Point(nextPoint.x-1, nextPoint.y-1);
                tempPoint[1] = Point(nextPoint.x-1, nextPoint.y);
                tempPoint[2] = Point(nextPoint.x-1, nextPoint.y+1);
                break;
            case UPRIGHT:
                tempPoint[0] = Point(nextPoint.x-1, nextPoint.y);
                tempPoint[1] = Point(nextPoint.x-1, nextPoint.y+1);
                tempPoint[2] = Point(nextPoint.x, nextPoint.y+1);
                break;
            case MIDRIGHT:
                tempPoint[0] = Point(nextPoint.x-1, nextPoint.y+1);
                tempPoint[1] = Point(nextPoint.x, nextPoint.y+1);
                tempPoint[2] = Point(nextPoint.x+1, nextPoint.y+1);
                break;
            case DOWNRIGHT:
                tempPoint[0] = Point(nextPoint.x, nextPoint.y+1);
                tempPoint[1] = Point(nextPoint.x+1, nextPoint.y+1);
                tempPoint[2] = Point(nextPoint.x+1, nextPoint.y);
                break;
            case DOWNMID:
                tempPoint[0] = Point(nextPoint.x+1, nextPoint.y+1);
                tempPoint[1] = Point(nextPoint.x+1, nextPoint.y);
                tempPoint[2] = Point(nextPoint.x+1, nextPoint.y-1);
                break;
            case DOWNLEFT:
                tempPoint[0] = Point(nextPoint.x+1, nextPoint.y);
                tempPoint[1] = Point(nextPoint.x+1, nextPoint.y-1);
                tempPoint[2] = Point(nextPoint.x, nextPoint.y-1);
                break;
            case MIDLEFT:
                tempPoint[0] = Point(nextPoint.x+1, nextPoint.y-1);
                tempPoint[1] = Point(nextPoint.x, nextPoint.y-1);
                tempPoint[2] = Point(nextPoint.x-1, nextPoint.y-1);
                break;
            default:
                break;
            }
            //greyData记录顺时针左中右三点的灰度值
            greyData[0] = rstMat.at<uchar>(tempPoint[0]);
            greyData[1] = rstMat.at<uchar>(tempPoint[1]);
            greyData[2] = rstMat.at<uchar>(tempPoint[2]);
            //找到下落方向
            for(int i=0;i<3;i++)
            {
                if(centerData-greyData[i]>=dValue && centerData-greyData[i]<=150 && centerData-greyData[i]>=0)
                {
                    dValue = centerData-greyData[i];
                    flagI = i;
                }
            }
            //该点无法下落
            if(flagI == -1)
            {
                break;
            }
            //flag值为0 1 2
            if(flagI==0)
                dir[i] = (enum DIR)((7+(int)dir[i]-1)%7);
            else if(flagI==1)
                dir[i] = dir[i];
            else
                dir[i] = (enum DIR)((7+(int)dir[i]+1)%7);
            nextPoint = tempPoint[flagI];
            centerData = rstMat.at<uchar>(nextPoint);
        }
        if(flagI!=-1)
        {
            for(auto it=dropVec.begin();it!=dropVec.end();it++)
            {
                rstMat.at<uchar>(*it) = 0;
            }
        }
        vector<Point>().swap(dropVec);
    }

    rstMat.at<uchar>(waterPoint) = 0;

    return rstMat;
}
/*
 * 函数功能：
 * 返回一点周围非零点个数
 * 应当针对的是flagMat！
 */
int CTSkeleton::getNeighbourPointNum(Point point)
{
    return (getCrossNeighbourPoints(point).size() + getRCrossNeighbourPoints(point).size());
}
/*
 * 函数功能：
 * 返回一点周围非零点
 */
vector<Point> CTSkeleton::getNeighbourPoints(Point point)
{
    vector<Point> vecPoint, tempVecPoint;

    vecPoint = getCrossNeighbourPoints(point);
    tempVecPoint = getRCrossNeighbourPoints(point);
    vecPoint.insert(vecPoint.end(), tempVecPoint.begin(), tempVecPoint.end());

    return vecPoint;
}
/*
 * 函数功能：
 * 返回一点周围十字的非零点
 * 应当针对的是flagMat！
 *  1
 * 4 2
 *  3
 */
vector<Point> CTSkeleton::getCrossNeighbourPoints(Point point)
{
    vector<Point> vecPoint;

    if(flagMat.at<uchar>(point.y-1,point.x) == 0)         //1
        vecPoint.push_back(Point(point.x, point.y-1));
    if(flagMat.at<uchar>(point.y,point.x+1) == 0)         //2
        vecPoint.push_back(Point(point.x+1, point.y));
    if(flagMat.at<uchar>(point.y+1,point.x) == 0)         //3
        vecPoint.push_back(Point(point.x, point.y+1));
    if(flagMat.at<uchar>(point.y,point.x-1) == 0)         //4
        vecPoint.push_back(Point(point.x-1, point.y));

    return vecPoint;
}
/*
 * 函数功能：
 * 返回一点周围非十字的非零点
 * 应当针对的是flagMat！
 * 1 2
 *
 * 4 3
 */
vector<Point> CTSkeleton::getRCrossNeighbourPoints(Point point)
{
    vector<Point> vecPoint;

    if(flagMat.at<uchar>(point.y-1,point.x-1) == 0)         //1
        vecPoint.push_back(Point(point.x-1, point.y-1));
    if(flagMat.at<uchar>(point.y-1,point.x+1) == 0)         //2
        vecPoint.push_back(Point(point.x+1, point.y-1));
    if(flagMat.at<uchar>(point.y+1,point.x+1) == 0)         //3
        vecPoint.push_back(Point(point.x+1, point.y+1));
    if(flagMat.at<uchar>(point.y+1,point.x-1) == 0)         //4
        vecPoint.push_back(Point(point.x-1, point.y+1));

    return vecPoint;
}
/*
 * 函数功能：
 * 返回一点周围十字的灰度为128的点
 * 应当针对的是flagMat！
 *  1
 * 4 2
 *  3
 */
vector<Point> CTSkeleton::getCrossNeighbourGreyPoints(Point point)
{
    vector<Point> vecPoint;

    if(flagMat.at<uchar>(point.y-1,point.x) == 128)         //1
        vecPoint.push_back(Point(point.x, point.y-1));
    if(flagMat.at<uchar>(point.y,point.x+1) == 128)         //2
        vecPoint.push_back(Point(point.x+1, point.y));
    if(flagMat.at<uchar>(point.y+1,point.x) == 128)         //3
        vecPoint.push_back(Point(point.x, point.y+1));
    if(flagMat.at<uchar>(point.y,point.x-1) == 128)         //4
        vecPoint.push_back(Point(point.x-1, point.y));

    return vecPoint;
}
/*
 * 函数功能：
 * 返回一点周围非十字的灰度为128的点
 * 应当针对的是flagMat！
 * 1 2
 *
 * 4 3
 */
vector<Point> CTSkeleton::getRCrossNeighbourGreyPoints(Point point)
{
    vector<Point> vecPoint;

    if(flagMat.at<uchar>(point.y-1,point.x-1) == 128)         //1
        vecPoint.push_back(Point(point.x-1, point.y-1));
    if(flagMat.at<uchar>(point.y-1,point.x+1) == 128)         //2
        vecPoint.push_back(Point(point.x+1, point.y-1));
    if(flagMat.at<uchar>(point.y+1,point.x+1) == 128)         //3
        vecPoint.push_back(Point(point.x+1, point.y+1));
    if(flagMat.at<uchar>(point.y+1,point.x-1) == 128)         //4
        vecPoint.push_back(Point(point.x-1, point.y+1));

    return vecPoint;
}
/*
 * 函数功能：
 * 创建标志矩阵
 * 在使用寻找端点、分叉点函数前要调用此函数
 */
void CTSkeleton::createFlagMat(Mat multiMat)
{
    flagMat = Mat::zeros(multiMat.size(), CV_8UC1);
    for(int i=0;i<multiMat.rows;i++)
    {
        for(int j=0;j<multiMat.cols;j++)
        {
            if(multiMat.at<uchar>(i, j)<=25)
                flagMat.at<uchar>(i, j)=255;
        }
    }
    vector<Point>().swap(crossVec);
    //debugShowMat(flagMat);
    debugSaveMat(flagMat, "outimage/flagMat.jpg");
}
/*
 * 函数功能：
 * 返回标志矩阵
 */
Mat CTSkeleton::getFlagMat()
{
    return flagMat;
}
/*
 * 函数功能：
 * 此时只能找三个点，右上方的交叉点没有。。。。
 * 改变函数，先找十字点，没有十字点的话找非十字点
 */
bool CTSkeleton::doFindCrossPoints(Point startPoint)
{
    Point tempPoint = startPoint;
    int count=0;

    while(1)
    {
        int countPath = 0;

        count++;
        /* 是否到端点 */
        if(getNeighbourPointNum(tempPoint) == 0)
        {
            break;      //先保留端点值
        }
        else
        {
            if(flagMat.at<uchar>(tempPoint)!=128)
                flagMat.at<uchar>(tempPoint) = 255;
        }
        /* 是否只有一条路 */
        if(getNeighbourPointNum(tempPoint) == 1)
        {
            tempPoint = getNeighbourPoints(tempPoint)[0]; //更新tempPoint
            continue;
        }
        /* 标记周围超过两个点的灰度值为128 */
        vector<Point> tempVecPoint = getNeighbourPoints(tempPoint);
        for(auto i:tempVecPoint)
        {
            flagMat.at<uchar>(i) = (uchar)128;         //周围超过两点的，周围点标记为128
        }
        /* 十字方向上是否有路 */
        tempVecPoint = getCrossNeighbourGreyPoints(tempPoint);
        if(!tempVecPoint.empty())
        {
            for(auto i:tempVecPoint)
            {
                if(doFindCrossPoints(i))
                    countPath++;
            }
            for(auto i:tempVecPoint)
            {
                uchar test = flagMat.at<uchar>(i);         //周围超过两点的，周围点标记为128
                test++;
            }
        }
        /* 非十字方向上是否有路 */
        tempVecPoint = getRCrossNeighbourGreyPoints(tempPoint);
        if(!tempVecPoint.empty())
        {
            for(auto i:tempVecPoint)
            {
                if(doFindCrossPoints(i))
                    countPath++;
                for(auto i:tempVecPoint)
                {
                    uchar test = flagMat.at<uchar>(i);         //周围超过两点的，周围点标记为128
                    test++;
                }
            }
        }
        /* 遇到分叉路，结束循环 */
        if(countPath>1)
            crossVec.push_back(tempPoint);
        break;
    }

    if(count>1)     //表明是一条路，而非单个点
        return true;
    else
        return false;
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
