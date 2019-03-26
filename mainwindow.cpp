#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(68,68,68));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug()<<this->width()<<"windows"<<this->height();
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    //qDebug()<<"press:"<<ev->key();
    if(ev->key()==Qt::Key_Shift)
    {
        ui->label_2->checkCur();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
    //qDebug()<<"release:"<<ev->key();
    if(ev->key()==Qt::Key_Shift)
    {
        ui->label_2->uncheckCur();
    }
}
