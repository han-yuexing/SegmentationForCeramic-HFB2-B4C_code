#include "func.h"
#include<cmath>

using namespace std;
using namespace cv;

#define GLCM_DIS 2  //灰度共生矩阵的统计距离
#define GLCM_CLASS 8 //计算灰度共生矩阵的图像灰度值等级化
#define GLCM_ANGLE_HORIZATION    0   //水平
#define GLCM_ANGLE_VERTICAL      1   //垂直
#define GLCM_ANGLE_DIGONAL_45    2   //45度对角
#define GLCM_ANGLE_DIGONAL_135   3   //135度对角

#pragma region calGLCM

int  GetMax(CvMat* bWavelet,int& max_value,int& min_value)
{
    int i,j;
    int width,height;
    int max = 0;
    int min = 0;
    width = bWavelet->width;
    height = bWavelet->height;
    for (i = 0;i<height;i++ )
    {
        for(j = 0;j<width;j++)
        {
            if (CV_MAT_ELEM(*bWavelet,float,i,j)> max)
                max = CV_MAT_ELEM(*bWavelet,float,i,j);
            if(CV_MAT_ELEM(*bWavelet,float,i,j) < min)
                min = CV_MAT_ELEM(*bWavelet,float,i,j);

        }
    }
    max_value = max;
    min_value = min;
    return 1;

}
//计算GLCM
int calGLCM(CvMat* bWavelet,int angleDirection,double* featureVector)
{

    int i,j;
    int width,height;
    int min,max;

    if(NULL == bWavelet)
        return 1;

    width = bWavelet->width;
    height = bWavelet->height;

    int * glcm = new int[GLCM_CLASS * GLCM_CLASS];
    int * histImage = new int[width * height];

    if(NULL == glcm || NULL == histImage)
        return 2;

    //灰度等级化---分GLCM_CLASS个等级
    //uchar *data =(uchar*) bWavelet->imageData;


    //GetMax(bWavelet,max,min);
    //printf("max = %d,min = %d ",max,min);
    //system("pause");
    int relative_value = 0;
    for(i = 0;i < height;i++){
        for(j = 0;j < width;j++){
            histImage[i * width + j] = (int)(CV_MAT_ELEM(*bWavelet,float,i,j) * GLCM_CLASS / 256);
            //relative_value = CV_MAT_ELEM(*bWavelet,float,i,j) - min;
            //histImage[i * width + j] = (int)(relative_value * GLCM_CLASS / (max-min+1));
            //printf("%d ",histImage[i * width + j]);
        }
    }

    //初始化共生矩阵
    for (i = 0;i < GLCM_CLASS;i++)
        for (j = 0;j < GLCM_CLASS;j++)
            glcm[i * GLCM_CLASS + j] = 0;

    //计算灰度共生矩阵
    int w,k,l;
    //水平方向
    if(angleDirection == GLCM_ANGLE_HORIZATION)
    {
        for (i = 0;i < height;i++)
        {
            for (j = 0;j < width;j++)
            {
                l = histImage[i * width + j];
                if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width)
                {
                    k = histImage[i * width + j + GLCM_DIS];
                    glcm[l * GLCM_CLASS + k]++;
                }
                if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width)
                {
                    k = histImage[i * width + j - GLCM_DIS];
                    glcm[l * GLCM_CLASS + k]++;
                }
            }
        }
    }
    //垂直方向
    else if(angleDirection == GLCM_ANGLE_VERTICAL)
    {
        for (i = 0;i < height;i++)
        {
            for (j = 0;j < width;j++)
            {
                l = histImage[i * width + j];
                if(i + GLCM_DIS >= 0 && i + GLCM_DIS < height)
                {
                    k = histImage[(i + GLCM_DIS) * width + j];
                    glcm[l * GLCM_CLASS + k]++;
                }
                if(i - GLCM_DIS >= 0 && i - GLCM_DIS < height)
                {
                    k = histImage[(i - GLCM_DIS) * width + j];
                    glcm[l * GLCM_CLASS + k]++;
                }
            }
        }
    }
    //135度对角方向
    else if(angleDirection == GLCM_ANGLE_DIGONAL_135)
    {
        for (i = 0;i < height;i++)
        {
            for (j = 0;j < width;j++)
            {
                l = histImage[i * width + j];

                if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width && i + GLCM_DIS >= 0 && i + GLCM_DIS < height)
                {
                    k = histImage[(i + GLCM_DIS) * width + j + GLCM_DIS];
                    glcm[l * GLCM_CLASS + k]++;
                }
                if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width && i - GLCM_DIS >= 0 && i - GLCM_DIS < height)
                {
                    k = histImage[(i - GLCM_DIS) * width + j - GLCM_DIS];
                    glcm[l * GLCM_CLASS + k]++;
                }
            }
        }
    }
    //45度对角方向
    else if(angleDirection == GLCM_ANGLE_DIGONAL_45)
    {
        for (i = 0;i < height;i++)
        {
            for (j = 0;j < width;j++)
            {
                l = histImage[i * width + j];

                if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width && i - GLCM_DIS >= 0 && i - GLCM_DIS < height)
                {
                    k = histImage[(i - GLCM_DIS) * width + j + GLCM_DIS];
                    glcm[l * GLCM_CLASS + k]++;
                }
                if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width && i + GLCM_DIS >= 0 && i + GLCM_DIS < height)
                {
                    k = histImage[(i + GLCM_DIS) * width + j - GLCM_DIS];
                    glcm[l * GLCM_CLASS + k]++;
                }
            }
        }
    }

    //计算特征值
    double entropy = 0,energy = 0,contrast = 0,homogenity = 0;
    for (i = 0;i < GLCM_CLASS;i++)
    {
        for (j = 0;j < GLCM_CLASS;j++)
        {
            //熵
            if(glcm[i * GLCM_CLASS + j] > 0)
                entropy -= glcm[i * GLCM_CLASS + j] * log10(double(glcm[i * GLCM_CLASS + j]));
            //能量
            energy += glcm[i * GLCM_CLASS + j] * glcm[i * GLCM_CLASS + j];
            //对比度
            contrast += (i - j) * (i - j) * glcm[i * GLCM_CLASS + j];
            //一致性
            homogenity += 1.0 / (1 + (i - j) * (i - j)) * glcm[i * GLCM_CLASS + j];
        }
    }
    //返回特征值
    i = 0;
    featureVector[i++] = entropy;
    featureVector[i++] = energy;
    featureVector[i++] = contrast;
    featureVector[i++] = homogenity;

    delete[] glcm;
    delete[] histImage;
    return 0;
}

