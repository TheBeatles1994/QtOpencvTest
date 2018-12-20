#include"ctalign.h"
#include"ctwater.h"

int main(int argc, char *argv[])
{
    //char *str = "yes.jpg";
    Mat srcMat = imread(argv[1]);
    Mat labelMat = imread(argv[2]);
    string imgName(argv[1]);
    imgName = imgName.substr(0, imgName.length() - 4);

//    cout<<imgName<<endl;
//    cout<<argc<<endl;
    //testShrink();
    //testSkeleton();
    testDataAugmentation(srcMat, labelMat, imgName);
    //testAlign();

    cout<<"Finished!"<<endl;

    return 0;
}
