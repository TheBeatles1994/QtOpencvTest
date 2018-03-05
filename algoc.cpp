#include "algoc.h"

CvTest::CvTest()
{
}

CvTest::CvTest(Mat mymat)
{
    srcImage = mymat;
}
/*
 * 函数功能：
 * 执行边缘腐蚀操作
 */
void CvTest::startEdgeShrink(Mat srcMat)
{
    rstImage = imageBinarizationThreshold(srcMat);
    for (int i=0;i<40;i++)
    {
        imshow("result", rstImage);
        waitKey();
        Mat tempMat;
        tempMat = edgeShrink(edgeGenerate(backgroundMark(rstImage), foregroundMark(rstImage)));
        rstImage += tempMat;
        cout <<"key"<<endl;
    }
}
/*
 * 函数功能：
 * 执行透明化操作
 * 输入点集
 */
Mat CvTest::startAlphaPic(vector<Point> vecPoint)
{
    return vecPointToMat(vecPoint,255,0,0,0);
}
/*
 * 函数功能：
 * 执行透明化操作
 * 输入Mat
 */
Mat CvTest::startAlphaPic(Mat srcMat)
{
    vector<vector<Point> > vecVecPoint;
    Mat greyMat = imageBinarizationBorW(srcMat);
    vecVecPoint = findImageContours(greyMat);

    return vecPointToMat(vecVecPoint[0],255,255,255,0);
}
/*
 * 函数功能：
 * 执行图像旋转
 */
Mat CvTest::startRotatePic(Mat srcMat, int degree)
{
    double angle = degree * CV_PI / 180.;                                       //计算弧度
    double dsin = sin(angle), dcos = cos(angle);                                //计算正余弦
    int width = srcMat.rows;                                                    //原图宽
    int height = srcMat.cols;                                                   //原图高
    int width_rotate= int(height * fabs(dsin) + width * fabs(dcos));            //旋转后图像的宽度
    int height_rotate=int(width * fabs(dsin) + height * fabs(dcos));            //旋转后图像的高度
    Mat matUpRight(Size(width_rotate,height_rotate),CV_8UC4);                   //旋转后的图像

    Mat rMat = getRotationMatrix2D(Point2f(width/2,height/2),degree,1);         //得到旋转矩阵
    rMat.at<double>(0,2) += (width_rotate - width) / 2;                         //水平方向平移量
    rMat.at<double>(1,2) += (height_rotate - height) / 2;                       //竖直方向平移量

    warpAffine(srcMat,matUpRight,rMat,Size(matUpRight.rows,matUpRight.cols),
               INTER_LINEAR,BORDER_REPLICATE);                                  //进行旋转，最后一个参数是边缘处理方式，此处采用的是边缘复制
    /* 去图片边缘空隙 */
    Mat matUpRightNoSpace = imageBinarizationThreshold(matUpRight);
    vector<Point> vecPoint = findImageContours(matUpRightNoSpace)[0];
    matUpRightNoSpace = vecPointToMat(vecPoint,0,0,255,0);
    debugShowMat(matUpRightNoSpace);

    return matUpRightNoSpace;
}
/*
 * 函数功能：
 * 得到种子旋转角度
 */
float CvTest::getRotateDegree(Mat srcMat)
{
    vector<vector<Point> > vecVecPoint;
    Mat greyMat = imageBinarizationBorW(srcMat);
    vecVecPoint = findImageContours(greyMat);

    RotatedRect rRect = minAreaRect(vecVecPoint[0]);                            //得到最小矩形框
    return rRect.angle;
}
/*
 * 函数功能：
 * 生成背景标签
 * 种子进行了膨胀操作
 * 背景灰度值为黑色(0)，种子灰度值为灰色(128)
 */
Mat CvTest::backgroundMark(Mat srcMat)
{
    Mat backgroundMat;
#if 1
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(srcMat, backgroundMat, element);
    threshold(backgroundMat, backgroundMat, 1, 128, CV_THRESH_BINARY_INV);
#else
    threshold(mymat, backgroundMat, 1, 128, CV_THRESH_BINARY_INV);
#endif

    return backgroundMat;
}
/*
 * 函数功能：
 * 生成前景标签
 * 种子进行了腐蚀操作
 * 背景灰度值为白色(255)，种子灰度值为黑色(0)
 */
Mat CvTest::foregroundMark(Mat srcMat)
{
    Mat foregroundMat;
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    erode(srcMat, foregroundMat, element);
    return foregroundMat;
}
/*
 * 函数功能：
 * 分水岭算法
 * 该函数未深究（不知道返回的wsh是什么样子）
 */
Mat CvTest::watershedProc(Mat mymat, Mat srcMat)
{
    WatershedSegmenter segmenter;
    segmenter.setMarkers(mymat);
    Mat mak = segmenter.process(srcMat);

    //描绘种子边缘
    Mat wsh(mak.size(), 0);
    for (int i = 0; i < mak.rows; i++)
    {
        for (int j = 0; j < mak.cols; j++)
        {
            if (i == 0 || j == 0 || i == mak.rows - 1 || j == mak.cols - 1)
                wsh.at<uchar>(i, j) = 0;
            else
            {
                int index = mak.at<int>(i, j);
                if (index == -1)
                    wsh.at<uchar>(i, j) = 255;
                else
                    wsh.at<uchar>(i, j) = 0;
            }
        }
    }

    return wsh;
}
/*
 * 函数功能：
 * 边缘腐蚀
 * 将种子变成白色，背景变成黑色
 * 种子已经缩小了一圈
 */
