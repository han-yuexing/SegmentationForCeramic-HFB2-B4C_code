#include "func.h"
#include<queue>
// 排除同方向同区域较少的线。然后选择同方向与同区域中与白条相交最少的线
using namespace std;
using namespace cv;


struct LinkList{
        cv::Point start; //直线的端点1
        cv::Point end; //直线的端点2
        int label; //直线是否被分类
        //int point_flag; //如果有交点的话，指明哪个端点是交点，1代表start点；2代表end点
        double rate; //直线长度和经过白条数之比
        LinkList *next;
};



double computeRate(Mat src_binary, cv::Point pt_start, cv::Point pt_end) { ////计算一条线经过的白条数和线长的比
        cv::LineIterator lit(src_binary, pt_start, pt_end, 8); //遍历直线上的每一个点
        double count = 0, sum = 0, rate = 0;
        for (int i = 0; i<lit.count; i++, ++lit) {
                cv::Point pt(lit.pos());
                if (src_binary.at<uchar>(pt.y, pt.x) > 0) { //假如直线和白条有交点
                        count++; //交点个数
                }
                sum++; //直线上的总点数
        }

        rate = count / sum;
        return rate;
        //cout << rate << ',';
}


double computeJuLi(cv::Point itr_start,cv::Point itr_end,cv::Point itrZhun_start,double itrZhunXie){  //计算两线段的距离(一个线段的中点到另一个线段的距离)
        double mid_x = double(itr_start.x + itr_end.x) / 2;
        double mid_y = double(itr_start.y + itr_end.y) / 2;

        //线段itrZhun的直线方程参数
        double b = itrZhun_start.y - itrZhunXie * itrZhun_start.x;

        //中点到itrZhun的距离
        double juli = abs(itrZhunXie*mid_x + b - mid_y)/sqrt(pow(itrZhunXie,2)+1);
        return juli;
}


//建立lines的链表
LinkList *BuildList(vector<Vec4i>Lines,Mat src_binary){
        LinkList *head = NULL;
        LinkList *end = NULL;
    vector<Vec4i>::const_iterator itr = Lines.begin()+1;

        for (; itr != Lines.end();++itr)
        {
                LinkList *new_node = (LinkList*)malloc(sizeof(LinkList));
                new_node->start.x = (*itr)[0];
                new_node->start.y = (*itr)[1];
                new_node->end.x = (*itr)[2];
                new_node->end.y = (*itr)[3];
                new_node->label = 0;
                //new_node->point_flag=0;
                new_node->rate = computeRate(src_binary,new_node->start,new_node->end);

                if(head == NULL){
                        head = new_node;
                        end = head;
                }else{
                        new_node->next = NULL;
                        end->next = new_node;
                        end = new_node;
                }
        }

        return head;

}


//判断链表元素是否全部被分类即label是否都不等于0
int Length(LinkList* head){
        int length = 0;
        LinkList* p = head;
        while(p){
                if(p->label == 0){
                    ++length;
                }
                p = p->next;
        }

        return length;
}


//删除链表结点
LinkList* DeleteByNode(LinkList* node,LinkList* head){
        if(head == NULL)
                return head;

        if(head == node){
                head = head->next;
        }else{
                LinkList *p=head;
                while(p->next != NULL && p->next != node)
                        p = p->next;
                if(p->next != NULL)
                        p->next = node->next;
        }

        if(node != NULL){
                free(node);
                node = NULL;
        }

        return head;
}

//找到链表中第一个label为0的结点
LinkList* FindByNode(LinkList* head){
        //if(head == NULL)
                //return head;

        LinkList *p=head;
        while(p != NULL)
        {
                if(p->label == 0)
                        return p;
                p=p->next;
        }
}

