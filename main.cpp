#include"algoc.h"

int main(int argc, char *argv[])
{
    Mat myImage;
    myImage = imread("D://10.jpg");

    CvTest *algoc = new CvTest;
    Mat alphaMat = algoc->startAlphaPic(myImage);
    Mat saveMat = algoc->startRotatePic(alphaMat,-algoc->getRotateDegree(myImage));

    imwrite("saveMat.png",saveMat);

    return 0;
}

