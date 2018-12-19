#include"ctalign.h"
#include"ctwater.h"

int main(int argc, char *argv[])
{
    Mat srcMat = imread("img.png");
    Mat labelMat = imread("label.png");
    //testShrink();
    //testSkeleton();
    testDataAugmentation(srcMat, labelMat);
    //testAlign();

    cout<<"Finished!"<<endl;

    return 0;
}
