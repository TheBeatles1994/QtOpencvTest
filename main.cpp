#include"algoc.h"

int main(int argc, char *argv[])
{
    Mat myImage;
    //myImage = imread("D://mario.jpg");
    myImage = imread("D://30.png");
    //myImage = imread("D://135.png");
    //myImage = imread("D://20.png");
#if TESTMARIO
    myImage = imread("D://mario.jpg");
    CvTest *algoc = new CvTest;
    Mat saveMat = algoc->rotatePic(myImage,22);
    algoc->debugShowMat(saveMat);
    imwrite("marioR.jpg",saveMat);
#else
    CvTest *algoc = new CvTest;
    Mat alphaMat = algoc->alphaPic(myImage);
    Mat saveMat = algoc->rotatePic(alphaMat,algoc->getRotateDegree(myImage));
    saveMat = algoc->removeEdge(saveMat);
    algoc->debugShowMat(saveMat);
    imwrite("saveMat.png",saveMat);
#endif
    return 0;
}
