#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include <QString>
#include <QMessageBox>

#include <fstream>
#include <stdio.h>

#include "func.h"

using namespace cv;

int resizeRows=1,resizeCols=1;
int pic_x=0, pic_y=0;
std::vector<std::vector<std::vector<cv::Point>>>contoursRes;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setGeometry(100,100,this->width(),this->height()); //设置窗口初始位置

    ui->lineEdit_l1->setText("3");
    ui->lineEdit_sigma->setText("0.2");
    ui->lineEdit_eps->setText("0.03");
    ui->lineEdit_minPts->setText("10");
    ui->lineEdit_elenum->setText("1");
}

MainWindow::~MainWindow()
{
    delete ui;
}

//读入图片
void MainWindow::on_inputImage_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image File(*.png *.jpg *.jpeg *.bmp *.tif)"));

    Mat src = imread(string(fileName.toLocal8Bit()));

    if(src.empty())
    {
        QString title = title.fromLocal8Bit("提示!");
        QString information = information.fromLocal8Bit("输入图像为空!");
        QMessageBox::information(this, title, information);
        return;
    }

    //设置图片窗口位置
    int window_x=this->pos().x(), window_y=this->pos().y();
    int window_width=this->width();
    pic_x=window_x+window_width, pic_y=window_y;

    resizeRows=this->height();//让图片高度和主窗口高度一致
    float resizeRate=float(src.rows)/resizeRows;
    resizeCols=src.cols/resizeRate;
    cv::resize(src,src_resize,Size(resizeCols,resizeRows));

    namedWindow("OriginalImage");
    moveWindow("OriginalImage",pic_x,pic_y);
    cv::imshow("OriginalImage", src_resize);

    cv::waitKey(0);
}

//检测边缘
void MainWindow::on_detectBoundary_clicked()
{
    ccompSet.clear();

    //--------------------参数:

    //默认参数:
    int l1 = 3;
    int l2 = 1;
    int sigma_1 = 10; //线之间距离，一般为10
    double sigma_2=0.2;  //白条比
    int sigma_3 = 2; //线的数量
    double sigma_4 = 0.2; //延长时的白条比

    int hou_5 = 50;  //一般为50

    //输入参数:
    QString l1_str=ui->lineEdit_l1->text();
    //QString l2_str=ui->lineEdit_l2->text();
    QString sigma2_str=ui->lineEdit_sigma->text();

//    if(l1_str.isEmpty() || sigma2_str.isEmpty())//输入为空采用默认值
//    {
//        QString title = title.fromLocal8Bit("提示!");
//        QString information = information.fromLocal8Bit("有未输入的参数，将采用默认值");
//        QMessageBox::information(this, title, information);
//        //return -1;
//    }else
//    {
    bool ok;
    l1 = l1_str.toInt(&ok,10);
    l2 = l1/2;
    sigma_2=sigma2_str.toDouble();
//    }

    if(l1!=3 && l1!=5 && l1!=7 && l1!=9)
    {
        QString title = title.fromLocal8Bit("提示!");
        QString information = information.fromLocal8Bit("l1超出范围{3,5,7,9}");
        QMessageBox::information(this, title, information);
        return;
    }

    if(sigma_2<0.1 || sigma_2>0.9)
    {
        QString title = title.fromLocal8Bit("提示!");
        QString information = information.fromLocal8Bit("sigma超出范围[0.1,0.9]");
        QMessageBox::information(this, title, information);
        return;
    }

    //--------------------------------------------------
    src = imread(string(fileName.toLocal8Bit()), 0);
    double src_width = src.cols, src_height = src.rows;
    int src_small_width = 512;
    double rate = src_width / src_small_width;
    int src_small_height = src_height / rate;
    //Mat src_small;
    cv::resize(src, src_small, Size(src_small_width, src_small_height));  //把图缩小
//  Mat src = imread("21.tif"); //504(2);21;103；105-2

    Mat lineImage,baoliu;
    src_small.copyTo(lineImage);
    src_small.copyTo(baoliu);

    threshold(src_small, src_small, 100, 255, CV_THRESH_BINARY); //二值化10,255; 100,255  //------------------
    //cvtColor(src_small, src_small, CV_BGR2GRAY);
//        imshow("erzhi2",src_small);
//        //imwrite("erzhi.png",src_small);



     //形态学处理----------------------
    Mat result1;
    cv::Mat element(l1,l1,CV_8U,cv::Scalar(1)); //5,5;;3,3
    cv::dilate(src_small,result1,element);
    //imshow("pz02",result1);
    //imwrite("pz1.png",result1);
    threshold(result1, result1, 3, 255, CV_THRESH_BINARY_INV); //二值化
    //imshow("pengzhang",result1);
    //imwrite("pz2.png",result1);



    //局部灰度共生矩阵-----------------------------
    Mat result2(src_small.rows,src_small.cols,CV_8UC1,Scalar::all(0));
    localGLCM(src_small,result2,1,l2);
    //imshow("glcm",result2);
    //imwrite("GLCM2.png",result2);



    Mat src_color,src_color2;
    cvtColor(src_small,src_color,CV_GRAY2BGR);
    cvtColor(src_small,src_color2,CV_GRAY2BGR);
    //霍夫变换-----------------------------
    vector<Vec4i>Lines1, Lines2, Lines;
    HoughLinesP(result1,Lines1,1,CV_PI/360,80,100,hou_5);   //1,180,80,100,20
    HoughLinesP(result2,Lines2,1,CV_PI/360,80,100,hou_5);   //1,180,80,100,20
    Lines.insert(Lines.end(),Lines1.begin(),Lines1.end());
    Lines.insert(Lines.end(),Lines2.begin(),Lines2.end());

//    for(size_t i=0;i<Lines.size();i++)
//    {
//        line(src_color,Point(Lines[i][0],Lines[i][1]),Point(Lines[i][2],Lines[i][3]),Scalar(0,0,255),1,8);
//    }
//    imshow("qqqr",src_color); //显示所有直线





    //根据直线和白条的交点数排除掉一些直线----------------------------------
//    //对原图进行腐蚀
//    cv::Mat element2(1,1,CV_8U,cv::Scalar(1)); //1,1;  2,2
//    cv::erode(src_small,src_small,element2);
//    imshow("erode",src_small);


    cvtColor(lineImage, lineImage, CV_GRAY2BGR);
    vector<Vec4i>LinesTemp;
    //vector<ConnectedComp> ccompSet; //区域集
    linesFilter2(src_small,Lines,lineImage,ccompSet,sigma_1,sigma_2,sigma_3,sigma_4);

    Mat lineImage_show;
    cv::resize(lineImage,lineImage_show,Size(resizeCols,resizeRows));

    namedWindow("Boundary");
    moveWindow("Boundary",pic_x,pic_y);
    imshow("Boundary",lineImage_show);
    imwrite("results\\finallines.png", lineImage);


    //对不同的区域进一步分析--------------------------------------------------
    //cout<<ccompSet.size();
    QString regnum_str = QString("%1").arg(ccompSet.size());
    ui->text_elenum->setText(regnum_str);
    regSize=regionAnalyser(src,ccompSet,src_small,contoursRes);


    cv::waitKey(0);
}