bool neighborXielv(Vec4i LineFinalTemp,Mat src_binary){
        int roi_col=100,roi_row=100;

        int center_x = (LineFinalTemp[0]+LineFinalTemp[2])/2;
        int center_y = (LineFinalTemp[1]+LineFinalTemp[3])/2;

        int roi_x = center_x-50;
        int roi_y = center_y-50;

        if(roi_x<0){roi_x=0;}
        if(roi_y<0){roi_y=0;}
        if((roi_x+roi_col)>(src_binary.cols-1)){roi_col=src_binary.cols-1-roi_x;}
        if((roi_y+roi_row)>(src_binary.rows-1)){roi_row=src_binary.rows-1-roi_y;}

        Mat roi = src_binary(Rect(roi_x,roi_y,roi_col,roi_row));

        double line_xie,mean_xie,bai_xie;
        //直线斜率
        line_xie = double(LineFinalTemp[3] - LineFinalTemp[1]) / double(LineFinalTemp[2] - LineFinalTemp[0]+0.000001);

        std::vector<std::vector<cv::Point>>contours;
        cv::findContours(roi,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

    //遍历所有轮廓
        std::vector<std::vector<cv::Point>>::const_iterator itc=contours.begin();

        //删除太小的轮廓
        while(itc!=contours.end()){
         if(itc->size()<10) //??
                itc=contours.erase(itc);
         else
                ++itc;
    }

        //cv::Mat result(roi.size(),CV_8U,Scalar(255));
        //cv::drawContours(result,contours,-1,cv::Scalar(0),1);

        double sum=0;
        int num =0;
        itc=contours.begin();
        while(itc!=contours.end()){
                RotatedRect box = minAreaRect(cv::Mat(*itc));
                //计算白条斜率
                Point2f vtx[4];
                box.points(vtx);

                if(sqrt(pow((vtx[0].x-vtx[1].x),2)+pow((vtx[0].y-vtx[1].y),2)) > sqrt(pow((vtx[0].x-vtx[3].x),2)+pow((vtx[0].y-vtx[3].y),2)))
                {
                        bai_xie = double(vtx[0].y-vtx[1].y)/double(vtx[0].x - vtx[1].x+0.000001);
                }else{
                        bai_xie = double(vtx[0].y-vtx[3].y)/double(vtx[0].x - vtx[3].x+0.000001);
                }


                if(bai_xie>5)
                        bai_xie=5;
                else if(bai_xie<-5)
                        bai_xie=-5;

                sum+=bai_xie;
                num++;
                ++itc;
        }

        mean_xie = sum/num;
        if ((abs(line_xie - mean_xie) < 1)  || (line_xie>3 && mean_xie>3)|| (line_xie<-3 && mean_xie<-3)){ //....................
                return false;
        }else{

                return true;
        }

}

int* deleteXianDuan(vector<Vec4i>& LinesFinal,int rows,int cols){
        cv::Point pt_start, pt_end;
        int* flag = new int[LinesFinal.size()];//如果直线有交点的话，指明哪个端点是交点，0代表没有交点；1代表start点；2代表end点

        for(int i=0;i<LinesFinal.size();i++)
        {
                flag[i]=0;
                Mat tempImg(rows,cols,CV_8UC1,Scalar::all(0));
                for(int j=0;j<LinesFinal.size();j++)
                {
                        if(j!=i){
                                line(tempImg,Point(LinesFinal[j][0],LinesFinal[j][1]),Point(LinesFinal[j][2],LinesFinal[j][3]),Scalar::all(255),1,8);
                        }
                }

                pt_start.x = LinesFinal[i][0];
                pt_start.y = LinesFinal[i][1];
                pt_end.x = LinesFinal[i][2];
                pt_end.y = LinesFinal[i][3];

                cv::LineIterator lit1(tempImg,pt_start,pt_end,8); //从头到尾遍历直线上的每一个点
                cv::LineIterator lit2(tempImg,pt_end,pt_start,8); //从尾到头遍历直线上的每一个点
                double count1 = 0, count2 = 0; //线段长度
                cv::Point pt1,pt2;
                int label=0;

                for(int i=0;i<lit1.count;i++,++lit1){ //从头到尾遍历
                        pt1.y=lit1.pos().y;
                        pt1.x=lit1.pos().x;
                        count1++;
                        if(tempImg.at<uchar>(pt1.y,pt1.x) > 0){ //假如直线和其它直线有交点
                                count1++;
                                label=1; //表示有交点
                                break;
                        }
                }

                for(int i=0;i<lit2.count;i++,++lit2){ //从尾到头遍历
                        pt2.y=lit2.pos().y;
                        pt2.x=lit2.pos().x;
                        count2++;
                        if(tempImg.at<uchar>(pt2.y,pt2.x) > 0){ //假如直线和其它直线有交点
                                count2++;
                                label=1; //表示有交点
                                break;
                        }
                }

                //与其它直线没有交点
                if(label==0){
                        continue;
                }

                if(count1>count2){ //end点要变   //这个还需改进(应该是留下最长的线段？？)
                        LinesFinal[i][2]=pt2.x;
                    LinesFinal[i][3]=pt2.y;
                        flag[i]=2;
                }else{ //start点变
                        LinesFinal[i][0]=pt1.x;
                    LinesFinal[i][1]=pt1.y;
                        flag[i]=1;
                }

        }

        return flag;
}



void connectXianDuan1(vector<Vec4i>& LinesFinal,int rows,int cols,int* flag,Mat& tempImg,vector<Vec4i>& LinesFinalFinal,Mat src_binary,double rateThre){
        cv::Point pt_start, pt_end;
        Vec4i checkTemp; //..........
        //先延长两端都没有交点的直线
        for(int i=0;i<LinesFinal.size();i++)
        {
                cv::Point pa,pb;
                if(flag[i]==0)
                {
                        //checkTemp=LinesFinal[i];//.......
                        checkTemp[0] = LinesFinal[i][0];
                        checkTemp[1] = LinesFinal[i][1];
                        checkTemp[2] = LinesFinal[i][2];
                        checkTemp[3] = LinesFinal[i][3];

                        pt_start.x = LinesFinal[i][0];
                        pt_start.y = LinesFinal[i][1];
                        pt_end.x = LinesFinal[i][2];
                        pt_end.y = LinesFinal[i][3];

                        //先保证start点在end点之上
                        if(pt_start.y>pt_end.y)//swap
                        {
                                pa=pt_start;
                                pt_start=pt_end;
                                pt_end=pa;
                        }

                        double k=(pt_end.y-pt_start.y)/(pt_end.x-pt_start.x+0.000001);
                        double h=rows,w=cols;

                        //延长start端
                        pb=pt_start;
                        pa.y=0;
                        pa.x=pt_start.x-pt_start.y/k;
                        if(pa.x<0){
                                pa.x=0;
                                pa.y=pt_start.y-k*pt_start.x;
                        }else if(pa.x>=w){
                                pa.x=w;
                                pa.y=pt_start.y+k*(w-pt_start.x);
                        }
                        //LinesFinal[i][0]=pa.x;
                        //LinesFinal[i][1]=pa.y;
                        checkTemp[0]=pa.x;
                        checkTemp[1]=pa.y;

                        cv::LineIterator lit1(tempImg,pb,pa,8); //从pb到pa遍历延长线上的每一个点
                        cv::Point pt1;
                        lit1++;//................................................................
                        for(int j=0;j<lit1.count-1;j++,++lit1){ //遍历
                                pt1.y=lit1.pos().y;
                                pt1.x=lit1.pos().x;
                                if(tempImg.at<uchar>(pt1.y,pt1.x) > 0){ //假如直线和其它直线有交点
                                        //LinesFinal[i][0]=pt1.x;
                                        //LinesFinal[i][1]=pt1.y;
                                        checkTemp[0]=pt1.x;
                                        checkTemp[1]=pt1.y;
                                        break;
                                }
                        }

                        //延长end端
                        pb=pt_end;
                        pa.y=h;
                        pa.x=pt_end.x+(h-pt_end.y)/k;
                        if(pa.x>w){
                                pa.x=w;
                                pa.y=pt_end.y+k*(w-pt_end.x);
                        }else if(pb.x<0){
                                pa.x=0;
                                pa.y=pt_end.y-k*pt_end.x;
                        }
                        //LinesFinal[i][2]=pa.x;
                        //LinesFinal[i][3]=pa.y;
                        checkTemp[2]=pa.x;
                        checkTemp[3]=pa.y;

                        cv::LineIterator lit2(tempImg,pb,pa,8); //从pb到pa遍历延长线上的每一个点
                        cv::Point pt2;
                        lit2++; //........................................
                        for(int j=0;j<lit2.count-1;j++,++lit2){ //遍历
                                pt2.y=lit2.pos().y;
                                pt2.x=lit2.pos().x;
                                if(tempImg.at<uchar>(pt2.y,pt2.x) > 0){ //假如直线和其它直线有交点
                                        //LinesFinal[i][2]=pt2.x;
                                        //LinesFinal[i][3]=pt2.y;
                                        checkTemp[2]=pt2.x;
                                        checkTemp[3]=pt2.y;
                                        break;
                                }
                        }

                        double rateTemp = computeRate(src_binary,Point(checkTemp[0],checkTemp[1]),Point(checkTemp[2],checkTemp[3]));
                        if(rateTemp<rateThre){
                                LinesFinalFinal.push_back(checkTemp);
                        line(tempImg,Point(checkTemp[0],checkTemp[1]),Point(checkTemp[2],checkTemp[3]),Scalar::all(255),1,8);
                        }else{
                                LinesFinalFinal.push_back(LinesFinal[i]);
                        line(tempImg,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar::all(255),1,8);
                        }
                }
        }

}

void connectXianDuan2(vector<Vec4i>& LinesFinal,int rows,int cols,int* flag,Mat& tempImg,vector<Vec4i>& LinesFinalFinal,Mat src_binary,double rateThre){
        cv::Point pt_start, pt_end;
        Vec4i checkTemp;
        //然后延长两端有交点的直线
        for(int i=0;i<LinesFinal.size();i++)
        {
                cv::Point pa,pb;
                if(flag[i]==1) //延长end点
                {
                        //checkTemp = LinesFinal[i]; //................
                        checkTemp[0] = LinesFinal[i][0];
                        checkTemp[1] = LinesFinal[i][1];
                        checkTemp[2] = LinesFinal[i][2];
                        checkTemp[3] = LinesFinal[i][3];

                        pt_start.x = LinesFinal[i][0];
                        pt_start.y = LinesFinal[i][1];
                        pt_end.x = LinesFinal[i][2];
                        pt_end.y = LinesFinal[i][3];

                        double k=(pt_end.y-pt_start.y)/(pt_end.x-pt_start.x+0.000001);
                        double h=rows,w=cols;

                        pb=pt_end;
                        if(pt_start.y>pt_end.y){//假如end在start之上
                                pa.y=0;
                        }else{ //假如end在start之下
                                pa.y=h;
                        }
                        pa.x=pb.x+(pa.y-pb.y)/k;
                        if(pa.x<0){
                                pa.x=0;
                                pa.y=pb.y+k*(pa.x-pb.x);
                        }else if(pa.x>=w){
                                pa.x=w;
                                pa.y=pb.y+k*(pa.x-pb.x);
                        }
                        //LinesFinal[i][2]=pa.x;
                        //LinesFinal[i][3]=pa.y;
                        checkTemp[2]=pa.x;
                        checkTemp[3]=pa.y;


                        cv::LineIterator lit2(tempImg,pb,pa,8); //从pb到pa遍历延长线上的每一个点
                        cv::Point pt1;
                        lit2++;//...................................................
                        for(int j=0;j<lit2.count-1;j++,++lit2){ //遍历
                                pt1.y=lit2.pos().y;
                                pt1.x=lit2.pos().x;
                                if(tempImg.at<uchar>(pt1.y,pt1.x) > 0){ //假如直线和其它直线有交点
                                        //LinesFinal[i][2]=pt1.x;
                                        //LinesFinal[i][3]=pt1.y;
                                        checkTemp[2]=pt1.x;
                                        checkTemp[3]=pt1.y;
                                        break;
                                }
                        }
                        double rateTemp = computeRate(src_binary,Point(checkTemp[0],checkTemp[1]),Point(checkTemp[2],checkTemp[3]));
                        if(rateTemp<rateThre){
                                LinesFinalFinal.push_back(checkTemp);
                        line(tempImg,Point(checkTemp[0],checkTemp[1]),Point(checkTemp[2],checkTemp[3]),Scalar::all(255),1,8);
                    }else{
                                LinesFinalFinal.push_back(LinesFinal[i]);
                        line(tempImg,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar::all(255),1,8);
                        }
                        //line(tempImg,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar::all(255),1,8);

                }else if(flag[i]==2)//延长start点
                {
                        //checkTemp = LinesFinal[i];//...........
                        checkTemp[0] = LinesFinal[i][0];
                        checkTemp[1] = LinesFinal[i][1];
                        checkTemp[2] = LinesFinal[i][2];
                        checkTemp[3] = LinesFinal[i][3];

                        pt_start.x = LinesFinal[i][0];
                        pt_start.y = LinesFinal[i][1];
                        pt_end.x = LinesFinal[i][2];
                        pt_end.y = LinesFinal[i][3];

                        double k=(pt_end.y-pt_start.y)/(pt_end.x-pt_start.x+0.000001);
                        double h=rows,w=cols;

                        pb=pt_start;
                        if(pt_end.y>pt_start.y){//假如start在end之上
                                pa.y=0;
                        }else{ //假如start在end之下
                                pa.y=h;
                        }
                        pa.x=pb.x+(pa.y-pb.y)/k;
                        if(pa.x<0){
                                pa.x=0;
                                pa.y=pb.y+k*(pa.x-pb.x);
                        }else if(pa.x>=w){
                                pa.x=w;
                                pa.y=pb.y+k*(pa.x-pb.x);
                        }
                        //LinesFinal[i][0]=pa.x;
                        //LinesFinal[i][1]=pa.y;
                        checkTemp[0]=pa.x;
                        checkTemp[1]=pa.y;


                        cv::LineIterator lit3(tempImg,pb,pa,8); //从pb到pa遍历延长线上的每一个点
                        cv::Point pt1;
                        lit3++;//.........................................................
                        for(int j=0;j<lit3.count-1;j++,++lit3){ //遍历
                                pt1.y=lit3.pos().y;
                                pt1.x=lit3.pos().x;
                                if(tempImg.at<uchar>(pt1.y,pt1.x) > 0){ //假如直线和其它直线有交点
                                        //LinesFinal[i][0]=pt1.x;
                                        //LinesFinal[i][1]=pt1.y;
                                        checkTemp[0]=pt1.x;
                                        checkTemp[1]=pt1.y;
                                        break;
                                }
                        }
                        double rateTemp = computeRate(src_binary,Point(checkTemp[0],checkTemp[1]),Point(checkTemp[2],checkTemp[3]));
                        if(rateTemp<rateThre){
                                LinesFinalFinal.push_back(checkTemp);
                        line(tempImg,Point(checkTemp[0],checkTemp[1]),Point(checkTemp[2],checkTemp[3]),Scalar::all(255),1,8);
                    }else{
                                LinesFinalFinal.push_back(LinesFinal[i]);
                        line(tempImg,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar::all(255),1,8);
                        }
                        //line(tempImg,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar::all(255),1,8);
                }
        }
}



//--------------------------------------------------------------------------------------------------floodfill_排除掉block面积过小的
int newfloodfill(Mat &_image, Mat &_mask,
        Point seedPoint,
        int diff, int range_area, vector<ConnectedComp> &ccompSet)
{
    std::queue<cv::Point> q;
    q.push(seedPoint);
    _mask.at<uchar>(seedPoint.y, seedPoint.x) = 255;

    ConnectedComp ccompSingle;
    int count_area = 0;

    while (!q.empty()) {
        Point p_temp = q.front();
        ccompSingle.pointSet.push_back(p_temp);
        count_area++;

        q.pop();

        if ((p_temp.y-1>=0) && abs((int(_image.at<uchar>(p_temp.y - 1, p_temp.x))) - (int(_image.at<uchar>(p_temp.y, p_temp.x)))) <= diff  && _mask.at<uchar>(p_temp.y - 1, p_temp.x) == 0)
        {
            q.push(Point(p_temp.x, p_temp.y - 1));
            _mask.at<uchar>(p_temp.y - 1, p_temp.x) = 255;
        }
        if ((p_temp.y + 1 < _image.rows) && abs((int(_image.at<uchar>(p_temp.y + 1, p_temp.x))) - (int(_image.at<uchar>(p_temp.y, p_temp.x)))) <= diff  && _mask.at<uchar>(p_temp.y + 1, p_temp.x) == 0)
        {
            q.push(Point(p_temp.x, p_temp.y + 1));
            _mask.at<uchar>(p_temp.y + 1, p_temp.x) = 255;
        }
        if ((p_temp.x - 1 >= 0) && abs((int(_image.at<uchar>(p_temp.y, p_temp.x - 1))) - (int(_image.at<uchar>(p_temp.y, p_temp.x)))) <= diff  && _mask.at<uchar>(p_temp.y, p_temp.x - 1) == 0)
        {
            q.push(Point(p_temp.x - 1, p_temp.y));
            _mask.at<uchar>(p_temp.y, p_temp.x - 1) = 255;
        }
        if ((p_temp.x + 1 < _image.cols) && abs((int(_image.at<uchar>(p_temp.y, p_temp.x + 1))) - (int(_image.at<uchar>(p_temp.y, p_temp.x)))) <= diff  && _mask.at<uchar>(p_temp.y, p_temp.x + 1) == 0)
        {
            q.push(Point(p_temp.x + 1, p_temp.y));
            _mask.at<uchar>(p_temp.y, p_temp.x + 1) = 255;
        }

    }

    ccompSingle.area = count_area;

    if (ccompSingle.area>range_area) //面积大于阈值
            ccompSet.push_back(ccompSingle);

    return cvRound(ccompSingle.area);
}


void floodfillPaint(Mat &_result, vector<ConnectedComp> &ccompSet) { //画floodfill结果
        RNG rng = theRNG();

        for (int n = 0; n<ccompSet.size(); n++) {
                Vec3b newVal(rng(256), rng(256), rng(256));
                for (int i = 0; i<ccompSet[n].pointSet.size(); i++) { //画floodfill结果
                        _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y, ccompSet[n].pointSet[i].x) = newVal;
                }
        }
}



