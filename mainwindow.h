#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMouseEvent>
#include <opencv2/opencv.hpp>
#include <math.h>

#include "func.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_detectBoundary_clicked();

    void on_inputImage_clicked();

    void on_pushButton_cla_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    cv::Mat src_resize;
    cv::Mat src;
    cv::Mat src_small;
    std::vector<ConnectedComp> ccompSet;
    cv::Size regSize;
};

#endif // MAINWINDOW_H
