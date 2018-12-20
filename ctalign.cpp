#include "ctalign.h"

/* 设置背景是否要透明 */
// 在函数removeEdge和vecPointToMat处有使用
#define ISALPHA false
/* 设置背景是否要为白色 */
#define ISBLACK true

/*
 * 函数功能：
 * 测试并演示新函数
 * 获取紧密排列 函数名称变为alignMat()
 */
void testAlign()
{
#if 1
    Mat srcMat1 = imread("D://135.png");
    Mat srcMat2 = imread("D://20.png");
    Mat srcMat3 = imread("D://30.png");

    shared_ptr<CTAlpha> alpha = make_shared<CTAlpha>();
    Mat alphaMat1 = alpha->getAlphaPic(srcMat1);
    Mat alphaMat2 = alpha->getAlphaPic(srcMat2);
    Mat alphaMat3 = alpha->getAlphaPic(srcMat3);
    vector<Mat> vec1,vec2;
    vector<vector<Mat> > vec;
    shared_ptr<CTRotate> rotate = make_shared<CTRotate>();

    Mat rotateMat1 = rotate->getRotateMat(alphaMat1, CTRotate::DEGREE180);
    Mat rotateMat2 = rotate->getRotateMat(alphaMat1, CTRotate::DEGREE0);

    Mat rotateMat3 = rotate->getRotateMat(alphaMat2, CTRotate::DEGREE180);
    Mat rotateMat4 = rotate->getRotateMat(alphaMat2, CTRotate::DEGREE0);

    Mat rotateMat5 = rotate->getRotateMat(alphaMat3, CTRotate::DEGREE180);
    Mat rotateMat6 = rotate->getRotateMat(alphaMat3, CTRotate::DEGREE0);

    Mat mirrorMat1 = rotate->getMirrorMat(rotateMat1, CTRotate::MIRRORX);
    Mat mirrorMat2 = rotate->getMirrorMat(rotateMat2, CTRotate::MIRRORY);
    Mat mirrorMat3 = rotate->getMirrorMat(rotateMat3, CTRotate::MIRRORX);
    Mat mirrorMat4 = rotate->getMirrorMat(rotateMat4, CTRotate::MIRRORY);
    Mat mirrorMat5 = rotate->getMirrorMat(rotateMat5, CTRotate::MIRRORX);
    Mat mirrorMat6 = rotate->getMirrorMat(rotateMat6, CTRotate::MIRRORY);

    vec1.push_back(rotateMat3);
    vec1.push_back(rotateMat5);
    vec1.push_back(mirrorMat3);
    vec1.push_back(mirrorMat6);
    vec1.push_back(mirrorMat4);

    vec2.push_back(mirrorMat5);
    vec2.push_back(mirrorMat4);
    vec2.push_back(rotateMat1);
    vec2.push_back(mirrorMat4);
    vec2.push_back(rotateMat2);

    vec.push_back(vec1);
    vec.push_back(vec2);

    shared_ptr<CTAlign> align = make_shared<CTAlign>(vec);
    Mat matQua = align->alignMat(CTAlign::VERTICAL,CTAlign::MID,20);
    vector<Point> vPoint = align->getAlignPoint(1,3);
    cout << vPoint[0]<<vPoint[1]<<endl;
    debugShowMat(matQua,"test1.png");
    debugSaveMat(matQua,"test1.png");
    align->setAlignMat(mirrorMat3,0,0);
    matQua = align->alignMat(CTAlign::VERTICAL,CTAlign::MID,20);
    debugShowMat(matQua,"test2.png");
    debugSaveMat(matQua,"test2.png");
#else
    Mat srcMat1 = imread("D://135.png");

    shared_ptr<CTAlpha> alpha = make_shared<CTAlpha>();
    Mat alphaMat = alpha->getAlphaPic(srcMat1);
    shared_ptr<CTRotate> rotate = make_shared<CTRotate>();

    Mat rotateMat = rotate->getRotateMat(alphaMat, CTRotate::DEGREE180);
    cout << "ori:"<<rotateMat.cols<<" "<<rotateMat.rows<<endl;
    while(1)
    {
        rotateMat = rotate->getRotateMat(rotateMat, CTRotate::DEGREE180);
        cout << "ori:"<<rotateMat.cols<<" "<<rotateMat.rows<<endl;
        debugShowMat(rotateMat);
        debugSaveMat(rotateMat);
    }
#endif

    return;
}
/*
 * 函数功能：
 * 测试边缘腐蚀
 */
