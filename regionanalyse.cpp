#include "func.h"
#include <fstream>
#include <cmath>
// 对划分后的区域进一步分析
using namespace std;
using namespace cv;

typedef struct barFeatures
{
        int flag;     //所属区域
        double CDRate; //长短轴之比
        double changZhou;
        double duanZhou;
        double xielv; //斜率
        int x,y; //质心坐标
}
barFeatures;


void makeMask(Mat mask[],vector<ConnectedComp> &ccompSet,int rows,int cols){

        for (int n = 0; n<ccompSet.size(); n++) {
                Mat temp(rows,cols,CV_8UC1,Scalar::all(0));
                int s=ccompSet[n].pointSet.size();
                //cout<<s<<',';//......................  //每个区域像素数

                for (int i = 0; i<ccompSet[n].pointSet.size(); i++) {
                        temp.at<uchar>(ccompSet[n].pointSet[i].y, ccompSet[n].pointSet[i].x) = 255;
                }
                temp.copyTo(mask[n]);
        }
}


//计算每个白条的长短轴之比以及长轴方向
void computeWhitebars(std::vector<std::vector<cv::Point>>& contours,int flag,vector<barFeatures>& barsFeatures,Mat& region){
        //遍历所有轮廓
        std::vector<std::vector<cv::Point>>::const_iterator itc=contours.begin();

        //删除太小的轮廓
        while(itc!=contours.end()){
         if(itc->size()<5) //?????????????
                itc=contours.erase(itc);
         else
                ++itc;
    }

        //cout<<'\n'<<"num of contours:"<<contours.size(); //轮廓数目
        char picName[10];
        //在白色图像上绘制黑色轮廓
        cv::Mat result(region.size(),CV_8U,Scalar(255));
        cv::drawContours(result,contours,-1,cv::Scalar(0),1);
        sprintf(picName,"c%d.png",flag);
        //imwrite(picName,result);


        //计算长短轴之比
        double changzhou,duanzhou,rate,xielv;
        itc=contours.begin();
        vector<double> rateVector,xielvVector;
        while(itc!=contours.end()){
                barFeatures singleBar;

            //计算所有的力矩
                cv::Moments mom=cv::moments(cv::Mat(*itc));
                //计算质心
                singleBar.x=mom.m10/(mom.m00+0.000001);
                singleBar.y=mom.m01/(mom.m00+0.000001);

                //计算长短轴之比
                RotatedRect box = minAreaRect(cv::Mat(*itc));
                //changzhou=box.size.width;
                //duanzhou=box.size.height;
                if(box.size.width > box.size.height){
                        singleBar.changZhou=box.size.width;
                        singleBar.duanZhou=box.size.height;
                }else{
                        singleBar.changZhou=box.size.height;
                    singleBar.duanZhou=box.size.width;
                }
                singleBar.CDRate = singleBar.changZhou/singleBar.duanZhou;

                //计算斜率
                Point2f vtx[4];
                box.points(vtx);

                if(sqrt(pow((vtx[0].x-vtx[1].x),2)+pow((vtx[0].y-vtx[1].y),2)) > sqrt(pow((vtx[0].x-vtx[3].x),2)+pow((vtx[0].y-vtx[3].y),2)))
                {
                        xielv = double(vtx[0].y-vtx[1].y)/double(vtx[0].x - vtx[1].x+0.000001);
                }else{
                        xielv = double(vtx[0].y-vtx[3].y)/double(vtx[0].x - vtx[3].x+0.000001);
                }

                //............
                if(xielv>5)
                        xielv=5;
                else if(xielv<-5)
                        xielv=-5;


                //singleBar.CDRate=rate;  //记录长短轴之比
                //singleBar.CDRate=duanzhou;  //只记录长轴长度
                singleBar.xielv=xielv;
                singleBar.flag=flag;

                barsFeatures.push_back(singleBar);
                ++itc;

                //if(/*abs(xielv)>0.001 &&*/ abs(xielv)<50){ //排除斜率过小或过大的
                //	barsFeatures.push_back(singleBar);
                //	++itc;
                //}else{
                //	itc=contours.erase(itc);
                //}
        }


}




