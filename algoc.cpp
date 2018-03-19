#include "algoc.h"

/*
 * 函数功能：
 * 测试ROI区域
 */
void testClass(Mat srcMat)
{

#if 0
    CTAlpha *alpha = new CTAlpha;
    Mat alphaMat = alpha->getAlphaPic(srcMat);
    CTRotate *rotate = new CTRotate;
    Mat rotateMat = rotate->getRotateMat(alphaMat, CTRotate::DEGREE180);
    Mat rotateMat1 = rotate->getRotateMat(alphaMat, CTRotate::DEGREE0);
    Mat mirrorMat1 = rotate->mirrorMat(rotateMat, CTRotate::MIRRORX);
    Mat mirrorMat2 = rotate->mirrorMat(rotateMat, CTRotate::MIRRORY);
    Mat mirrorMat3 = rotate->mirrorMat(rotateMat1, CTRotate::MIRRORX);
    Mat mirrorMat4 = rotate->mirrorMat(rotateMat1, CTRotate::MIRRORY);

    Mat matQua(Size(rotateMat.cols*3,rotateMat.rows*5),CV_8UC4,cv::Scalar(0,0,0,0));            //此处用到CV_8UC4通道，故输入的原图必须是4四通道Mat才行
    matQua = rotate->panningMat(rotateMat,matQua,0,0);
    matQua = rotate->panningMat(rotateMat1,matQua,0, rotateMat.rows);
    matQua = rotate->panningMat(rotateMat,matQua,0, rotateMat.rows*2);
    matQua = rotate->panningMat(rotateMat1,matQua,0, rotateMat.rows*3);
    matQua = rotate->panningMat(rotateMat,matQua,0, rotateMat.rows*4);

    matQua = rotate->panningMat(mirrorMat1,matQua,rotateMat.cols*2, 0);
    matQua = rotate->panningMat(mirrorMat2,matQua,rotateMat.cols*2, rotateMat.rows);
    matQua = rotate->panningMat(mirrorMat3,matQua,rotateMat.cols*2, rotateMat.rows*2);
    matQua = rotate->panningMat(mirrorMat4,matQua,rotateMat.cols*2, rotateMat.rows*3);
    matQua = rotate->panningMat(rotateMat,matQua,rotateMat.cols*2, rotateMat.rows*4);
    debugShowMat(matQua);
    debugSaveMat(matQua);
#else
    CTAlpha *alpha = new CTAlpha;
    Mat alphaMat = alpha->getAlphaPic(srcMat);
    //debugShowMat(alphaMat);
    debugSaveMat(alphaMat,"alphaMat.png");
    debugSaveMat(alphaMat,"alphaMat.jpg");
    CTRotate *rotate = new CTRotate;
    Mat rotateMat = rotate->getRotateMat(alphaMat, CTRotate::DEGREE180);
    Mat rotateMat1 = rotate->getRotateMat(alphaMat, CTRotate::DEGREE0);
    Mat mirrorMat1 = rotate->mirrorMat(rotateMat, CTRotate::MIRRORX);
    Mat mirrorMat2 = rotate->mirrorMat(rotateMat, CTRotate::MIRRORY);
    Mat mirrorMat3 = rotate->mirrorMat(rotateMat1, CTRotate::MIRRORX);
    Mat mirrorMat4 = rotate->mirrorMat(rotateMat1, CTRotate::MIRRORY);

    Mat matQua(Size(rotateMat.cols*3,rotateMat.rows*5),CV_8UC4,cv::Scalar(255,255,255,0));            //此处用到CV_8UC4通道，故输入的原图必须是4四通道Mat才行
    matQua = rotate->panningMat(rotateMat,matQua,0,0);
    matQua = rotate->panningMat(rotateMat1,matQua,0, rotateMat.rows);
    matQua = rotate->panningMat(rotateMat,matQua,0, rotateMat.rows*2);
    matQua = rotate->panningMat(rotateMat1,matQua,0, rotateMat.rows*3);
    matQua = rotate->panningMat(rotateMat,matQua,0, rotateMat.rows*4);

    matQua = rotate->panningMat(mirrorMat1,matQua,rotateMat.cols*2, 0);
    matQua = rotate->panningMat(mirrorMat2,matQua,rotateMat.cols*2, rotateMat.rows);
    matQua = rotate->panningMat(mirrorMat3,matQua,rotateMat.cols*2, rotateMat.rows*2);
    matQua = rotate->panningMat(mirrorMat4,matQua,rotateMat.cols*2, rotateMat.rows*3);
    matQua = rotate->panningMat(rotateMat,matQua,rotateMat.cols*2, rotateMat.rows*4);
    debugShowMat(matQua);
    debugSaveMat(matQua);
#endif
}
/*
 * 函数功能：
 * 显示图片，名称为strName
 */
