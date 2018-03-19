#include"algoc.h"

int main(int argc, char *argv[])
{
    Mat myImage;
    //myImage = imread("D://mario.jpg");
    //myImage = imread("D://135.png");
    myImage = imread("D://135.png");            //会出错
    //myImage = imread("D://20.png");
    testClass(myImage);

    return 0;
}