Size regionAnalyser(Mat& src,vector<ConnectedComp> &ccompSet,Mat& src_small,std::vector<std::vector<std::vector<cv::Point>>>& contoursRes){
        ofstream file("results\\regionsNum.txt");
        file<<ccompSet.size(); //输出区域数
        file.close();

        Mat *mask = new Mat[ccompSet.size()]; //掩模
        Mat *mask_large = new Mat[ccompSet.size()];
        Mat *regions = new Mat[ccompSet.size()];//区域

        makeMask(mask,ccompSet,src_small.rows,src_small.cols);

        double src_width=src.cols, src_height=src.rows;
        int src_med_width = 1000;   //1000;512
        double rate = src_width/src_med_width;
        int src_med_height=src_height / rate;
        Mat src_med;
        resize(src,src_med,Size(src_med_width,src_med_height));  //把原图缩小

       //cvtColor(src_med,src_med,CV_GRAY2BGR);
        vector<barFeatures> barsFeatures;
        //char picName[10];

    for(int n=0;n<ccompSet.size();n++)//把mask放大,并和原图相与,以及进行其它操作
        {
                resize(mask[n],mask_large[n],Size(src_med_width,src_med_height));
                src_med.copyTo(regions[n], mask_large[n]);

                threshold(regions[n],regions[n],100,255,CV_THRESH_BINARY);
                //cvtColor(regions[n],regions[n],CV_BGR2GRAY);

                std::vector<std::vector<cv::Point>>contours;
                cv::findContours(regions[n],contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

                //----------检验轮廓效果----------------------
                //在白色图像上绘制黑色轮廓
                //if(n==0){
                //cv::Mat result(regions[n].size(),CV_8U,Scalar(255));
                //cv::drawContours(result,contours,-1,cv::Scalar(0),1);
                //imshow("qqq22",result);}
                //sprintf(picName,"c%d.png",n);
                //imwrite(picName,result);

                computeWhitebars(contours,n,barsFeatures,regions[n]); //计算并输出每个白条的长轴短轴比以及方向

                contoursRes.push_back(contours);

        }

        //原来输出的特征
        ofstream file2("results\\bars_feature.txt");
        for(int i=0;i<barsFeatures.size();i++)
        {
                file2<<barsFeatures[i].flag<<' '<<barsFeatures[i].duanZhou<<' '<<barsFeatures[i].xielv<<' '<<barsFeatures[i].x<<' '<<barsFeatures[i].y<<'\n';
        }
        file2.close();


        /*
        //特征改为长轴、短轴、斜率
        ofstream file2("bars_feature.txt");
        for(int i=0;i<barsFeatures.size();i++)
        {
                file2<<barsFeatures[i].flag<<' '<<barsFeatures[i].changZhou<<' '<<barsFeatures[i].duanZhou<<' '<<barsFeatures[i].xielv<<' '<<barsFeatures[i].x<<' '<<barsFeatures[i].y<<'\n';
        }
        file2.close();
        */

        return Size(src_med_width,src_med_height);
}



//对单独特定的区域进行分析
void singleRegionAnalyser(int nReg,double eps,int minPts,std::vector<std::vector<std::vector<cv::Point>>>& contoursRes,Size regSize,cv::Mat& result){

        ofstream file("results\\parameters.txt");
        file<<nReg<<' '<<eps<<' '<<minPts; //输出聚类参数
        file.close();

        system("python dbscan.py"); //运行python脚本
        //system("dbscan.exe");

        int nClass;
        int* label=new int[contoursRes[nReg].size()];
        char filename1[30],filename2[30];
        sprintf(filename1,"results\\nLabels%d.txt",nReg);
        sprintf(filename2,"results\\labels%d.txt",nReg);
        ifstream file2(filename1);
        ifstream file3(filename2);
        file2>>nClass;
        for(int i=0;i<contoursRes[nReg].size();i++){
                file3>>label[i];
        }
        int* count_singleClass=new int[nClass];

        //cout<<"num of class:"<<nClass<<endl;
        //在白色图像上绘制彩色轮廓
        RNG rng = theRNG();
        Vec3b* colorArr = new Vec3b[nClass];
        for(int i=0;i<nClass;i++){
                Vec3b newVal(rng(256), rng(256), rng(256));
                colorArr[i]=newVal;
                //cout<<'('<<int(colorArr[i][0])<<','<<int(colorArr[i][1])<<','<<int(colorArr[i][2])<<')';
        }


        //初始化count_singleClass
        for(int i=0;i<nClass;i++){
                count_singleClass[i]=0;
        }

        for(int i=0;i<contoursRes[nReg].size();i++){
                for(int j=-1;j<(-1+nClass);j++){
                        if(label[i]==j){
                                //cv::drawContours(result,contoursRes[nReg],i,Scalar(abs(j)*20%255,abs(j)*50%255,abs(j)*200%255),-1);
                                cv::drawContours(result,contoursRes[nReg],i,Scalar(colorArr[j+1][0],colorArr[j+1][1],colorArr[j+1][2]),-1);
                                count_singleClass[j+1]++;
                                //for(int n=0;n<contoursRes[nReg][i].size();n++){
                                 //circle(result,contoursRes[nReg][i][n],1,Scalar(abs(j)*20%255,abs(j)*50%255,abs(j)*200%255),-1);
                                //}
                                //break;
                        }
                }
        }

//        cout<<'\n';
//        for(int i=0;i<nClass;i++){
//                cout<<i<<" class:"<<count_singleClass[i]<<endl;
//        }

        char filename_pic[30];
        sprintf(filename_pic,"results\\classRes-%d.png",nReg);
        //imshow("1111111111",result);
        imwrite(filename_pic,result);
}
