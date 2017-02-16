#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

Mat raw2Mat(int);
int getChoice();
Mat cannyDector(Mat);
void createGaussianKernel();

void  createGaussianKernel()
{
    int widthOfMask = 0;
    float sigma = .0;
    
    sigma = 1;
    widthOfMask = int((sigma-0.01)*3)*2+1;
    printf("width is %d\n", widthOfMask);
    
    
}

int main(int argc, char** argv)
{
    int imgChoice;
    char owndName[] = "Original Image", t1wndName[] = "Temp Image" ;
    Mat oriImage, tmp1Image;
    
    createGaussianKernel();
    
    imgChoice = getChoice();
    oriImage = raw2Mat(imgChoice);
    
    tmp1Image = oriImage.clone();
    tmp1Image = cannyDector(tmp1Image);
    
    imshow(owndName, oriImage);
    imshow(t1wndName, tmp1Image);
    
    waitKey();
    return 0;
}

Mat cannyDector(Mat tmpImg)
{
    int t=0;
    for(int i = 0; i < tmpImg.rows; i++){
        for(int j = 0; j < tmpImg.cols; j++){
            t++;
            //printf("test: %d", tmpImg.at<uchar>(i,j));
        }
    }
    
    printf("Total is %d\n", t);
    //printf("test: %d", tmpImg.at<uchar>(10,20));
    Canny(tmpImg, tmpImg, 40, 80);
    return tmpImg;
}

int getChoice()
{
    int inputNumber;
    printf(">>>> 1.Cana 2.Fruit 3.Img335 4.Leap 5.Leaf <<<<\n");
    printf("Please input your choice number and press Enter: ");
    scanf("%d", &inputNumber);
    return inputNumber;
}

Mat raw2Mat(int choice)
{
    int IMAGE_WIDTH, IMAGE_HEIGHT, imgSize;
    char *rawData = NULL;
    FILE *fp = NULL;
    
    switch (choice) {
        case 1:
            printf("1.cana.raw is selected.\n");
            IMAGE_WIDTH = 512;
            IMAGE_HEIGHT = 479;
            //Open raw image.
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDector/CED/raw/cana.raw", "rb");
            break;
            
        case 2:
            printf("2.fruit.raw is selected.\n");
            IMAGE_WIDTH = 487;
            IMAGE_HEIGHT = 414;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDector/CED/raw/fruit.raw", "rb");
            break;
            
        case 3:
            printf("3.img335.raw is selected.\n");
            IMAGE_WIDTH = 500;
            IMAGE_HEIGHT = 335;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDector/CED/raw/img335.raw", "rb");
            break;
            
        case 4:
            printf("4.lamp.raw is selected.\n");
            IMAGE_WIDTH = 256;
            IMAGE_HEIGHT = 256;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDector/CED/raw/lamp.raw", "rb");
            break;
            
        case 5:
            printf("5.leaf.raw is selected.\n");
            IMAGE_WIDTH = 190;
            IMAGE_HEIGHT = 243;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDector/CED/raw/leaf.raw", "rb");
            break;
            
        default:
            printf("Please select correct image.\n");
            break;
    }
    
    if (fp == NULL) printf("Open Raw Data Fail!\n");
    
    imgSize = IMAGE_HEIGHT * IMAGE_WIDTH;
    Mat rawImage = Mat::zeros(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    
    //Memory allocation for raw image data buffer.
    rawData = (char*) malloc (sizeof(char) * imgSize);
    
    //Read image data and store in buffer.
    fread(rawData, sizeof(char), imgSize, fp);
    memcpy(rawImage.data, rawData, imgSize);

    free(rawData);
    fclose(fp);
    
    return rawImage;
}