void debugShowMat(Mat showMat,string strName)
{
    imshow(strName,showMat);
    waitKey();
}
/*
 * 函数功能：
 * 保存图片
 */
void debugSaveMat(Mat saveMat, string saveName)
{
    imwrite(saveName,saveMat);
}

/*
 * 函数功能：
 * 执行边缘腐蚀操作
 */
void CTShrink::startEdgeShrink(Mat srcMat)
{
    Mat rstMat = CTAlpha::imageBinarizationThreshold(srcMat);
    for (int i=0;i<40;i++)
    {
        imshow("result", rstMat);
        waitKey();
        Mat tempMat;
        tempMat = edgeShrink(edgeGenerate(backgroundMark(rstMat), foregroundMark(rstMat)));
        rstMat += tempMat;
        debugShowMat(rstMat);
        cout <<"key"<<endl;
    }
}
/*
 * 函数功能：
 * 生成背景标签
 * 种子进行了膨胀操作
 * 背景灰度值为黑色(0)，种子灰度值为灰色(128)
 */
Mat CTShrink::backgroundMark(Mat srcMat)
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
Mat CTShrink::foregroundMark(Mat srcMat)
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
Mat CTShrink::watershedProc(Mat mymat, Mat srcMat)
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
Mat CTShrink::edgeShrink(Mat srcMat)
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
Mat CTShrink::edgeGenerate(Mat back, Mat fore)
{
    return (back+fore);
}
/*
 * 函数功能：
 * 去除图片边缘空隙
 * 输入：正方形白色透明背景黑色种子图片
 */
Mat CTRotate::removeEdge(Mat srcMat)
{
//    Mat greyMat = CTAlpha::imageBinarizationThreshold(srcMat);         //二值化图像
    Mat greyMat = CTAlpha::imageBinarizationBorW(srcMat);
    debugShowMat(greyMat);
    vector<Point> vecPoint = CTContour::findImageContours(greyMat)[0];   //寻找边缘点集
//    greyMat = CTContour::vecPointToMat(vecPoint,0,0,255,0);              //边缘点集变成Mat
    Mat tempMat = CTContour::vecPointToMat(srcMat, vecPoint,0);
//    debugShowMat(tempMat);
    return tempMat;
}
/*
 * 函数功能：
 * 寻找图像中的边缘点集
 */
vector<vector<Point> > CTContour::findImageContours(Mat greyMat)
{
    vector<vector<Point> > tempContours;
    vector<Vec4i> heichy;
    findContours(greyMat, tempContours, heichy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    return tempContours;
}
/*
 * 函数功能：
 * 将特定的点集变为Mat，背景为白色，点集内为指定色
 * 此函数生成三通道Mat
 */
Mat CTContour::vecPointToMat(vector<Point> vecPoint, int red, int green, int blue)
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
 * 将特定的点集变为Mat，背景为黑色透明色(由alpha参数决定，0是全透明，255是不透明的)
 * 此函数生成四通道Mat
 */
Mat CTContour::vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha)
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
            if (pointPolygonTest(vecPoint, pt, false) >= 0)         //必须是大于等于，等于时会把边框也画上
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
 * 将特定的点集变为Mat，种子为原图颜色
 * alpha指背景透明度，0是全透明
 * 背景默认色为白色
 */
