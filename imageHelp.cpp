#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstdio>
void dealImageTempFunction();

void dealImageTempFunction() {
    // 加载分类
    cv::CascadeClassifier cascade;
    // 暂时只要人脸
    cascade.load("haarcascade_frontalface_alt2.xml");

    cv::Mat srcImage, grayImage , dstImage ,markImage;
    srcImage = cv::imread("./resource/3.PNG");
    cv::imshow("原图",srcImage);
    cv::waitKey(0); // 0的意思是保持等待 直到键盘输入任意数字
    cv::destroyAllWindows();
    cv::cvtColor(srcImage,grayImage,cv::COLOR_RGB2GRAY);// 生成灰度图

    std::vector<cv::Rect> rect;
    // minNeighbors 3 类似多人检测
    // cascade.detectMultiScale(grayImage, rect, 1.1, 3,0);
    cv::imshow("【人脸识别detectMultiScale】", grayImage);
    cv::waitKey(0);
    std::cout << "--------*****----------" << std::endl;
    cv::destroyAllWindows();
    cascade.detectMultiScale(grayImage,rect,1.1,3);
    // 识别已经结束了
    // rect 其实就是识别出的框 在图片中的坐标 https://petergao.blog.csdn.net/article/details/127123997?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EYuanLiJiHua%7EPosition-2-127123997-blog-79009825.pc_relevant_multi_platform_whitelistv3&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EYuanLiJiHua%7EPosition-2-127123997-blog-79009825.pc_relevant_multi_platform_whitelistv3&utm_relevant_index=5
    // 
    // 
    for (int i = 0; i < rect.size(); i++)
    {
        cv::rectangle(srcImage,rect[i],cv::Scalar(0,0,255));
    }
    

    cv::imshow("【人脸识别detectMultiScale】", srcImage);
    cv::waitKey(0);
    std::cout << "--------*****----------" << std::endl;
    cv::destroyAllWindows();


    // 读取水印
    // 水印即为带有Alpha 通道的图片
    // markImage = cv::imread("./resource/XAY.png",cv::IMREAD_UNCHANGED);
    // cv::Mat imageROI = srcImage(cv::Rect(450, 20, markImage.cols, markImage.rows)); 
    
    // markImage.copyTo(imageROI);
    // cv::imshow("【人脸识别detectMultiScale】", srcImage);
    // cv::waitKey(0);
    // std::cout << "--------*****----------" << std::endl;
    // cv::destroyAllWindows();
}
int main () {

    dealImageTempFunction();
    return 0;
}