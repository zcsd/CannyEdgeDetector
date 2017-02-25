#include <stdio.h>
#include <stdlib.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

int getChoice();
Mat raw2Mat(int);
void createGaussianKernel();
void cannyDector();
void useGaussianBlur();
void useSobelDerivat();
void nonMaxSuppress();
void hysteresisThreshold(int, int);
Mat combineImage();
void createLapGaussianKernel();

Mat oriImage, bluredImage, edgeMagImage, edgeAngImage, thinEdgeImage, thresholdImage;
int *gaussianMask, maskRad, maskWidth = 0, maskSum = 0;
float sigma = 0.0;

int main(int argc, char** argv)
{
    int imgChoice;
    char wndName[] = "Canny Process";
    Mat combinedImage;
    
    printf(">>>>         Canny Edge Detector           <<<<\n");

    bool isNewImage = true;
    while (isNewImage)
    {
        isNewImage = false;
        
        imgChoice = getChoice();
        oriImage = raw2Mat(imgChoice);
    
        bool isNewSigma = true;
        while (isNewSigma)
        {
            isNewSigma = false;
            createGaussianKernel();
            cannyDector();
        
            combinedImage = combineImage();
        
            imshow(wndName, combinedImage);
            waitKey(10);
        
            char tryNewSigma;
            printf("Do you want to try other sigma?(Y/N): ");
            scanf("%s", &tryNewSigma);
            if (tryNewSigma == 'y' || tryNewSigma == 'Y') {
                isNewSigma = true;
                printf("\n-------------Please Try Another Sigma-------------\n");
                destroyWindow(wndName);
                combinedImage.release();
            }
        
            free(gaussianMask);
            bluredImage.setTo(Scalar(0));
            edgeMagImage.setTo(Scalar(0));
            edgeAngImage.setTo(Scalar(0));
            thinEdgeImage.setTo(Scalar(0));
            thresholdImage.setTo(Scalar(0));
            sigma = 0.0;
            maskRad = 0;
            maskWidth = 0;
            maskSum = 0;
        }
        char tryNewImage;
        printf("Do you want to try another image?(Y/N): ");
        scanf("%s", &tryNewImage);
        if (tryNewImage == 'y' || tryNewImage == 'Y') {
            isNewImage = true;
            printf("\n>>>>>>>>>---------Please Try Another Image--------<<<<<<<<<<\n");
        }
        oriImage.setTo(Scalar(0));
    }
    printf("-------Program End-------\n");
    return 0;
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
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDetector/CED/raw/cana.raw", "rb");
            break;
            
        case 2:
            printf("2.fruit.raw is selected.\n");
            IMAGE_WIDTH = 487;
            IMAGE_HEIGHT = 414;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDetector/CED/raw/fruit.raw", "rb");
            break;
            
        case 3:
            printf("3.img335.raw is selected.\n");
            IMAGE_WIDTH = 500;
            IMAGE_HEIGHT = 335;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDetector/CED/raw/img335.raw", "rb");
            break;
            
        case 4:
            printf("4.lamp.raw is selected.\n");
            IMAGE_WIDTH = 256;
            IMAGE_HEIGHT = 256;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDetector/CED/raw/lamp.raw", "rb");
            break;
            
        case 5:
            printf("5.leaf.raw is selected.\n");
            IMAGE_WIDTH = 190;
            IMAGE_HEIGHT = 243;
            fp = fopen("/Users/zichun/Documents/Assignment/CannyEdgeDetector/CED/raw/leaf.raw", "rb");
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

void createGaussianKernel()
{
    printf("Please input standard deviation(>0) and press Enter: ");
    scanf("%f", &sigma);
    if(sigma < 0.01) sigma = 0.01;
    //computer mask width according to sigma value
    maskWidth = int((sigma - 0.01) * 3) * 2 + 1;
    if(maskWidth < 1)   maskWidth = 1;
    printf("Sigma is %.2f, Mask Width is %d.\n", sigma, maskWidth);
    
    gaussianMask = (int*)malloc(maskWidth * maskWidth * sizeof(int));
    
    double gaussianMaskDou[maskWidth][maskWidth], maskMin = 0.0;
    int gaussianMaskInt[maskWidth][maskWidth];
    
    maskRad = maskWidth / 2;
    int i, j;
    //construct the gaussian mask
    for(int x = - maskRad; x <= maskRad; x++)
    {
        for (int y = -maskRad; y <= maskRad; y++)
        {
            i = x + maskRad;
            j = y + maskRad;
            //gaussian 2d function
            gaussianMaskDou[i][j] = exp( (x*x + y*y) / (-2*sigma*sigma) );
            //min value of mask is the first one
            if(i == 0 && j == 0)  maskMin = gaussianMaskDou[0][0];
            //convert mask value double to integer
            gaussianMaskInt[i][j] = cvRound(gaussianMaskDou[i][j] / maskMin);
            maskSum += gaussianMaskInt[i][j];
        }
    }
    
    printf("Mask Sum is %d, rad is %d.\n", maskSum, maskRad);
    //represent mask using global pointer
    for(i = 0; i <  maskWidth; i++)
        for (j = 0; j < maskWidth; j++)
            *(gaussianMask + i*maskWidth + j) = gaussianMaskInt[i][j];
}

void cannyDector()
{
    useGaussianBlur();
    useSobelDerivat();
    nonMaxSuppress();
    hysteresisThreshold(20, 50);
    
    //print the gaussian mask
    //access the gaussian mask using pointer
    for(int i = 0; i <  maskWidth; i++)
    {
        for (int j = 0; j < maskWidth; j++)
        {
            printf("%-6d",*(gaussianMask + i*maskWidth + j));
        }
        printf("\n");
    }
}

void useGaussianBlur()
{
    //keep border pixel unchanged
    bluredImage = oriImage.clone();
    //Convolutuion process, image*mask
    for (int i = 0; i < oriImage.rows; i++)
    {
        for (int j = 0; j < oriImage.cols; j++)
        {
            if ( (i >= maskRad)&&(i <= oriImage.rows-maskRad)&&(j >= maskRad)&&(j<=oriImage.cols-maskRad) )
            {
                double sum = 0;
                
                for (int x = 0; x < maskWidth; x++)
                    for (int y = 0; y < maskWidth; y++)
                    {
                        sum += *(gaussianMask + x*maskWidth + y) * (double)(oriImage.at<uchar>(i + x - maskRad, j + y - maskRad));
                    }
                bluredImage.at<uchar>(i, j) = sum/maskSum;
            }
        }
        
    }
}

void useSobelDerivat()
{
    edgeMagImage = Mat::zeros(bluredImage.rows, bluredImage.cols, CV_8UC1);
    edgeAngImage = Mat::zeros(bluredImage.rows, bluredImage.cols, CV_8UC1);
    
    int xsobelMask[3][3] = { {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1} };
    int ysobelMask[3][3] = { {1, 2, 1},
        {0, 0 , 0},
        {-1, -2, -1} };
    int sobelRad = 1;//int(width/2)=3/2=1
    int sobelWidth = 3;
    
    for (int i = 0; i < bluredImage.rows; i++)
    {
        for (int j = 0; j < bluredImage.cols; j++)
        {
            if ( i == sobelRad-1 || i == bluredImage.rows-sobelRad || j == sobelRad-1 || j ==bluredImage.cols-sobelRad)
            {
                edgeMagImage.at<uchar>(i, j) = 0;
                edgeAngImage.at<uchar>(i, j) = 255;
            }
            else
            {
                int sumX = 0;
                int sumY = 0;
                
                for (int x = 0; x < sobelWidth; x++)
                    for (int y = 0; y < sobelWidth; y++)
                    {
                        sumX += xsobelMask[x][y] * bluredImage.at<uchar>(i+x-sobelRad, j+y-sobelRad);
                        sumY += ysobelMask[x][y] * bluredImage.at<uchar>(i+x-sobelRad, j+y-sobelRad);
                    }
                
                int mag = sqrt(sumX*sumX + sumY*sumY);
                if (mag > 255)  mag = 255;
                edgeMagImage.at<uchar>(i, j) = mag;
                
                int ang = (atan2(sumY, sumX)/M_PI) * 180;
                
                if ( ( (ang < 22.5) && (ang >= -22.5) ) || (ang >= 157.5) || (ang < -157.5) )
                    ang = 0;
                if ( ( (ang >= 22.5) && (ang < 67.5) ) || ( (ang < -112.5) && (ang >= -157.5) ) )
                    ang = 45;
                if ( ( (ang >= 67.5) && (ang < 112.5) ) || ( (ang < -67.5) && (ang >= -112.5) ) )
                    ang = 90;
                if ( ( (ang >= 112.5) && (ang < 157.5) ) || ( (ang < -22.5) && (ang >= -67.5) ) )
                    ang = 135;
                edgeAngImage.at<uchar>(i, j) = ang;
            }
        }
    }
}

void nonMaxSuppress()
{
    thinEdgeImage = edgeMagImage.clone();
    
    for (int i = 0; i < thinEdgeImage.rows; i++)
    {
        for (int j = 0; j < thinEdgeImage.cols; j++)
        {
            if ( i == 0 || i == thinEdgeImage.rows-1 || j == 0 || j == thinEdgeImage.cols-1){
                thinEdgeImage.at<uchar>(i, j) = 0;
            }
            else
            {
                //0 degree direction, left and right
                if (edgeAngImage.at<uchar>(i, j) == 0) {
                    if ( edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i, j+1) || edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i, j-1) )
                        thinEdgeImage.at<uchar>(i, j) = 0;
                }
                //45 degree direction,up right and down left
                if (edgeAngImage.at<uchar>(i, j) == 45) {
                    if ( edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i+1, j-1) || edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i-1, j+1) )
                        thinEdgeImage.at<uchar>(i, j) = 0;
                }
                //90 degree direction, up and down
                if (edgeAngImage.at<uchar>(i, j) == 90) {
                    if ( edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i+1, j) || edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i-1, j) )
                        thinEdgeImage.at<uchar>(i, j) = 0;
                }
                //135 degree direction, up left and down right
                if (edgeAngImage.at<uchar>(i, j) == 135) {
                    if ( edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i-1, j-1) || edgeMagImage.at<uchar>(i, j) < edgeMagImage.at<uchar>(i+1, j+1) )
                        thinEdgeImage.at<uchar>(i, j) = 0;
                }
            }
        }
    }
}

