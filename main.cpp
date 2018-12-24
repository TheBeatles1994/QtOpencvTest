#include"ctalign.h"
#include"ctwater.h"

int main(int argc, char *argv[])
{
    Mat srcMat = imread(argv[1]);       //原样本图
    Mat labelMat = imread(argv[2]);     //标签图
    string imgFileName(argv[1]);
    imgFileName = imgFileName.substr(0, imgFileName.length() - 4);  //原样本图对应的文件名称

    /* 测试边缘腐蚀 */
    //testShrink();
    /* 测试水滴法 */
    //testSkeleton();
    /* 测试紧密排列 */
    //testAlign();
    /* 测试数据增广 */
    testDataAugmentation(srcMat, labelMat, imgFileName);

    cout<<"Finished!"<<endl;

    return 0;
}
