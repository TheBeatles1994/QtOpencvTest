#ifndef UTILITY
#define UTILITY

#include "string.h"
#include <qstring.h>
#include <windows.h>
#include <qdir.h>
#include <windows.h>
#include <QProcess>
#include<QCoreApplication>
#include<QTime>
#include "time.h"
#include <QImage>
#include <QPixmap>

#define MyClock clock_t

//判断某文件夹是否存在，不存在则创建
bool IsDirExist(QString fullPath);

void        ConvertUnCharToStr(char* str, unsigned char* UnChar, int ucLen);
void        ConvertStrToUnChar(char* str, unsigned char* UnChar);
QString     ConvertUnCharToQString(unsigned char *str, int length);
QString     ConvertStrToQString(std::string str);
std::string ConvertQStringToStr(QString q_str);
char *      ConvertStrToChar(std::string std_str);                              //将string转换成char*
std::string ConvertCharToString(const char *c_str);                             //将char* 转换成string
char *      ConvertQStrToChar(QString q_str);                                   //将QString转换成char*
QString     ConvertCharToQString(const char *c_str);                            //将char* 转QString
QString     ConvertIntToQString(int temp);                                      //将int转化为QString
int         ConvertQStringToInt(QString str, int n);                            //将QString转化为int
QString     ConvertFloatToQString(float temp);                                  //将float转化为QString
float       ConvertQStringToFloat(QString str);                                 //将QString转化为float


//功能 获得当前路径
char* GetAppDir(char* szPath);
//把source字符串里所有s1字符全部替换成字符s2
void replace_char(char *result, char *source, char s1, char s2);
//把source字符串里所有s1字符串全部替换成字符串s2
void replace_string(char *result, char *source, char* s1, char *s2);

//判断一点在不在四个点组成的四边形内部
struct point //点
{
    double x, y;
};
inline int min(int x, int y)
{
    return x > y ? y : x;
}
inline int max(int x, int y)
{
    return x > y ? x : y;
}
// 判断点是否在四边形内部
bool inIt(point pCur, point pLeftTop, point pLeftBelow, point pRightBelow, point pRightTop);
//获得程序运行的内存大小
QString getUsedMemory(DWORD pid);
//延迟
void delaymsec(int msec);
MyClock getclock();

bool copyFile(QString, QString);
QPixmap removeBorder(QImage img);

#endif // UTILITY

