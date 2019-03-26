#include "mylabel.h"
#include "time.h"

#define StartNumber 20
#define UsePointNumber 300
#define PI 3.1415
#define RectNum 1
#define SuoFangStep 4
#define Bound 800

myLabel::myLabel(QWidget *parent) :
    QLabel(parent)
{
	srand(time(0));
    curDir = -5;
    VaildFeature = -5;
    curRandIndex = 0;
    for(int i=0;i<1000;i++)
    {
        randFeature[i] = i%4;
    }
    int t1,t2;
    for(int i=0;i<996;i++)
    {
        t1 = rand()%4;
        t2 = randFeature[i];
        randFeature[i] = randFeature[i+t1];
        randFeature[i+t1] = t2;
    }

    srand(time(NULL));
    lockCur = false;
    running = false;
    PieMode = false;

    UpdateTimer.start(100);
    connect(&UpdateTimer,&QTimer::timeout,this,&myLabel::updateTime);

    for(int i=0;i<4;i++)
    {
        toolRect[i] = QRect(700+i*100,10,100,100);
    }
    toolRect[4] = QRect(0,0,100,100);//计时
    toolRect[5] = QRect(0,400,100,100);//PieCursor

    for(int i=0;i<2;i++)
        movequeue.enqueue(QPoint(0,i*100));
    curDir = -1;
    check = false;
    setMouseTracking(true);
    Press = false;

    file = new QFile("PieCursor.log");

    for(int i=0;i<RectNum;i++)
    {
        //testRect[i] = QRect((rand()%6)*179+134,(rand()%3)*114+123,200,100);
        testRect[i] = QRect(400+i*234,200+i*44,200,100);
        inRect[i] = false;
        rectOk[i] = false;
    }


}
void myLabel::paintEvent(QPaintEvent *)
{
        painter.begin(this);          //进行重绘;
        int s[5] = {5,8,12,17,20};
        painter.setPen(QPen(Qt::white, s[1], Qt::SolidLine, Qt::RoundCap));    //设置画笔;
        painter.setFont(QFont(NULL,35));
        if(curTime>2900&&VaildFeature>=0)
            painter.drawText(QRect(0,90,300,150),Qt::AlignCenter,QString("请使用功能:%1").arg(VaildFeature));
        if(curTime>3000)
        {
            painter.drawText(QRect(1500,30,300,150),Qt::AlignCenter,QString("%1:%2").arg((curTime-3000)/1000).arg(curTime%1000));
        }
        else if(curTime!=0&&curTime<=2900)
            painter.drawText(QRect(0,30,300,150),Qt::AlignCenter,QString("%1").arg((4000-curTime)/1000));

        //painter.setFont(QFont(NULL,30));
        for(int i=0;i<6;i++)
        {
            painter.drawRect(toolRect[i]);
        }

        painter.setFont(QFont(NULL,15));
        painter.drawText(toolRect[0],Qt::AlignCenter,"0");
        painter.drawText(toolRect[1],Qt::AlignCenter,"1");
        painter.drawText(toolRect[2],Qt::AlignCenter,"2");
        painter.drawText(toolRect[3],Qt::AlignCenter,"3");
        if(running)
            painter.drawText(toolRect[4],Qt::AlignCenter,"复位");
        else
            painter.drawText(toolRect[4],Qt::AlignCenter,"开始");
        painter.drawText(toolRect[5],Qt::AlignCenter,"PieCursor");

        painter.setPen(QPen(Qt::yellow, s[1], Qt::SolidLine, Qt::RoundCap));    //设置画笔;
        painter.drawLine(QPoint(0,Bound),QPoint(2025,Bound));
        //画矩形对象
        painter.setFont(QFont(NULL,35));
        for(int i=0;i<RectNum;i++)
        {
            if(rectOk[i])
            {
                painter.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));    //设置画笔;
                painter.drawText(testRect[i],Qt::AlignCenter,"✔");
            }
            else
                painter.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));    //设置画笔;

            painter.drawRect(testRect[i]);
        }

        //画pie
        if(PieMode)
        {
            painter.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));    //设置画笔;
            int size = 70;
            int iconNumber = 4;
            QPoint boundaryOffset(size / 2, size / 2);
            QRect boundary(curPoint - boundaryOffset, curPoint + boundaryOffset);
            painter.drawEllipse(boundary);

            if(!lockCur)
            {
            QVector<QLine> lines;
            for (int i = 8 / iconNumber; i < 8; i += 16 / iconNumber) {

                QPoint offsetValue(static_cast<int>(size * qSin(M_PI_4 * i / 2) / 2),
                                   static_cast<int>(size * qCos(M_PI_4 * i / 2) / 2));
                lines.append(QLine(curPoint + offsetValue,
                                   curPoint - offsetValue));
            }
            painter.drawLines(lines);


            }

            painter.setBrush(Qt::red);
            painter.drawPie(boundary, 5760 - 2880 / iconNumber - curDir * 5760 / iconNumber, 5760 / iconNumber);
            painter.setFont(QFont(NULL,25));
            painter.drawText(QRect(curPoint.x()+size/6, curPoint.y()-size/6,size/3,size/3),Qt::AlignCenter,"0");
            painter.drawText(QRect(curPoint.x()-size/6-size/3, curPoint.y()-size/6,size/3,size/3),Qt::AlignCenter,"2");
            painter.drawText(QRect(curPoint.x()-size/6, curPoint.y()-size/7-size/3,size/3,size/3),Qt::AlignCenter,"1");
            painter.drawText(QRect(curPoint.x()-size/6, curPoint.y()+size/9,size/3,size/3),Qt::AlignCenter,"3");

        }
        painter.end();  //重绘结束;

}

