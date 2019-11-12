#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <opencv2/opencv.hpp>

typedef struct ConnectedComp
{
    int area;     //面积
    std::vector<cv::Point> pointSet; //区域内的点集
    int num; //编号
}
ConnectedComp;

void localGLCM(cv::Mat image, cv::Mat &result, float threshold,int radius);

std::vector<cv::Vec4i> linesFilter(cv::Mat src_binary, std::vector<cv::Vec4i>Lines, cv::Mat &lineImg);
void linesFilter2(cv::Mat src_binary, std::vector<cv::Vec4i>Lines, cv::Mat &line_img, std::vector<ConnectedComp> &ccompSet,int sigma_1,double sigma_2,int sigma_3,double sigma_4);
cv::Size regionAnalyser(cv::Mat& src,std::vector<ConnectedComp> &ccompSet,cv::Mat& src_small,std::vector<std::vector<std::vector<cv::Point>>>& contoursRes);
void singleRegionAnalyser(int nReg,double eps,int minPts,std::vector<std::vector<std::vector<cv::Point>>>& contoursRes,cv::Size regSize,cv::Mat& result);

#endif // FUNC_H
