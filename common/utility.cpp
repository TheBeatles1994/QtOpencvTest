#include <iostream>
#include <string>
#include "utility.h"

bool IsDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }
    else
    {
       bool ok = dir.mkdir(fullPath);//只创建一级子目录，即必须保证上级目录存在
       return ok;
    }
}
//将字符串unsigned char*转换为字符串char*
void ConvertUnCharToStr(char* str, unsigned char* UnChar, int ucLen)
{
    int i = 0;
    for(i = 0; i < ucLen; i++)
    {
        //格式化输str,每unsigned char 转换字符占两位置%x写输%X写输
        sprintf(str + i * 2, "%02x", UnChar[i]);
    }
}
//将字符串char*转换为字符串unsigned char*
void ConvertStrToUnChar(char* str, unsigned char* UnChar)
{
    int i = strlen(str), j = 0, counter = 0;
    char c[2];
    unsigned int bytes[2];

    for (j = 0; j < i; j += 2)
    {
        if(0 == j % 2)
        {
            c[0] = str[j];
            c[1] = str[j + 1];
            sscanf(c, "%02x" , &bytes[0]);
            UnChar[counter] = bytes[0];
            counter++;
        }
    }
    return;
}
//将unsigned char*字符串转换为QString
QString ConvertUnCharToQString( unsigned char *str , int length){
    QString result = "";
    //int lengthOfString = strlen( reinterpret_cast<const char*>(str) );
    int lengthOfString = length;
    printf("len: %d\n", lengthOfString);

    // print string in reverse order
    QString s;
    for( int i = 0; i < lengthOfString; i++ ){
        s = QString( "%1" ).arg( str[i], 0, 16 );

        // account for single-digit hex values (always must serialize as two digits)
        if( s.length() == 1 )
            result.append( "0" );

        result.append( s );
    }

    return result;
}
//将string转换为QString
QString ConvertStrToQString(std::string str)
{
    return QString::fromStdString(str);
}

//将QString转换成std::string
std::string ConvertQStringToStr(QString q_str)
{
    return q_str.toStdString();
}

//将string转换成char*
char *ConvertStrToChar(std::string std_str)
{
    return const_cast<char*>(std_str.c_str());
}

//将char* 转换成string
std::string ConvertCharToString(const char *c_str)
{
    std::string str = c_str;
    return str;
}

//将QString转换成char*
char *ConvertQStrToChar(QString q_str)
{
#if 1
    char*  ch;
    QByteArray ba = q_str.toLatin1();
    ch=ba.data();
    //printf("ch = %s\n", ch);
    return ch;
#else
    char *ch = const_cast<char*>(q_str.toStdString().c_str());
    return ch;
#endif
}

//将char* 转QString
QString ConvertCharToQString(const char *c_str)
{
    return QString(QLatin1String(c_str));
}

// 功能 获得当前路径
char* GetAppDir(char* szPath)
{
    char* ret = szPath;
    GetModuleFileName(NULL, szPath, MAX_PATH); // 得到当前执行文件的文件名（包含路径）
    *(strrchr(szPath , '\\')) = '\0';   // 删除文件名，只留下目录
    return ret;
}

//把source字符串里所有s1字符全部替换成字符s2
void replace_char(char *result, char *source, char s1, char s2)
{
    int i = 0;
    char *q = NULL;

    q = source;
    for(i=0; i<strlen(q); i++)
    {
        if(q[i] == s1)
        {
            q[i] = s2;
        }
    }
    strcpy(result, q);
}
//把source字符串里所有s1字符串全部替换成字符串s2
void replace_string(char *result, char *source, char* s1, char *s2)
{
    char *q=NULL;
    char *p=NULL;

    p=source;
    while((q=strstr(p, s1))!=NULL)
    {
        strncpy(result, p, q-p);
        result[q-p]= '\0';//very important, must attention!
        strcat(result, s2);
        strcat(result, q+strlen(s1));
        strcpy(p,result);
    }
    strcpy(result, p);
}
//将int转化为QString
QString ConvertIntToQString(int temp)
{
    //转为10进制，可设置下面第二个参数
    return QString::number ( temp, 10 );
}
//将QString转化为int
int ConvertQStringToInt(QString str,int n=10)
{
    bool ok;
    //转为10进制，可设置下面第二个参数
    return str.toInt(&ok,n);
}
//将float转化为QString
QString ConvertFloatToQString(float temp)
{
    QString data = QString("%1").arg(temp);
    return data;
}
//将QString转化为float
float ConvertQStringToFloat(QString str)
{
    return str.toFloat();
}

