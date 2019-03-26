#ifndef MYLABEL_H
#define MYLABEL_H

#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QQueue>
#include <QPainter>
#include <QPoint>
#include <QtDebug>
#include <QMouseEvent>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QtMath>
#include <QFile>
#include <QTextStream>

#define QDBG qDebug()
#define MAXCornerLen 1000
#define NoiseSize 10    //十分之一的长或宽
#define MAX_POINT_NUMBER 1000

class Line{
public:
    QPoint pnt[10000];
    int length;
    QString curKey;

    QPoint cornerArr[MAXCornerLen];
    int cornerLen;
    QPoint preCornerPoint;
    QPoint preDir;

    QPoint stepArr[20];
    bool end;
    QFile *file;

    QString dirString;
    void init(){
            end = false;
            length =0;
            cornerLen = 0;
            for(int i=0;i<MAXCornerLen;i++)
            {
                cornerArr[i].setX(0);
                cornerArr[i].setY(0);
            }
    }
    void predect()
    {
        dirString.clear();
        for(int i=1;i<cornerLen;i++)
        {
            int dir=4;
            QPoint temp = (cornerArr[i]-cornerArr[i-1]);
            if(temp.x()<0)
                dir-=3;
            else if(temp.x()>0)
                dir+=3;
            if(temp.y()<0)
                dir-=1;
            else if(temp.y()>0)
                dir+=1;
            dirString.append('0'+dir);
        }

        //横线去噪
        if(dirString=="0"||dirString=="1"||dirString=="2")
            curKey = "左";
        else if(dirString=="6"||dirString=="7"||dirString=="8")
            curKey = "右";
        else if(dirString=="6868")
            curKey = "M";
        else if(dirString=="8686")
            curKey = "W";
        else if(dirString=="6826"||dirString=="6827"||dirString=="6828")
            curKey = "2";
        else if(dirString=="626"||dirString=="627"||dirString=="628"||
                dirString=="726"||dirString=="727"||dirString=="728"||
                dirString=="826"||dirString=="827"||dirString=="828")
            curKey = "Z";
        else if(dirString.indexOf("282")>-1)
            curKey = "S";
        else if(dirString.indexOf("286")>-1)
            curKey = "O";
        else if(dirString.indexOf("820")>-1)
            curKey = "O";//顺
        else if(dirString=="28")
            curKey = "<";
        else if(dirString=="82")
            curKey = ">";
        else if(dirString=="68")
            curKey = "^";
        else if(dirString=="86")
            curKey = "V";
        else
            curKey = " ";



    }
    void addPoint(QPoint p){
        if(length>=9995) return ;
        pnt[length]=p;
        length++;
        if(length>2)
        {
            QPoint curDir = p - preCornerPoint;
            //if((curDir.x()*preDir.x())<0||(curDir.y()*preDir.y())<0)

            bool corner = false;
            if(curDir.x()>=0&&preDir.x()<0)
            {
                corner = true;
            }
            else if(curDir.x()<0&&preDir.x()>=0)
            {
                corner = true;
            }
            else if(curDir.y()>=0&&preDir.y()<0)
            {
                corner = true;
            }
            else if(curDir.y()<0&&preDir.y()>=0)
            {
                corner = true;
            }
            if(corner)
            {
                cornerArr[cornerLen++] = pnt[length-2];
            }
            preCornerPoint = p;
            preDir = curDir;
        }
        else if(length==1)
        {
            preCornerPoint = p;
            cornerLen = 0;
        }
        else if(length==2)
        {
            preDir = p - preCornerPoint;
            cornerArr[cornerLen++] = pnt[length-2];
        }
    }
    void AddEnd(){

        int y_max=-1,y_min=88888;
        int x_max=-1,x_min=88888;

        preCornerPoint = pnt[0];
        bool valid[MAXCornerLen];
        for(int i=0;i<cornerLen;i++)
        {
            if(cornerArr[i].x()>x_max)
                x_max = cornerArr[i].x();
            else if(cornerArr[i].x()<x_min)
                x_min = cornerArr[i].x();
            if(cornerArr[i].y()>y_max)
                y_max = cornerArr[i].y();
            else if(cornerArr[i].y()<y_min)
                y_min = cornerArr[i].y();
        }
        //int unitx = (x_max-x_min)/NoiseSize;
        //int unity = (y_max-y_min)/NoiseSize;
        int unitx = 50;
        int unity = 50;
        cornerArr[cornerLen++] = pnt[length-1];
        //去噪,记录噪点
        QPoint temp;
        for(int i=1;i<cornerLen;i++)
        {
            temp = QPoint(cornerArr[i]-preCornerPoint);
            if(abs(temp.x())<unitx&&abs(temp.y())<unity)
                valid[i] = false;
            else
                valid[i] = true;
            preCornerPoint = cornerArr[i];
        }
        //删除噪点
        int oldLen = cornerLen;
        cornerLen = 1;
        for(int i=1;i<oldLen;i++)
        {
            if(valid[i])
            {
                cornerArr[cornerLen++] = cornerArr[i];
            }
            else
            {
                cornerArr[i].setX(0);
                cornerArr[i].setY(0);
            }
        }
        //去掉同一条线的
        int i;
        for(i=1;i<(cornerLen-1);i++)
        {
            QPoint temp = QPoint(cornerArr[i]-cornerArr[i-1]);
            QPoint temp2 = QPoint(cornerArr[i+1]-cornerArr[i]);
            if(abs(temp.x())<unitx&&abs(temp2.x())<unitx)
                valid[i] = false;
            else if(abs(temp.y())<unity&&abs(temp2.y())<unity)
                valid[i] = false;
            else
                valid[i] = true;
        }
        valid[i] = true;//增加最后一个点
        //删除噪点
        oldLen = cornerLen;
        cornerLen = 1;
        for(int i=1;i<oldLen;i++)
        {
            if(valid[i])
            {
                cornerArr[cornerLen++] = cornerArr[i];
            }
        }
    }
};
class MyPoint{
public:
    QPoint point;
    bool inside;
    int label;
    QString shape;
    MyPoint(){
        inside = false;
        label = 0;
        shape = "dot";
        point = QPoint(rand()%1900,rand()%1000);
    }
};

class myLabel : public QLabel
{
    Q_OBJECT
public:
    explicit myLabel(QWidget*parent = 0);

    void judgeInside();
    int computeDistance(int index);
    int curMoveDir();

    void judgeRect();
    void judgeOk();
    void updateTime();

    void checkCur();
    void uncheckCur();
private:
    QPainter painter;
    bool Press;
    Line line;
    int lineNumber;
    int curx,cury;
    int curx_2,cury_2;
    QQueue<QPoint> movequeue;
    MyPoint allPoint[MAX_POINT_NUMBER];
    int insideList[30];
    int insideNumber;
    int curCheck;
    bool check;
    int CursorRadius;

    bool RecordingLine;
    int curDir;
    QPoint curPoint;
    QPoint pressPoint;
    QPoint front;
    QPoint back;

    QRect testRect[9];
    bool inRect[9];
    bool rectOk[9];

    QTime recordTime;
    int curTime;
    QTimer UpdateTimer;

    QRect toolRect[6];

    bool PieMode;

    bool lockCur;
    bool running;
    QFile *file;

    int VaildFeature;

    int randFeature[1000];
    int curRandIndex;
protected:

    void wheelEvent(QWheelEvent *e);
public slots:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent*event);
};

#endif // MYLABEL_H
