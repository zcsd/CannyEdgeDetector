#ifndef LOAD_IMAGE_H
#define LOAD_IMAGE_H

#include <stdio.h>
#include "opencv2/core.hpp"

using namespace cv;

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

#endif