/* 判断一点在不在四个点组成的四边形内部 */
// POINT pCur 指定的当前点
// POINT pLeftTop(左上), POINT pLeftBelow(左下), POINT pRightBelow(右下), POINT pRightTop(右上), 四边形的四个点
bool inIt(point pCur, point pLeftTop, point pLeftBelow, point pRightBelow, point pRightTop)
{
    int nCount = 4;//任意四边形有4个顶点
    point RectPoints[4] = { pLeftTop, pLeftBelow, pRightBelow, pRightTop };
    int nCross = 0;
    for (int i = 0; i < nCount; i++)
    {
        //依次取相邻的两个点
        point pBegin = RectPoints[i];
        point pEnd = RectPoints[(i + 1) % nCount];
        //相邻的两个点是平行于x轴的,当前点和x轴的平行线要么重合,要么不相交,不算
        if (pBegin.y == pEnd.y)continue;
        //交点在pBegin,pEnd的延长线上,不算
        if (pCur.y < min(pBegin.y, pEnd.y) || pCur.y > max(pBegin.y, pEnd.y))continue;
        //当前点和x轴的平行线与pBegin,pEnd直线的交点的x坐标
        double x = (double)(pCur.y - pBegin.y) * (double)(pEnd.x - pBegin.x) / (double)(pEnd.y - pBegin.y) + pBegin.x;
        if (x > pCur.x)//只看pCur右边交点
            nCross++;
    }
    // 单方向交点为奇数，点在多边形之内
    // 单方向交点为偶数，点在多边形之外
    return (nCross % 2 == 1);
}

/**
  * 查询程序占用内存。
  * 思路：通过调用外部命令'tasklist /FI "PID EQ pid"'。
  * 将返回的字符串首先替换掉','，
  * 然后用正则表达式匹配已KB为单位表示内存的字符串，
  * 最后换算为MB为单位返回。
  */
QString getUsedMemory(DWORD pid)
{
    char pidChar[25];
    //将DWORD类型转换为10进制的char*类型
    _ultoa(pid,pidChar,10);

    //调用外部命令
    QProcess p;
    p.start("tasklist /FI \"PID EQ " + QString(pidChar) + " \"");
    p.waitForFinished();
    //得到返回结果
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    //关闭外部命令
    p.close();

    //替换掉","
    result = result.replace(",","");
    //匹配 '数字+空格+K'部分。
    QRegExp rx("(\\d+)(\\s)(K)");
    //初始化结果
    QString usedMem("");
    if(rx.indexIn(result) != -1){
        //匹配成功
        usedMem = rx.cap(0);
    }
    //截取K前面的字符串，转换为数字，供换算单位使用。
    usedMem = usedMem.left(usedMem.length() - 1);
    //换算为MB的单位
    return QString::number(usedMem.toDouble() / 1024) + " MB";
}

void delaymsec(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while( QTime::currentTime() < dieTime )

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

MyClock getclock()
{
    return clock();
}

/*
 * 函数功能：
 * 复制文件
*/
bool copyFile(QString oldFile, QString newFile)
{
    QFile file(newFile);
    if(file.exists())
    {
        file.remove();
    }
    QFile::copy(oldFile, newFile);
}
/*
 * 函数功能：
 * 移除黑边框，同时返回QPixmap
 */
#define BLACKBORDERW 45
#define BLACKBORDERH 140
QPixmap removeBorder(QImage img)
{
    return QPixmap::fromImage(img).copy(BLACKBORDERW,BLACKBORDERH,img.width()-BLACKBORDERW,img.height()-BLACKBORDERH);
}