Mat CTContour::vecPointToMat(Mat srcMat, vector<Point> vecPoint, int alpha)
{
    Rect rect = cv::boundingRect(Mat(vecPoint));

    Mat tempMat = Mat(rect.size(), CV_8UC4, Scalar(255, 255, 255,255));
    for (int row = 0; row < tempMat.rows; row++)
    {
        for (int col = 0; col < tempMat.cols; col++)
        {
            Point pt;
            pt.x = rect.x + col;
            pt.y = rect.y + row;
            if (pointPolygonTest(vecPoint, pt, false) >= 0)         //必须是大于等于，等于时会把边框也画上
            {
                if(srcMat.channels() == 3)
                {
                    tempMat.at<Vec4b>(row, col)[0] = srcMat.at<Vec3b>(pt.y, pt.x)[2];
                    tempMat.at<Vec4b>(row, col)[1] = srcMat.at<Vec3b>(pt.y, pt.x)[1];
                    tempMat.at<Vec4b>(row, col)[2] = srcMat.at<Vec3b>(pt.y, pt.x)[0];
                }
                else
                {
                    tempMat.at<Vec4b>(row, col)[0] = srcMat.at<Vec4b>(pt.y, pt.x)[2];
                    tempMat.at<Vec4b>(row, col)[1] = srcMat.at<Vec4b>(pt.y, pt.x)[1];
                    tempMat.at<Vec4b>(row, col)[2] = srcMat.at<Vec4b>(pt.y, pt.x)[0];
                }
                tempMat.at<Vec4b>(row, col)[3] = 255;
            }
            else
            {
                tempMat.at<Vec4b>(row, col)[0] = 255;
                tempMat.at<Vec4b>(row, col)[1] = 255;
                tempMat.at<Vec4b>(row, col)[2] = 255;
                tempMat.at<Vec4b>(row, col)[3] = alpha;
            }
        }
    }

    return tempMat;
}
/*
 * 函数功能：
 * 将特定的点集变为Mat，种子为原图颜色，有空隙
 */
Mat CTContour::vecPointToSpaceMat(Mat srcMat, vector<Point> vecPoint, int alpha)
{
    Rect rect = cv::boundingRect(Mat(vecPoint));
    rect.x -= 5;
    rect.y -= 5;
    rect.width += 10;
    rect.height += 10;

    Mat tempMat = Mat(rect.size(), CV_8UC4, Scalar(255, 255, 255,0));
    for (int row = 5; row < tempMat.rows-5; row++)
    {
        for (int col = 5; col < tempMat.cols-5; col++)
        {
            Point pt;
            pt.x = rect.x + col;
            pt.y = rect.y + row;
            if (pointPolygonTest(vecPoint, pt, false) >= 0)         //必须是大于等于，等于时会把边框也画上
            {
                tempMat.at<Vec4b>(row, col)[0] = srcMat.at<Vec3b>(pt.y, pt.x)[2];
                tempMat.at<Vec4b>(row, col)[1] = srcMat.at<Vec3b>(pt.y, pt.x)[1];
                tempMat.at<Vec4b>(row, col)[2] = srcMat.at<Vec3b>(pt.y, pt.x)[0];
                tempMat.at<Vec4b>(row, col)[3] = 255;
            }
            else
            {
                tempMat.at<Vec4b>(row, col)[3] = alpha;
            }
        }
    }

    return tempMat;
}
/*
 * 函数功能：
 * 执行透明化操作
 * 输入点集
 */
Mat CTAlpha::getAlphaPic(vector<Point> vecPoint)
{
    return CTContour::vecPointToMat(vecPoint,255,0,0,0);
}
/*
 * 函数功能：
 * 执行透明化操作
 * 已经去除了边隙
 * 输入Mat
 * 输出原种子，同时还有透明背景（默认是白色）
 */
Mat CTAlpha::getAlphaPic(Mat srcMat)
{
    vector<vector<Point> > vecVecPoint;
    Mat greyMat = imageBinarizationBorW(srcMat);
    vecVecPoint = CTContour::findImageContours(greyMat);

    //return CTContour::vecPointToMat(vecVecPoint[0],255,255,255,0);
    return CTContour::vecPointToSpaceMat(srcMat, vecVecPoint[0],0);
}
/*
 * 函数功能：
 * 图像二值化
 * 输入必须是灰度图
 * 灰度值大于SEEDGRAYVALUE的为白色，小于SEEDGRAYVALUE的为黑色
 * 种子是黑色，背景是白色
 */
#define SEEDGRAYVALUE 45                //定义灰度值阈值
Mat CTAlpha::imageBinarizationThreshold(Mat srcMat)
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
Mat CTAlpha::imageBinarizationBorW(Mat srcMat)
{
    Mat tempMat;
    tempMat = imageBinarizationThreshold(srcMat);
    threshold(tempMat, tempMat, 0, 255, CV_THRESH_BINARY_INV);
    return tempMat;
}
/*
 * 函数功能：
 * 执行图像旋转
 * warpAffine()函数中的角度信息是让种子逆时针旋转，比如设置60，意思是逆时针旋转60度
 * 注意，之前的问题没有解决，即如果不是长宽相等的图片，结果会出错，要么少了头，要么少了尾
 * 现在的解决思路是使原图长宽相等
 */