void hysteresisThreshold(int lowTh, int highTh)
{
    thresholdImage = thinEdgeImage.clone();
    
    for (int i=0; i<thresholdImage.rows; i++)
    {
        for (int j = 0; j<thresholdImage.cols; j++)
        {
            if(thinEdgeImage.at<uchar>(i,j) > highTh)
                thresholdImage.at<uchar>(i,j) = 255;
            else if(thinEdgeImage.at<uchar>(i,j) < lowTh)
                thresholdImage.at<uchar>(i,j) = 0;
            else
            {
                bool isHigher = false;
                bool doConnect = false;
                for (int x=i-1; x < i+2; x++)
                {
                    for (int y = j-1; y<j+2; y++)
                    {
                        if (x <= 0 || y <= 0 || x > thresholdImage.rows || y > thresholdImage.cols)
                            continue;
                        else
                        {
                            if (thinEdgeImage.at<uchar>(x,y) > highTh)
                            {
                                thresholdImage.at<uchar>(i,j) = 255;
                                isHigher = true;
                                break;
                            }
                            else if (thinEdgeImage.at<uchar>(x,y) <= highTh && thinEdgeImage.at<uchar>(x,y) >= lowTh)
                                doConnect = true;
                        }
                    }
                    if (isHigher)    break;
                }
                if (!isHigher && doConnect)
                    for (int x = i-2; x < i+3; x++)
                    {
                        for (int y = j-2; y < j+3; y++)
                        {
                            if (x < 0 || y < 0 || x > thresholdImage.rows || y > thresholdImage.cols)
                                continue;
                            else
                            {
                                if (thinEdgeImage.at<uchar>(x,y) > highTh)
                                {
                                    thresholdImage.at<uchar>(i,j) = 255;
                                    isHigher = true;
                                    break;
                                }
                            }
                        }
                        if (isHigher)    break;
                    }
                if (!isHigher)   thresholdImage.at<uchar>(i,j) = 0;
            }
        }
    }
}

Mat combineImage()
{
    Mat h1CombineImage, h2CombineImage, allImage;
    Mat extraImage = Mat(oriImage.rows, oriImage.cols, CV_8UC1, Scalar(255));
    char sigmaChar[10];
    sprintf(sigmaChar, "%.2f", sigma);
    
    putText(extraImage, "Ori, Gaus, Sobel", Point(10,20), FONT_HERSHEY_PLAIN, 1, Scalar(0));
    putText(extraImage, "NMS, Threshold", Point(10,38), FONT_HERSHEY_PLAIN, 1, Scalar(0));
    putText(extraImage, "Sigma: ", Point(10,56), FONT_HERSHEY_PLAIN, 1, Scalar(0));
    putText(extraImage, sigmaChar, Point(65,56), FONT_HERSHEY_PLAIN, 1, Scalar(0));
    
    hconcat(oriImage, bluredImage, h1CombineImage);
    hconcat(h1CombineImage, edgeMagImage, h1CombineImage);
    hconcat(thinEdgeImage, thresholdImage, h2CombineImage);
    hconcat(h2CombineImage, extraImage, h2CombineImage);
    vconcat(h1CombineImage, h2CombineImage, allImage);
    
    return allImage;
}
