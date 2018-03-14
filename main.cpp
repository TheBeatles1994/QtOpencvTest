#include"algoc.h"

int main(int argc, char *argv[])
{
    Mat myImage;
    //myImage = imread("D://mario.jpg");
    myImage = imread("D://30.png");
    //myImage = imread("D://135.png");            //会出错
    //myImage = imread("D://20.png");

    CvTest *algoc = new CvTest;
    Mat alphaMat = algoc->alphaPic(myImage);
    Mat saveMat = algoc->rotatePic(alphaMat,algoc->getRotateDegree(myImage));
    saveMat = algoc->removeEdge(saveMat);
//    Mat saveMatTemp = algoc->panningPic(saveMat);
//    Mat testMat = algoc->panningPic(myImage);
    testROI(saveMat);

    return 0;
}