Mat CvTest::edgeShrink(Mat srcMat)
{
    Mat tempMat(srcMat.size(),0);

    for (int i=0;i<tempMat.rows;i++)
    {
        for (int j=0;j<tempMat.cols;j++)
        {
            if (srcMat.at<uchar>(i,j) == 0)
            {
                tempMat.at<uchar>(i,j) = 255;
            }
            else
            {
                tempMat.at<uchar>(i,j) = 0;
            }
        }
    }

    return tempMat;
}
/*
 * 函数功能：
 * 边缘生成
 * 通过背景标签和前景标签得到
 * 背景白色(255)，种子边缘灰色(128)，种子内部黑色(0)
 */
Mat CvTest::edgeGenerate(Mat back, Mat fore)
{
    return (back+fore);
}
/*
 * 函数功能：
 * 图像二值化
 * 输入必须是灰度图
 * 灰度值大于SEEDGRAYVALUE的为白色，小于SEEDGRAYVALUE的为黑色
 * 种子是黑色，背景是白色
 */
#define SEEDGRAYVALUE 45                //定义灰度值阈值
Mat CvTest::imageBinarizationThreshold(Mat srcMat)
{
    Mat tempMat;
    cvtColor(srcMat,tempMat,CV_BGR2GRAY);
    threshold(tempMat, tempMat, SEEDGRAYVALUE, 255, CV_THRESH_BINARY);
    return tempMat;
}
/*
 * 函数功能：
 * 图像二值化
 * 输入必须是灰度图
 * 非黑变黑，黑变白
 * 种子是白色，背景是黑色
 * 这样才能找到边缘点集，若用imageBinarization()函数则不行
 */
Mat CvTest::imageBinarizationBorW(Mat srcMat)
{
    Mat tempMat;
    tempMat = imageBinarizationThreshold(srcMat);
    threshold(tempMat, tempMat, 0, 255, CV_THRESH_BINARY_INV);
    return tempMat;
}
/*
 * 函数功能：
 * 将特定的点集变为Mat，背景为白色，点集内为指定色
 */
Mat CvTest::vecPointToMat(vector<Point> vecPoint, int red, int green, int blue)
{
    Rect rect = cv::boundingRect(Mat(vecPoint));

    Mat tempMat = Mat(rect.size(), CV_8UC3, Scalar(255, 255, 255));
    for (int row = 0; row < tempMat.rows; row++)
    {
        for (int col = 0; col < tempMat.cols; col++)
        {
            Point pt;
            pt.x = rect.x + col;
            pt.y = rect.y + row;
            if (pointPolygonTest(vecPoint, pt, false) >= 0)
            {
                tempMat.at<Vec4b>(row, col)[0] = blue;
                tempMat.at<Vec4b>(row, col)[1] = green;
                tempMat.at<Vec4b>(row, col)[2] = red;
            }
        }
    }

    return tempMat;
}
/*
 * 函数功能：
 * 将特定的点集变为Mat，背景为透明色(由alpha参数决定，0是全透明，255是不透明的)
 */
Mat CvTest::vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha)
{
    Rect rect = cv::boundingRect(Mat(vecPoint));
//    rect.x -= 5;
//    rect.y -= 5;
//    rect.width += 10;
//    rect.height += 10;

    Mat tempMat = Mat(rect.size(), CV_8UC4, Scalar(255, 255, 255,255));
    for (int row = 0; row < tempMat.rows; row++)
    {
        for (int col = 0; col < tempMat.cols; col++)
        {
            Point pt;
            pt.x = rect.x + col;
            pt.y = rect.y + row;
            if (pointPolygonTest(vecPoint, pt, false) > 0)
            {
                tempMat.at<Vec4b>(row, col)[0] = blue;
                tempMat.at<Vec4b>(row, col)[1] = green;
                tempMat.at<Vec4b>(row, col)[2] = red;
                tempMat.at<Vec4b>(row, col)[3] = 255;
            }
            else
            {
                tempMat.at<Vec4b>(row, col)[0] = 0;
                tempMat.at<Vec4b>(row, col)[1] = 0;
                tempMat.at<Vec4b>(row, col)[2] = 0;
                tempMat.at<Vec4b>(row, col)[3] = alpha;
            }
        }
    }

    return tempMat;
}
/*
 * 函数功能：
 * 寻找图像中的边缘点集
 */
vector<vector<Point> > CvTest::findImageContours(Mat greyMat)
{
    vector<vector<Point> > tempContours;
    vector<Vec4i> heichy;
    findContours(greyMat, tempContours, heichy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    return tempContours;
}
/*
 * 函数功能：
 * 显示图片
 */
void CvTest::debugShowMat(Mat showMat)
{
    imshow("debug",showMat);
    waitKey();
}

