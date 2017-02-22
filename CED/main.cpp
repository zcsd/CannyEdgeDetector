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

Mat oriImage, bluredImage, edgeMagImage, edgeAngImage, thinEdgeImage;
int *gaussianMask, maskRad, maskWidth = 0, maskSum = 0;

int main(int argc, char** argv)
{
    int imgChoice;
    char oriwndName[] = "Original Image", bluwndName[] = "Blured Image", magwndName[] = "Edge Mag Image", angwndName[] = "Edge Angle Image";
    
    imgChoice = getChoice();
    oriImage = raw2Mat(imgChoice);

    createGaussianKernel();
    cannyDector();
    
    imshow(oriwndName, oriImage);
    imshow(bluwndName, bluredImage);
    imshow(magwndName, edgeMagImage);
    imshow(angwndName, edgeAngImage);
    
    waitKey();
    
    free(gaussianMask);
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
    float sigma;
    
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
    
    //access the gaussian mask using pointer
    for(int i = 0; i <  maskWidth; i++)
    {
        for (int j = 0; j < maskWidth; j++)
        {
            printf("%-6d",*(gaussianMask + i*maskWidth + j));
        }
        printf("\n");
    }
    
    //acceess the pixel value using .at<uchar>(i,j)
    int counter = 0;
    for(int i = 0; i < oriImage.rows; i++)
    {
        for(int j = 0; j < oriImage.cols; j++)
        {
            counter++;
            //printf("test: %d", tmpImg.at<uchar>(i,j));
        }
    }
    printf("Total Pixel is %d\n", counter);
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
            if ( i == sobelRad || i == bluredImage.rows-sobelRad || j == sobelRad || j ==bluredImage.cols-sobelRad)
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