void linesFilter2(Mat src_binary, vector<Vec4i>Lines, Mat &line_img, vector<ConnectedComp> &ccompSet,int sigma_1,double sigma_2,int sigma_3,double sigma_4) {

        vector<Vec4i>LinesFinal;

        LinkList* headLines = BuildList(Lines,src_binary); //将lines转换为链表存储

        while (Length(headLines))
        {
                int count=0;
                LinkList* itrZhun = FindByNode(headLines); //以第一条线为基准
                LinkList* itr = itrZhun;

                while(itr != NULL) //从第一条线开始，比较两条线的斜率、位置、经过的白条数
                {
                        if(itr->label == 0){
                            cv::Point itr_start, itr_end, itrZhun_start, itrZhun_end; //两条线的起点和端点
                            itr_start.x = itr->start.x;
                            itr_start.y = itr->start.y;
                            itr_end.x = itr->end.x;
                                itr_end.y = itr->end.y;
                                itrZhun_start.x = itrZhun->start.x;
                                itrZhun_start.y = itrZhun->start.y;
                                itrZhun_end.x = itrZhun->end.x;
                                itrZhun_end.y = itrZhun->end.y;

                                //算斜率
                                double itrXie, itrZhunXie;
                                itrXie = double(itr_end.y-itr_start.y)/double(itr_end.x - itr_start.x+0.000001);
                                itrZhunXie = double(itrZhun_end.y - itrZhun_start.y) / double(itrZhun_end.x - itrZhun_start.x+0.000001);

                                //算两线段的距离(一个线段的中点到另一个线段的距离)
                                //double juli;
                                //double deltaY = double(itr_start.y + itr_end.y) / 2 - double(itrZhun_start.y + itrZhun_end.y) / 2;
                                //double deltaX = double(itr_start.x + itr_end.x) / 2 - double(itrZhun_start.x + itrZhun_end.x) / 2;
                                //juli = sqrt(pow(deltaX,2) + pow(deltaX, 2));

                                double juli = computeJuLi(itr_start,itr_end,itrZhun_start,itrZhunXie);


                                //cout << "[("<<itrXie<<','<<itr->rate<<")("<<itrZhunXie<<','<<itrZhun->rate<<"),"<<juli<<"]\n";


                                if ((abs(itrXie - itrZhunXie) < 0.3  || (itrXie>3 && itrZhunXie>3)|| (itrXie<-3 && itrZhunXie<-3)) && juli < sigma_1) {//如果两条线相近,0.3，50(还要考虑斜率数值大的情况)10
                                        if (itr->rate < itrZhun->rate) {//假如一条线的rate小于基准线的rate
                                                itrZhun = itr;
                                                itr->label = 1;
                                                itr = itr->next;
                                        }
                                        else {
                                                itr->label = 1;
                                                itr = itr->next;
                                        }
                                        count++;
                                }else{
                                        itr = itr->next;
                                }

                        }else{
                                itr = itr->next;
                        }

                }

                if(count>sigma_3 && itrZhun->rate<sigma_2){  //假如同一类的线大于一定数目(5)并且rate小于一个阈值15，0.1；1，0.2
                        Vec4i LineFinalTemp;
                        LineFinalTemp[0] = itrZhun->start.x;
                        LineFinalTemp[1] = itrZhun->start.y;
                        LineFinalTemp[2] = itrZhun->end.x;
                        LineFinalTemp[3] = itrZhun->end.y;
                        if(count>30 || neighborXielv(LineFinalTemp,src_binary)){  //线和周围白条对比斜率
                                LinesFinal.push_back(LineFinalTemp);
                        }
                        //cout<<"("<<count<<','<<itrZhun->rate<<')';
                }


                //cout << '\n';
        }


        int* flag=deleteXianDuan(LinesFinal,line_img.rows,line_img.cols); //删除线头ar
        vector<Vec4i>LinesFinalFinal;

        Mat tempImg(line_img.rows,line_img.cols,CV_8UC1,Scalar::all(0));
        for(int i=0;i<LinesFinal.size();i++)
        {
                line(tempImg,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar::all(255),1,8);
        }


        //imshow("linim",tempImg);

        double rateThre=sigma_4; //0.5;0.2
        connectXianDuan1(LinesFinal,line_img.rows,line_img.cols,flag,tempImg,LinesFinalFinal,src_binary,rateThre); //延长两端都没交点的直线(还要加阈值参数)
        connectXianDuan2(LinesFinal,line_img.rows,line_img.cols,flag,tempImg,LinesFinalFinal,src_binary,rateThre); //延长有交点的直线

        //deleteXianDuan(LinesFinal,line_img.rows,line_img.cols);//再删一次?? 加到延长里
        /*
        vector<Vec4i>LinesFinalFinal;
        for(int i=0;i<LinesFinal.size();i++){
                double rateTemp = computeRate(src_binary,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]));
                if(rateTemp<0.2){
                        line(line_img,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar(0,0,255),2,8);
                }
        }
        */


        //imshow("qqw",tempImg);


        //根据边界提取区域
        float area_block = src_binary.rows * src_binary.cols;
        int range_area=area_block * 0.01;  //用于抛弃过小的区域

        Mat mask;
        tempImg.copyTo(mask);

    for( int y = 1; y < tempImg.rows-1; y++ )
    {
        for( int x = 1; x < tempImg.cols-1; x++ )
        {
            if( mask.at<uchar>(y, x) == 0 )  //非0处即为1，表示已经经过填充，不再处理
            {
                                int s=newfloodfill(tempImg, mask, Point(x,y), 5 ,range_area,ccompSet);
                                //cout<<s;

            }
        }
    }
        //画floodfill结果-----------------------------------
        Mat result_floodfill(tempImg.rows, tempImg.cols, CV_8UC3, Scalar::all(0) );
        floodfillPaint(result_floodfill,ccompSet);
        //imshow("floodfill_result",result_floodfill);
        //imwrite("flood2.png",result_floodfill);


        /*
        for(int i=0;i<LinesFinal.size();i++)
        {
                line(line_img,Point(LinesFinal[i][0],LinesFinal[i][1]),Point(LinesFinal[i][2],LinesFinal[i][3]),Scalar(0,0,255),1,8);
        }
        */

        for(int i=0;i<LinesFinalFinal.size();i++)
        {
                line(line_img,Point(LinesFinalFinal[i][0],LinesFinalFinal[i][1]),Point(LinesFinalFinal[i][2],LinesFinalFinal[i][3]),Scalar(0,0,255),2,8);
        }

}