void testShrink()
{
    CTShrink shrink;
    Mat srcMat = imread("doc/10.png");

    shrink.startEdgeShrink(srcMat);
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
    debugShowMat(rstMat);
    debugSaveMat(rstMat, "outimage/shrink.jpg");
    for (int i=0;i<40;i++)
    {
        imshow("result", rstMat);
        Mat tempMat;
        tempMat = edgeShrink(edgeGenerate(backgroundMark(rstMat), foregroundMark(rstMat)));
        rstMat += tempMat;
        debugShowMat(rstMat);
        debugSaveMat(rstMat, "outimage/shrink.jpg");
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
Mat CTRotate::removeEdge(Mat srcMat, int alpha)
{
    assert(srcMat.channels() ==3 || srcMat.channels()==4);

    //Mat greyMat = CTAlpha::imageBinarizationBorW(srcMat);
    Mat greyMat;
    cvtColor(srcMat, greyMat, CV_BGR2GRAY);
    threshold(greyMat, greyMat, 4, 255, CV_THRESH_BINARY);
    //debugShowMat(greyMat);
    vector<Point> vecPoint = CTContour::findImageContours(greyMat)[0];   //寻找边缘点集
    Mat tempMat;
    tempMat = CTContour::vecPointToMat(srcMat, vecPoint,Scalar(0,0,0), alpha,5);
    return tempMat;
}
/* ===================================================================
 * @函数功能: 去除图片边缘空隙
 * @输入参数: 原图、对应的label图、alpha通道
 * @输出参数: 去除边缘空隙的原图和label图
 * @注意事项:
 *      无
   ===================================================================
 */
pair<Mat, Mat> CTRotate::removeEdge(Mat srcMat, Mat labelMat, int alpha)
{
    assert(srcMat.channels() ==3 || srcMat.channels()==4);

    Mat greyMat;
    cvtColor(srcMat, greyMat, CV_BGR2GRAY);
    threshold(greyMat, greyMat, 4, 255, CV_THRESH_BINARY);
    vector<Point> vecPoint = CTContour::findImageContours(greyMat)[0];   //寻找边缘点集
    Mat tempMat;
    tempMat = CTContour::vecPointToMat(srcMat, vecPoint,Scalar(0,0,0), alpha,5);

    Mat greyLabelMat;
    cvtColor(labelMat, greyLabelMat, CV_BGR2GRAY);
    threshold(greyLabelMat, greyLabelMat, 0, 255, CV_THRESH_BINARY);
    vector<Point> vecLabelPoint = CTContour::findImageContours(greyLabelMat)[0];   //寻找边缘点集
    Mat tempLabelMat;
    tempLabelMat = CTContour::vecPointToMat(labelMat, vecLabelPoint,Scalar(0,0,0), alpha,5);

    return pair<Mat, Mat>(tempMat, tempLabelMat);
}
/*
 * 函数功能：
 * 寻找图像中的边缘点集
 * 输入必须是灰度图，一通道
 */
vector<vector<Point> > CTContour::findImageContours(Mat greyMat)
{
    assert(greyMat.channels() == 1);
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
Mat CTContour::vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int edgeSpacing)
{
    Rect rect = cv::boundingRect(Mat(vecPoint));
    rect.x -= edgeSpacing;
    rect.y -= edgeSpacing;
    rect.width += edgeSpacing*2;
    rect.height += edgeSpacing*2;
    Mat tempMat = Mat(rect.size(), CV_8UC3, Scalar(0, 0, 0));
    for (int row = edgeSpacing; row < tempMat.rows-edgeSpacing; row++)
    {
        for (int col = edgeSpacing; col < tempMat.cols-edgeSpacing; col++)
        {
            Point pt;
            pt.x = rect.x + col;
            pt.y = rect.y + row;
            if (pointPolygonTest(vecPoint, pt, false) >= 0)
            {
                tempMat.at<Vec3b>(row, col)[0] = blue;
                tempMat.at<Vec3b>(row, col)[1] = green;
                tempMat.at<Vec3b>(row, col)[2] = red;
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
Mat CTContour::vecPointToMat(vector<Point> vecPoint, int red, int green, int blue, int alpha,int edgeSpacing)
{
    Rect rect = cv::boundingRect(Mat(vecPoint));
    rect.x -= edgeSpacing;
    rect.y -= edgeSpacing;
    rect.width += edgeSpacing*2;
    rect.height += edgeSpacing*2;
    Mat tempMat = Mat(rect.size(), CV_8UC4, Scalar(0, 0, 0,0));
    for (int row = edgeSpacing; row < tempMat.rows-edgeSpacing; row++)
    {
        for (int col = edgeSpacing; col < tempMat.cols-edgeSpacing; col++)
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
 * 无空隙
 */
Mat CTContour::vecPointToMat(Mat srcMat, vector<Point> vecPoint, Scalar backColor,int alpha,int edgeSpacing)
{
    assert(srcMat.channels() == 3||srcMat.channels() == 4);

    Rect rect = cv::boundingRect(Mat(vecPoint));
    rect.x -= edgeSpacing;
    rect.y -= edgeSpacing;
    rect.width += edgeSpacing*2;
    rect.height += edgeSpacing*2;

    Mat tempMat;
    tempMat = Mat(rect.size(), CV_8UC4, Scalar(0, 0, 0,alpha));

    for (int row = edgeSpacing; row < tempMat.rows-edgeSpacing; row++)
    {
        for (int col = edgeSpacing; col < tempMat.cols-edgeSpacing; col++)
        {
            Point pt;
            pt.x = rect.x + col;
            pt.y = rect.y + row;
            if (pointPolygonTest(vecPoint, pt, false) >= 0)         //必须是大于等于，等于时会把边框也画上
            {
                if(srcMat.channels() == 3)
                {
                    tempMat.at<Vec4b>(row, col)[0] = srcMat.at<Vec3b>(pt.y, pt.x)[0];
                    tempMat.at<Vec4b>(row, col)[1] = srcMat.at<Vec3b>(pt.y, pt.x)[1];
                    tempMat.at<Vec4b>(row, col)[2] = srcMat.at<Vec3b>(pt.y, pt.x)[2];
                }
                else
                {
                    tempMat.at<Vec4b>(row, col)[0] = srcMat.at<Vec4b>(pt.y, pt.x)[0];
                    tempMat.at<Vec4b>(row, col)[1] = srcMat.at<Vec4b>(pt.y, pt.x)[1];
                    tempMat.at<Vec4b>(row, col)[2] = srcMat.at<Vec4b>(pt.y, pt.x)[2];
                }
                tempMat.at<Vec4b>(row, col)[3] = 255;
            }
            else
            {
                tempMat.at<Vec4b>(row, col)[0] = backColor[0];
                tempMat.at<Vec4b>(row, col)[1] = backColor[1];
                tempMat.at<Vec4b>(row, col)[2] = backColor[2];
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
    return CTContour::vecPointToMat(vecPoint,255,0,0,0,0);
}
/*
 * 函数功能：
 * 执行透明化操作
 * 已经去除了边隙
 * 输入三通道或四通道Mat
 * 输出原种子，同时还有透明背景（默认背景是白色，种子是暗色）
 */
Mat CTAlpha::getAlphaPic(Mat srcMat, int alpha)
{
    vector<vector<Point> > vecVecPoint;
    Mat greyMat;
    cvtColor(srcMat, greyMat, CV_BGR2GRAY);
    threshold(greyMat, greyMat, 45, 255, CV_THRESH_BINARY);
    vecVecPoint = CTContour::findImageContours(greyMat);

    return CTContour::vecPointToMat(srcMat, vecVecPoint[0],Scalar(0,0,0), alpha,2);
}
/*
 * 函数功能：
 * 图像二值化
 * 输入必须是三通道或四通道图，输出是灰度图
 * 灰度值大于SEEDGRAYVALUE的为白色，小于SEEDGRAYVALUE的为黑色
 * 种子是黑色，背景是白色
 */
#define SEEDGRAYVALUE 45                //定义灰度值阈值
Mat CTAlpha::imageBinarizationThreshold(Mat srcMat)
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
Mat CTAlpha::imageBinarizationBorW(Mat srcMat)
{
    assert(srcMat.channels() == 3||srcMat.channels() == 4);
    Mat greyMat;
    greyMat = imageBinarizationThreshold(srcMat);
    threshold(greyMat, greyMat, 0, 255, CV_THRESH_BINARY_INV);
    return greyMat;
}
/*
 * 函数功能：
 * 执行图像旋转
 * warpAffine()函数中的角度信息是让种子逆时针旋转，比如设置60，意思是逆时针旋转60度
 * 注意，之前的问题没有解决，即如果不是长宽相等的图片，结果会出错，要么少了头，要么少了尾
 * 现在的解决思路是使原图长宽相等
 */
Mat CTRotate::getRotateMat(Mat srcMat, float degree, int alpha)
{
    assert(srcMat.channels() ==3 || srcMat.channels()==4);

    degree += getRotateMatDegree(srcMat);                                       //默认值使种子竖直方向垂直
    srcMat = quadrateMat(srcMat,4);                                               //使原图长宽相等

    double angle = degree * CV_PI / 180.;                                       //计算弧度
    double dsin = sin(angle), dcos = cos(angle);                                //计算正余弦
    int width = srcMat.rows;                                                    //原图宽
    int height = srcMat.cols;                                                   //原图高
    float width_rotate= height*1.0 * fabs(dsin) + width*1.0 * fabs(dcos);            //旋转后图像的宽度
    float height_rotate=width*1.0 * fabs(dsin) + height*1.0 * fabs(dcos);            //旋转后图像的高度

    Mat matUpRight(Size(width_rotate,height_rotate),CV_8UC4,Scalar(0,0,0,alpha));                   //旋转后的图像
    Mat rMat = getRotationMatrix2D(Point2f(width*1.0/2,height*1.0/2),degree,1);         //得到旋转矩阵
    rMat.at<double>(0,2) += (width_rotate - width) / 2;                         //水平方向平移量
    rMat.at<double>(1,2) += (height_rotate - height) / 2;                       //竖直方向平移量
    warpAffine(srcMat,matUpRight,rMat,Size(matUpRight.rows,matUpRight.cols),
               INTER_LINEAR,BORDER_REPLICATE);                                  //进行旋转，最后一个参数是边缘处理方式，此处采用的是边缘复制

    //debugShowMat(matUpRight);
    debugSaveMat(matUpRight,"matUpRight.jpg");
    debugSaveMat(matUpRight,"matUpRight.png");

    return removeEdge(matUpRight, alpha);
}
/* ===================================================================
 * @函数功能: 将种子转到相对degree角度
 * @输入参数: srcMat为原图，degree为逆时针转动的相对角度
 * @输出参数: 转动后的图
 * @注意事项:
 *      无
   ===================================================================
 */
Mat CTRotate::getRelativeRotateMat(Mat srcMat, float degree, int alpha)
{
    assert(srcMat.channels() ==3 || srcMat.channels()==4);

    srcMat = quadrateMat(srcMat,4);                                               //使原图长宽相等

    double angle = degree * CV_PI / 180.;                                       //计算弧度
    double dsin = sin(angle), dcos = cos(angle);                                //计算正余弦
    int width = srcMat.rows;                                                    //原图宽
    int height = srcMat.cols;                                                   //原图高
    float width_rotate= height*1.0 * fabs(dsin) + width*1.0 * fabs(dcos);            //旋转后图像的宽度
    float height_rotate=width*1.0 * fabs(dsin) + height*1.0 * fabs(dcos);            //旋转后图像的高度

    Mat matUpRight(Size(width_rotate,height_rotate),CV_8UC4,Scalar(0,0,0,alpha));                   //旋转后的图像
    Mat rMat = getRotationMatrix2D(Point2f(width*1.0/2,height*1.0/2),degree,1);         //得到旋转矩阵
    rMat.at<double>(0,2) += (width_rotate - width) / 2;                         //水平方向平移量
    rMat.at<double>(1,2) += (height_rotate - height) / 2;                       //竖直方向平移量
    warpAffine(srcMat,matUpRight,rMat,Size(matUpRight.rows,matUpRight.cols),
               INTER_LINEAR,BORDER_REPLICATE);                                  //进行旋转，最后一个参数是边缘处理方式，此处采用的是边缘复制

    //debugShowMat(matUpRight);

    return removeEdge(matUpRight, alpha);
}
/* ===================================================================
 * @函数功能: 将种子转到相对degree角度
 * @输入参数: srcMat为原图，degree为逆时针转动的相对角度
 * @输出参数: 转动后的原图和Label图
 * @注意事项:
 *      无
   ===================================================================
 */
pair<Mat, Mat> CTRotate::getRelativeRotateMatAndLabel(Mat srcMat, Mat labelMat, float degree, int alpha)
{
    assert(srcMat.channels() ==3 || srcMat.channels()==4);
    /* 使原图长宽相等 */
    srcMat = quadrateMat(srcMat,4);
    labelMat = quadrateMat(labelMat,4);

    double angle = degree * CV_PI / 180.;                                       //计算弧度
    double dsin = sin(angle), dcos = cos(angle);                                //计算正余弦
    int width = srcMat.rows;                                                    //原图宽
    int height = srcMat.cols;                                                   //原图高
    float width_rotate= height*1.0 * fabs(dsin) + width*1.0 * fabs(dcos);            //旋转后图像的宽度
    float height_rotate=width*1.0 * fabs(dsin) + height*1.0 * fabs(dcos);            //旋转后图像的高度

    Mat matUpRight(Size(width_rotate,height_rotate),CV_8UC4,Scalar(0,0,0,alpha));       //旋转后的原图像
    Mat matUpRightLabel(Size(width_rotate,height_rotate),CV_8UC4,Scalar(0,0,0,alpha));  //旋转后的label图像
    Mat rMat = getRotationMatrix2D(Point2f(width*1.0/2,height*1.0/2),degree,1);         //得到旋转矩阵
    rMat.at<double>(0,2) += (width_rotate - width) / 2;                         //水平方向平移量
    rMat.at<double>(1,2) += (height_rotate - height) / 2;                       //竖直方向平移量
    warpAffine(srcMat,matUpRight,rMat,Size(matUpRight.rows,matUpRight.cols),
               INTER_LINEAR,BORDER_REPLICATE);                                  //进行旋转，最后一个参数是边缘处理方式，此处采用的是边缘复制
    warpAffine(labelMat,matUpRightLabel,rMat,Size(matUpRightLabel.rows,matUpRightLabel.cols),
               INTER_LINEAR,BORDER_REPLICATE);                                  //进行旋转，最后一个参数是边缘处理方式，此处采用的是边缘复制

    return removeEdge(matUpRight, matUpRightLabel, alpha);
}
/* ===================================================================
 * @函数功能: 使种子图片变成正方形
 * @输入参数: 原图、边缘间隔
 * @输出参数: 处理后的正方形图
 * @注意事项:
 *      此处要特别注意三通道和四通道！
   ===================================================================
 */
Mat CTRotate::quadrateMat(Mat srcMat, int spacing)
{
    assert(srcMat.channels() ==3 || srcMat.channels()==4);
    int quaLenOfSide = (srcMat.rows>srcMat.cols?srcMat.rows:srcMat.cols)+spacing*2;                       //左右上下各添加2个像素

    Mat matQua;
    if(srcMat.channels() == 4)
        matQua = Mat(Size(quaLenOfSide,quaLenOfSide),CV_8UC4,Scalar(0,0,0,255));                      //旋转后的图像
    else
        matQua = Mat(Size(quaLenOfSide,quaLenOfSide),CV_8UC3,Scalar(0,0,0));                      //旋转后的图像
    panningMat(srcMat,matQua,(quaLenOfSide-srcMat.cols)/2.0+spacing,(quaLenOfSide-srcMat.rows)/2.0+spacing);    //图像平移

    return matQua;
}
/* ===================================================================
 * @函数功能: 移动原图片到目标图片的指定位置
 * @输入参数: 原图、目标图、目标图的指定位置
 * @输出参数: 拷贝好后的目标图
 * @注意事项:
 *      若原图和目标图通道不一致时，Mat类的copyTo()函数会出问题，即出来的图全黑
   ===================================================================
 */
Mat CTRotate::panningMat(Mat srcMat, Mat &dstMat, float x, float y)
{
    Mat dstRoi = dstMat(Rect(x,y, srcMat.cols, srcMat.rows));
    srcMat.copyTo(dstRoi);

    cout<<srcMat.channels()<<" "<<dstMat.channels()<<" "<<dstRoi.channels()<<endl;
    return dstMat;
}
/*
 * 函数功能：
 * 镜像变换
 */
Mat CTRotate::getMirrorMat(Mat srcMat, int type)
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
    assert(srcMat.channels() ==3 || srcMat.channels()==4);

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
    if(calculatedRect.size.width < calculatedRect.size.height)
    {
        return (calculatedRect.angle);          //负值：顺时针移动
    }else
    {
        return (90+calculatedRect.angle);       //正值：逆时针移动
    }
}
/*
 * 函数功能：
 * 构造函数
 */
CTAlign::CTAlign(vector<vector<Mat> > vecVecMat)
{
    setAlignMats(vecVecMat);
}
/*
 * 函数功能：
 * 设置多排Mat
 */
void CTAlign::setAlignMats(vector<vector<Mat> > vecVecMat)
{
    vector<vector<vector<Point> > >().swap(this->vecPoints);
    vector<vector<Mat> >().swap(this->vecMats);
    this->vecMats.swap(vecVecMat);
}
/*
 * 函数功能：
 * 获取单个Mat坐标信息
 * x和y从0开始计数
 */
vector<Point> CTAlign::getAlignPoint(int x, int y)
{
    assert(x<vecPoints.size());
    assert(y<vecPoints[x].size());

    return vecPoints[x][y];
}
/*
 * 函数功能：
 * 获取全部Mat坐标信息
 */
vector<vector<vector<Point> > > CTAlign::getAlignPoints()
{
    return vecPoints;
}
/*
 * 函数功能：
 * 获取全部Mat
 */
vector<vector<Mat> > CTAlign::getAlignMats()
{
    return this->vecMats;
}
/*
 * 函数功能：
 * 获取单个Mat
 */
Mat CTAlign::getAlignMat(int x, int y)
{
    return this->vecMats[x][y];
}
/*
 * 函数功能：
 * 紧密排列
 * arrangeMode为排列方式：横排、竖排
 * arrangeAlign为对齐方式：左、居中、右
 * spacing为种子间的间距，默认是0
 */
#define MATSPACE 5
Mat CTAlign::alignMat(int arrangeMode, int arrangeAlign, int spacing)
{
    vector<vector<Mat> > vecMat(this->vecMats);
    assert(vecMat.size() == 2);

    if(arrangeMode == HORIZONTAL)
    {
        for(vector<vector<Mat> >::iterator ita=vecMat.begin();ita!=vecMat.end();ita++)
        {
            for(vector<Mat>::iterator itb=ita->begin();itb!=ita->end();itb++)
            {
                shared_ptr<CTRotate> rotate = make_shared<CTRotate>();
                *itb = rotate->getRotateMat(*itb, CTRotate::DEGREE90);
            }
        }
    }

    int heightMax=0;    //紧密排列所有单个种子高度最大值
    int widthMax=0;     //紧密排列所有单个种子宽度最大值
    int totalHeight=0;     //紧密排列单个种类中的最大个数，用来确认高度
    vector<int> heightVec(vecMat.size(), 0);         //紧密排列单个种类种子总高度，用来种子紧密排列

    for(vector<vector<Mat> >::iterator ita=vecMat.begin();ita!=vecMat.end();ita++)
    {
        int tempHeight = 0;
        for(vector<Mat>::iterator itb=ita->begin();itb!=ita->end();itb++)
        {
            if(itb->rows > heightMax)
                heightMax = itb->rows;
            if(itb->cols > widthMax)
                widthMax = itb->cols;
            tempHeight += itb->rows;
        }
        totalHeight = (totalHeight<tempHeight?tempHeight:totalHeight);
    }

    //下面用到CV_8UC4通道，故输入的原图必须是4四通道Mat才行
    int matQuaWidth = MATSPACE*2 + widthMax*(vecMat.size()+3) + spacing;  //+3的意思是左右两个空隙以及中间的空隙 目前只有一个spacing，即只支持两排种子排列
    int matQuaHeight = MATSPACE*2 + totalHeight;
    Mat matQua(Size(matQuaWidth,matQuaHeight),CV_8UC4,cv::Scalar(255,255,255,0));

    vector<vector<vector<Point> > >().swap(this->vecPoints);
    for(int i=0;i<vecMat.size();i++)
    {
        vector<vector<Point> > vvPoint;
        for(int j=0;j<(vecMat[i]).size();j++)
        {
            int offsetAlign;
            switch (arrangeAlign) {
            case LEFT:
                offsetAlign = 0;
                break;
            case MID:
                offsetAlign = (widthMax-vecMat[i][j].cols)/2;
                break;
            case RIGHT:
                offsetAlign = widthMax-vecMat[i][j].cols;
                break;
            default:
                break;
            }
            vector<Point> vPoint;
            int panningX = MATSPACE+widthMax*(i*2+1)+offsetAlign+(i==0?0:spacing);
            int panningY = MATSPACE+heightVec[i];
            vPoint.push_back(Point(panningX,panningY));
            vPoint.push_back(Point(panningX + vecMat[i][j].cols,panningY + vecMat[i][j].rows));
            vvPoint.push_back(vPoint);
            shared_ptr<CTRotate> rotate = make_shared<CTRotate>();
            matQua = rotate->panningMat(vecMat[i][j],matQua,panningX,panningY);
            heightVec[i] += vecMat[i][j].rows;
        }
        this->vecPoints.push_back(vvPoint);
    }
    /* 使用opencv函数画线 */
    /* 最上方横线 */
    line(matQua, Point(MATSPACE, MATSPACE), Point(matQua.cols-MATSPACE, MATSPACE), Scalar(0, 0, 255, 255), 1);
    /* 左边种子的底线 */
    line(matQua, Point(MATSPACE, MATSPACE+heightVec[0]), Point(MATSPACE + widthMax*2, MATSPACE+heightVec[0]), Scalar(0, 0, 255, 255), 1);
    /* 右边种子的底线 */
    line(matQua, Point(matQua.cols-MATSPACE-widthMax*2, MATSPACE+heightVec[1]),Point(matQua.cols-MATSPACE, MATSPACE+heightVec[1]), Scalar(0, 0, 255, 255), 1);
    /* 左边种子的边长线 */
    arrowedLine(matQua, Point(MATSPACE+widthMax/2,MATSPACE+heightVec[0]/2-heightMax/4),Point(MATSPACE+widthMax/2,MATSPACE),Scalar(0, 0, 255, 255),1,8,0,0.02);
    arrowedLine(matQua, Point(MATSPACE+widthMax/2,MATSPACE+heightVec[0]/2+heightMax/4),Point(MATSPACE+widthMax/2,MATSPACE+heightVec[0]),Scalar(0, 0, 255, 255),1,8,0,0.02);
    /* 右边种子的边长线 */
    arrowedLine(matQua, Point(matQua.cols-MATSPACE-widthMax/2,MATSPACE+heightVec[1]/2-heightMax/4),Point(matQua.cols-MATSPACE-widthMax/2,MATSPACE),Scalar(0, 0, 255, 255),1,8,0,0.02);
    arrowedLine(matQua, Point(matQua.cols-MATSPACE-widthMax/2,MATSPACE+heightVec[1]/2+heightMax/4),Point(matQua.cols-MATSPACE-widthMax/2,MATSPACE+heightVec[1]),Scalar(0, 0, 255, 255),1,8,0,0.02);
    /* 左边种子的文字 */
    char buffer[256];
    int counter = heightVec[0];
    snprintf(buffer, 4, "%03i", counter);
    string number = std::string(buffer);
    putText(matQua, number, Point(0, MATSPACE+heightVec[0]/2), FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255, 255), 1);
    /* 右边种子的文字 */
    counter = heightVec[1];
    snprintf(buffer, 4, "%03i", counter);
    number = std::string(buffer);
    putText(matQua, number, Point(matQua.cols-MATSPACE-widthMax, MATSPACE+heightVec[1]/2), FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255, 255), 1);

    return matQua;
}
/*
 * 函数功能：
 * 设置单个Mat
 * x和y从0开始计数
 */
void CTAlign::setAlignMat(Mat srcMat, int x, int y)
{
    assert(x<vecMats.size());
    assert(y<vecMats[x].size());
    vecMats[x][y] = srcMat;
}

void testDataAugmentation(Mat srcMat, Mat labelMat)
{
    int alphaV = 255;

    shared_ptr<CTAlpha> alpha = make_shared<CTAlpha>();
    Mat alphaMat1 = alpha->getAlphaPic(srcMat, alphaV);

    shared_ptr<CTRotate> rotate = make_shared<CTRotate>();

    pair<Mat, Mat> rotateMat0 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 0, alphaV);
    pair<Mat, Mat> rotateMat1 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 45, alphaV);
    pair<Mat, Mat> rotateMat2 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 90, alphaV);
    pair<Mat, Mat> rotateMat3 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 135, alphaV);
    pair<Mat, Mat> rotateMat4 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 180, alphaV);
    pair<Mat, Mat> rotateMat5 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 225, alphaV);
    pair<Mat, Mat> rotateMat6 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 270, alphaV);
    pair<Mat, Mat> rotateMat7 = rotate->getRelativeRotateMatAndLabel(alphaMat1, labelMat, 315, alphaV);

    //Mat mirrorMat1 = rotate->getMirrorMat(rotateMat1, CTRotate::MIRRORX);
    //Mat mirrorMat2 = rotate->getMirrorMat(rotateMat2, CTRotate::MIRRORY);

    //debugShowMat(rotateMat1);
    //debugShowMat(rotateMat2);
    //debugShowMat(mirrorMat1);
    //debugShowMat(mirrorMat2);
    debugSaveMat(rotateMat0.first,"rotateMat0.png");
    debugSaveMat(rotateMat0.second,"rotateLabelMat0.png");
    debugSaveMat(rotateMat1.first,"rotateMat1.png");
    debugSaveMat(rotateMat1.second,"rotateLabelMat1.png");
    debugSaveMat(rotateMat2.first,"rotateMat2.png");
    debugSaveMat(rotateMat2.second,"rotateLabelMat2.png");
    debugSaveMat(rotateMat3.first,"rotateMat3.png");
    debugSaveMat(rotateMat3.second,"rotateLabelMat3.png");
    debugSaveMat(rotateMat4.first,"rotateMat4.png");
    debugSaveMat(rotateMat4.second,"rotateLabelMat4.png");
    debugSaveMat(rotateMat5.first,"rotateMat5.png");
    debugSaveMat(rotateMat5.second,"rotateLabelMat5.png");
    debugSaveMat(rotateMat6.first,"rotateMat6.png");
    debugSaveMat(rotateMat6.second,"rotateLabelMat6.png");
    debugSaveMat(rotateMat7.first,"rotateMat7.png");
    debugSaveMat(rotateMat7.second,"rotateLabelMat7.png");
    //debugSaveMat(mirrorMat1,"mirrorMat1.png");
    //debugSaveMat(mirrorMat2,"mirrorMat2.png");
}