Mat CTRotate::getRotateMat(Mat srcMat, float degree)
{
    degree += getRotateMatDegree(srcMat);                                       //默认值使种子竖直方向垂直
    srcMat = quadrateMat(srcMat);                                               //使原图长宽相等
    debugSaveMat(srcMat,"quaMat.png");

    double angle = degree * CV_PI / 180.;                                       //计算弧度
    double dsin = sin(angle), dcos = cos(angle);                                //计算正余弦
    int width = srcMat.rows;                                                    //原图宽
    int height = srcMat.cols;                                                   //原图高
    int width_rotate= int(height * fabs(dsin) + width * fabs(dcos));            //旋转后图像的宽度
    int height_rotate=int(width * fabs(dsin) + height * fabs(dcos));            //旋转后图像的高度

    Mat matUpRight(Size(width_rotate,height_rotate),CV_8UC4,Scalar(255,255,255,0));                   //旋转后的图像
    Mat rMat = getRotationMatrix2D(Point2f(width/2,height/2),degree,1);         //得到旋转矩阵
    rMat.at<double>(0,2) += (width_rotate - width) / 2;                         //水平方向平移量
    rMat.at<double>(1,2) += (height_rotate - height) / 2;                       //竖直方向平移量
    warpAffine(srcMat,matUpRight,rMat,Size(matUpRight.rows,matUpRight.cols),
               INTER_LINEAR,BORDER_REPLICATE);                                  //进行旋转，最后一个参数是边缘处理方式，此处采用的是边缘复制

    //debugShowMat(matUpRight);
    debugSaveMat(matUpRight,"matUpRight.jpg");
    debugSaveMat(matUpRight,"matUpRight.png");

    return removeEdge(matUpRight);
}
/*
 * 函数功能：
 * 使种子图片变成正方形
 */
Mat CTRotate::quadrateMat(Mat srcMat)
{
    int quaLenOfSide = (srcMat.rows>srcMat.cols?srcMat.rows:srcMat.cols);

    Mat matQua(Size(quaLenOfSide,quaLenOfSide),CV_8UC4,Scalar(255,255,255,0));                                        //旋转后的图像
    panningMat(srcMat,matQua,(quaLenOfSide-srcMat.cols)/2.0,(quaLenOfSide-srcMat.rows)/2.0);    //图像平移

    return matQua;
}
/*
 * 函数功能：
 * 移动源图片到目标图片指定位置
 */
Mat CTRotate::panningMat(Mat srcMat, Mat &dstMat, float x, float y)
{
    Mat dstRoi = dstMat(Rect(x,y, srcMat.cols, srcMat.rows));
    srcMat.copyTo(dstRoi);

    return dstMat;
}
/*
 * 函数功能：
 * 镜像变换
 */
Mat CTRotate::mirrorMat(Mat srcMat, int type)
{
    Mat tempMat;
    flip(srcMat, tempMat, type);
    return tempMat;
}
/*
 * 函数功能：
 * 得到使种子Mat竖直向垂直的角度
 * 输入：透明白色背景的黑色种子图片
 */
float CTRotate::getRotateMatDegree(Mat srcMat)
{
    vector<vector<Point> > vecVecPoint;
//    Mat greyMat = CTAlpha::imageBinarizationThreshold(srcMat);
    Mat greyMat = CTAlpha::imageBinarizationBorW(srcMat);
    vecVecPoint = CTContour::findImageContours(greyMat);
    RotatedRect rRect = minAreaRect(vecVecPoint[0]);                            //得到最小矩形框

    return getRotateUprightDegree(rRect);
}
/*
 * 函数功能：
 * 得到使旋转矩形竖直向垂直的角度
 */
float CTRotate::getRotateUprightDegree(RotatedRect calculatedRect)
{
    cout << "calculatedRect width:"<<calculatedRect.size.width<<"  calculatedRect height:"<<calculatedRect.size.height<<endl;
    cout << "calculatedRect angle:"<<calculatedRect.angle<<endl;
    if(calculatedRect.size.width < calculatedRect.size.height)
    {
        cout << "getRotateRectDegree:"<<calculatedRect.angle<<endl;
        return (calculatedRect.angle);          //负值：顺时针移动
    }else
    {
        cout << "getRotateRectDegree:"<<90+calculatedRect.angle<<endl;
        return (90+calculatedRect.angle);       //正值：逆时针移动
    }
}