void MainWindow::on_pushButton_cla_clicked()
{

    //-------------------------------参数:

    //默认参数:
    double _eps = 0.03;
    int _minpts = 10;
    int nReg=1;

    //输入参数:
    QString eps_str=ui->lineEdit_eps->text();
    QString minPts_str=ui->lineEdit_minPts->text();
    QString nReg_str=ui->lineEdit_elenum->text();

//    if(eps_str.isEmpty() || minPts_str.isEmpty() || nReg_str.isEmpty())//输入为空采用默认值
//    {
//        QString title = title.fromLocal8Bit("提示!");
//        QString information = information.fromLocal8Bit("没有输入新的参数，将采用默认值");
//        QMessageBox::information(this, title, information);
//        //return -1;
//    }else
//    {
    bool ok;
    _minpts = minPts_str.toInt(&ok,10);
    nReg = nReg_str.toInt(&ok,10);
    _eps=eps_str.toDouble();
//    }

    nReg-=1;
    if(nReg<0 || nReg>=ccompSet.size())
    {
        QString title = title.fromLocal8Bit("提示!");
        QString information = information.fromLocal8Bit("输入的区域编号超过区域总数");
        QMessageBox::information(this, title, information);
        return;
    }

    if(_minpts<5 || _minpts>30)
    {
        QString title = title.fromLocal8Bit("提示!");
        QString information = information.fromLocal8Bit("minPts超出范围[5,30]");
        QMessageBox::information(this, title, information);
        return;
    }

    if(_eps<0.01 || _eps>0.05)
    {
        QString title = title.fromLocal8Bit("提示!");
        QString information = information.fromLocal8Bit("eps超出范围[0.01,0.05]");
        QMessageBox::information(this, title, information);
        return;
    }

    //    int nReg=1,minPts=10;  //0+0.05+20;1+0.02+25;2+0.05+10;3+0.05+10-----------0+0.02+20;1+0.05+15;2+0.05+10
    //    double eps = 0.03;
    cv::Mat result(regSize,CV_8UC3,Scalar::all(255));
    singleRegionAnalyser(nReg,_eps,_minpts,contoursRes,regSize,result); //对单独一块区域进行分析

    Mat result_show;
    cv::resize(result,result_show,Size(resizeCols,resizeRows));
    namedWindow("SingleRegion");
    moveWindow("SingleRegion",pic_x,pic_y);
    imshow("SingleRegion",result_show);

    cv::waitKey(0);

}