void myLabel::mousePressEvent(QMouseEvent *event)
{
    if(toolRect[5].contains(QRect(event->pos().x(),event->pos().y(),1,1)))
    {
        PieMode = !PieMode;
        if(PieMode)
            this->setCursor(Qt::BlankCursor);
        else
            this->setCursor(Qt::ArrowCursor);
    }

    Press = true;
    judgeRect();
    pressPoint = event->pos();
    if(PieMode)
    {
    }
    else
    {
        for(int i=0;i<4;i++)
        {
            if(toolRect[i].contains(QRect(event->pos().x(),event->pos().y(),1,1)))
            {
                if(i==VaildFeature)
                {
                    this->setCursor(Qt::OpenHandCursor);
                    if(i==0)
                        curDir = 0;
                    else
                        curDir = 4-i;
                }
                else
                {
                    this->setCursor(Qt::ArrowCursor);
                    curDir = -5;
                }
            }

        }
    }


    //计时，复位按钮
    if(toolRect[4].contains(QRect(event->pos().x(),event->pos().y(),1,1)))
    {
        if(!running)
        {
            UpdateTimer.start(99);
            recordTime.restart();
            VaildFeature = randFeature[curRandIndex++];
        }
        else
        {
            UpdateTimer.stop();
            for(int i=0;i<RectNum;i++)
            {
                //testRect[i] = QRect((rand()%6)*179+134,(rand()%3)*114+123,200,100);
                testRect[i] = QRect(400+i*234,200+i*44,200,100);
                inRect[i] = false;
                rectOk[i] = false;
            }
            curTime = 0;


        }
        running = !running;
            //recordTime
        update();
    }

    update();
}
void myLabel::mouseMoveEvent(QMouseEvent *event)
{

    if(PieMode)
    {
        curx = event->pos().x();
        cury = event->pos().y();
        movequeue.dequeue();
        movequeue.enqueue(event->pos());
        if(!Press)
            curMoveDir();
    }
    else
    {

    }
    if(Press)
    {
        for(int i=0;i<RectNum;i++)
        {
            if(inRect[i])
            {

                if(curDir==0&&VaildFeature==0)
                    testRect[i].moveTo(testRect[i].topLeft()+event->pos()-curPoint);
                else if(curDir+VaildFeature==4)
                    testRect[i].moveTo(testRect[i].topLeft()+event->pos()-curPoint);
            }

        }

    }
    curPoint = event->pos();
    judgeOk();

    update();
}

void myLabel::mouseReleaseEvent(QMouseEvent *event)
{
    Press = false;
    update();
}
void myLabel::checkCur()
{
    lockCur = true;
}

void myLabel::uncheckCur()
{
    lockCur = false;

}
void myLabel::judgeInside()
{
}


int myLabel::computeDistance(int index)
{

}

int myLabel::curMoveDir()
{
    if(lockCur)
        return 0;
    front = movequeue.at(0);
    back = movequeue.at(1);
    QPoint offset = back - front;
    curDir =  static_cast<int>(qAtan2(offset.y(), offset.x()) * 4 / M_PI + 1 + 4 * 2) % (4 * 2) / 2;

    return 1;

}

void myLabel::judgeRect()
{
    for(int i=0;i<RectNum;i++)
    {
        if(testRect[i].contains(curPoint))
            inRect[i] = true;
        else
            inRect[i] = false;
    }
}

void myLabel::wheelEvent(QWheelEvent *e)
{
    //if(!Press||curDir != 3)
    return ;

    if(e->delta()<0)
    {
        //下一张图片
    for(int i=0;i<RectNum;i++)
    {
        if(inRect[i])
        {
            testRect[i].setWidth(testRect[i].width()+SuoFangStep*2);
            testRect[i].setHeight(testRect[i].height()+SuoFangStep*2);
            testRect[i].setX(testRect[i].x()-SuoFangStep);
            testRect[i].setY(testRect[i].y()-SuoFangStep);
        }
    }
    }
    else
    {
    for(int i=0;i<RectNum;i++)
    {
        if(inRect[i])
        {
            testRect[i].setWidth(testRect[i].width()-SuoFangStep*2);
            testRect[i].setHeight(testRect[i].height()-SuoFangStep*2);
            testRect[i].setX(testRect[i].x()+SuoFangStep);
            testRect[i].setY(testRect[i].y()+SuoFangStep);
        }
    }
    }
    update();
}

void myLabel::judgeOk()
{
    for(int i=0;i<RectNum;i++)
    {
        if(!rectOk[i]&&testRect[i].y()>Bound)
        {
            this->setCursor(Qt::ArrowCursor);
            rectOk[i] = true;
            QString mode = (PieMode)?"PieMode ":"NormalMode ";
            //qDebug()<<mode<<"time is:"<<recordTime.elapsed();
            UpdateTimer.stop();

            bool ok =file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
            if(ok&&curTime>3000)
            {
                QTextStream out(file);
                out<<mode<<VaildFeature<<" "<<curTime-3000<<endl;
                file->close();
            }
            curDir = -5;
            VaildFeature = -5;
        }

    }
}

void myLabel::updateTime()
{
   curTime = recordTime.elapsed();
   update();
}