#pragma endregion calcGLCM
//计算方差

double calc_variance(double a[4])
{
    double sum;
    double mean;
    double variance;
    sum = a[0]+a[1]+a[2]+a[3];
    mean = sum/4.0;

    variance = (a[0]-mean)*(a[0]-mean) + (a[1]-mean)*(a[1]-mean) + (a[2]-mean)*(a[2]-mean)+(a[3]-mean)*(a[3]-mean);
    variance = variance/4.0;
    return sqrt(variance);
}


//--------------------------------------------------根据阈值二值化--------------
void localGLCM(Mat image, Mat &result, float threshold, int radius){

    double number = (radius*2+1) * (radius*2+1);
    int step=radius*2+1;

    CvMat* cwindow = cvCreateMat(step,step,CV_32FC1);


    double features[10];

    double entropy[4] = {0};
    double energy[4] = {0};
    double contrast[4] = {0};
    double homogenity[4] = {0};
    double variance;

    for(int y=radius;y<image.rows-radius;y+=step)
    {
            for(int x=radius;x<image.cols-radius;x+=step)
            {

                    for(int i=0;i<step;i++){
                            for(int j=0;j<step;j++){
                                    CV_MAT_ELEM(*cwindow,float,i,j) = image.at<uchar>(y+(i-radius),x+(j-radius));
                            }
                    }

                    //计算特征值
                    calGLCM(cwindow,0,features);
                    contrast[0] = features[2];
                    //entropy[0] = features[0];
                    //energy[0] = features[1];
                    //homogenity[0] = features[3];

                    calGLCM(cwindow,1,features);
                    contrast[1] = features[2];
                    //entropy[1] = features[0];
                    //energy[1] = features[1];
                    //homogenity[1] = features[3];

                    calGLCM(cwindow,2,features);
                    contrast[2] = features[2];
                    //entropy[2] = features[0];
                    //energy[2] = features[1];
                    //homogenity[2] = features[3];

                    calGLCM(cwindow,3,features);
                    contrast[3] = features[2];
                                //entropy[3] = features[0];
                    //energy[3] = features[1];
                    //homogenity[3] = features[3];

                    //计算方差
                    variance = calc_variance(contrast);
                    //cout<<variance<<',';

                    //根据方差和阈值的对比
                    if (variance >threshold){
                            for(int i=0;i<step;i++){
                                for(int j=0;j<step;j++){
                                        result.at<uchar>(y+(i-radius),x+(j-radius))=0;
                                }
                        }
                    }else{
                            for(int i=0;i<step;i++){
                                for(int j=0;j<step;j++){
                                        result.at<uchar>(y+(i-radius),x+(j-radius))=255;
                                }
                        }
                    }

            }
    }

}
