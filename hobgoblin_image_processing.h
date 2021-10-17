/* date = October 16th 2021 4:12 pm */

#ifndef HOBGOBLIN_IMAGE_PROCESSING_H
#define HOBGOBLIN_IMAGE_PROCESSING_H

#include <math.h>
#include <string.h>

#define Min(a, b) ((a < b) ? a : b)

float uniform_blur[9] = {1.0f/9, 1.0f/9, 1.0f/9, 1.0f/9, 1.0f/9, 1.0f/9, 1.0f/9, 1.0f/9, 1.0f/9};
float gaussian_blur[9] = {1.0f/16, 2.f/16, 1.f/16, 2.f/16, 4.f/16, 2.f/16, 1.f/16, 2.f/16, 1.f/16};
float sharpen[9] = {0.0f, -1.0f, 0.0f, -1.0f, 5.0f, -1.0f, 0.0f, -1.0f, 0.0f};
float identity[9] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float bottom_sobel[9] = {-1.0f, -2.0f, -1.0f, 0.f, 0.f, 0.f, 1.f, 2.f, 1.f};
float left_sobel[9] = {1.f, 0.f, -1.f, 2.f, 0.f, -2.f, 1.f, 0.f, -1.f};
float emboss[9] = {-2.f, -1.f, 0.f, -1.f, 1.f, 1.f, 0.f, 1.f, 2.f};
float simple_edge_detection[9] = {0, -1.f, 0.f, -1.f, 4.f, -1.f, 0.f, -1.f, 0.f};

void
ApplyKernel(u8* InputImage, int Width, int Height, 
            u8* OutputImage, float* Kernel)
{
    int ImageArea = Width * Height;
    u8* SourcePixel = InputImage + Width + 1;
    u8* DestinationPixel = OutputImage + Width + 1;
    for(int I = Width + 1; I < ImageArea - Width - 1; I++)
    {
        *DestinationPixel = 
        (Kernel[0]**(SourcePixel-1-Width)
         + (Kernel[1]**(SourcePixel-Width))
         + (Kernel[2]**(SourcePixel-Width+1))
         + (Kernel[3]**(SourcePixel-1))
         + (Kernel[4]**SourcePixel)
         + (Kernel[5]**(SourcePixel+1))
         + (Kernel[6]**(SourcePixel+Width-1))
         + (Kernel[7]**(SourcePixel+Width))
         + (Kernel[8]**(SourcePixel+Width+1)));
        SourcePixel++;
        DestinationPixel++;
    }
}

void
ApplyKernelFloatImage(u8* InputImage, int Width, int Height, 
                      float* OutputImage, float* Kernel)
{
    int ImageArea = Width * Height;
    u8* SourcePixel = InputImage + Width + 1;
    float* DestinationPixel = OutputImage + Width + 1;
    for(int I = Width + 1; I < ImageArea - Width - 1; I++)
    {
        *DestinationPixel = 
            abs(Kernel[0]**(SourcePixel-1-Width)
                + (Kernel[1]**(SourcePixel-Width))
                + (Kernel[2]**(SourcePixel-Width+1))
                + (Kernel[3]**(SourcePixel-1))
                + (Kernel[4]**SourcePixel)
                + (Kernel[5]**(SourcePixel+1))
                + (Kernel[6]**(SourcePixel+Width-1))
                + (Kernel[7]**(SourcePixel+Width))
                + (Kernel[8]**(SourcePixel+Width+1)));
        SourcePixel++;
        DestinationPixel++;
    }
}

void
ApplyThreshold(u8* InputImage, int Width, int Height, 
               u8* OutputImage, u8 Threshold)
{
    int ImageArea = Width * Height;
    u8* SourcePixel = InputImage;
    u8* DestinationPixel = OutputImage;
    for(int I = 0; I < ImageArea; I++)
    {
        if(*SourcePixel < Threshold)
        {
            *DestinationPixel = 0;
        }
        else 
        {
            *DestinationPixel = 255;
        }
        SourcePixel++;
        DestinationPixel++;
    }
}


void 
ApplyHysteresisThresholding(u8* InputImage, int Width, int Height,
                            u8* OutputImage, u8 LowThreshold, u8 HighThreshold)
{
    int Positions[9] = {-1-Width, -Width, -Width + 1, -1, 0, +1, Width-1, Width, Width+1};
    int ImageArea = Width * Height;
    u8* SourcePixel = InputImage + Width + 1;
    u8* DestinationPixel = OutputImage + Width + 1;
    for(int I = Width + 1; I < ImageArea - Width - 1; I++)
    {
        if(*SourcePixel < LowThreshold)
        {
            *DestinationPixel = 0;
        }
        else if(*SourcePixel > HighThreshold)
        {
            *DestinationPixel = 255;
        }
        else 
        {
            *DestinationPixel = 0;
            for(int J = 0; J < 9; J++)
            {
                if(*(SourcePixel + Positions[J]) > HighThreshold)
                {
                    *DestinationPixel = 255;
                    break;
                }
            }
        }
        SourcePixel++;
        DestinationPixel++;
    }
}

void 
GradientEdgeDetection(u8* InputImage, int Width, 
                      int Height, u8* OutputImage,
                      int Threshold)
{
    int ImageArea = Width*Height;
    float* LeftSobelImage = Allocate(float, ImageArea);
    float* BottomSobelImage = Allocate(float, ImageArea);
    
    ApplyKernel(InputImage, Width, Height, OutputImage, gaussian_blur);
    
    ApplyKernelFloatImage(OutputImage, Width, Height, BottomSobelImage, bottom_sobel);
    ApplyKernelFloatImage(OutputImage, Width, Height, LeftSobelImage, left_sobel);
    
    for(u8* out = OutputImage; out < OutputImage + ImageArea; out++)
    {
        float left = *LeftSobelImage;
        float right = *BottomSobelImage;
        int res = (int)sqrt(left*left+right*right);
        res = res > 255 ? 255 : res;
        *out = (u8)res;
        LeftSobelImage++;
        BottomSobelImage++;
    }
    ApplyHysteresisThresholding(OutputImage, Width, Height, OutputImage, 80, 150);
    free(LeftSobelImage);
    free(BottomSobelImage);
}

unsigned char*
ReadPBMP4(char* Filename, int* Width, int* Height)
{
    FILE* File = fopen(Filename, "r");
    
    if(File == 0)
    {
        return 0;
    }
    
    fseek(File, 0L, SEEK_END);
    int FileSize = ftell(File);
    
    unsigned char *Buffer = malloc(FileSize);
    rewind(File);
    fread(Buffer, FileSize, 1, File);
    int BufferLen = strlen(Buffer);
    char WidthBuffer[10];
    int J = 3;
    while(Buffer[J] != 32)
    {
        WidthBuffer[J-3] = Buffer[J];
        J++;
    }
    int K = J;
    char HeightBuffer[10];
    while(Buffer[J] != 10)
    {
        HeightBuffer[J-K] = Buffer[J];
        J++;
    }
    *Width  = atoi(WidthBuffer);
    *Height = atoi(HeightBuffer);
    
    unsigned char* Image = malloc(sizeof(unsigned char)*(*Width)*(*Height));
    unsigned char* Pixel = Image;
    unsigned char* SourcePixel = Buffer + J + 1;
    
    unsigned char* Row = Image;
    int LineWidth = (*Width / 8) + ((*Width % 8) != 0);
    for(int Y = 0; Y < *Height; Y++)
    {
        Pixel = Row;
        for(int X = 0; X < LineWidth; X++)
        {
            for(int K = 0; K < 8; K++)
            {
                unsigned char Bit = ((*SourcePixel >> (7-K))) & 0x01;
                *Pixel++ = !Bit*255;
            }
            SourcePixel++;
        }
        Row += *Width;
    }
    
    return Image;
}

int
CircleCost(int CenterX, int CenterY, int Radius,
           int X, int Y)
{
    int Result = (X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY) - Radius * Radius;
    return(Result >= 0 ? Result : -Result);
}

void 
AccumulatePoint(u8* Image, int Width, int Height, 
                int X, int Y)
{
    if(X > 0 && X < Width &&
       Y > 0 && Y < Height &&
       Image[X+Y*Width] < 255)
    {
        Image[X+Y*Width]++;
    }
}

void AccumulateCircleGrayscale(u8* Image, int Width, int Height, 
                               int CenterX, int CenterY, int Radius)
{
    // First with no clipping
    int p1x = CenterX + Radius;
    int p1y = CenterY;
    int p2x = CenterX;
    int p2y = CenterY - Radius;
    int p3x = CenterX - Radius;
    int p3y = CenterY;
    int p4x = CenterX;
    int p4y = CenterY + Radius;
    while(p1x != CenterX)
    {
        AccumulatePoint(Image, Width, Height, p1x, p1y);
        AccumulatePoint(Image, Width, Height, p2x, p2y);
        AccumulatePoint(Image, Width, Height, p3x, p3y);
        AccumulatePoint(Image, Width, Height, p4x, p4y);
        
        
        // Need to find the minimum bettwen 4 options
        int UpCost    = CircleCost(CenterX, CenterY, Radius, p1x, p1y-1);
        //int DownCost  = CircleCost(CenterX, CenterY, Radius, p1x, p1y+1);
        int LeftCost  = CircleCost(CenterX, CenterY, Radius, p1x-1, p1y);
        //int RigthCost = CircleCost(CenterX, CenterY, Radius, p1x+1, p1y);
        int DownCost = 1000;
        int RigthCost = 1000;
        
        int MinCost = Min(Min(UpCost, DownCost), Min(LeftCost, RigthCost));
        
        if(UpCost == MinCost)
        {
            p1y-=1;
            p2x-=1;
            p3y+=1;
            p4x+=1;
        }
        else if(LeftCost == MinCost)
        {
            p1x-=1;
            p2y+=1;
            p3x+=1;
            p4y-=1;
        }
    }
}

typedef struct Position
{
    int X;
    int Y;
} Position;

Position* 
GetCirclePositions(u8* Edges, int Width, int Height, 
                   int MinCircleRadius, int CircleRadiusOffset, 
                   int* DetectedCircleRadius, 
                   int NumberOfCircles)
{
    int AccumulatorWidth = Width;
    int AccumulatorHeight = Height;
    int AccumulatorArea = AccumulatorWidth * AccumulatorHeight;
    u8* Accumulator = Allocate(u8, AccumulatorArea*CircleRadiusOffset);
    memset(Accumulator, 0, AccumulatorArea*CircleRadiusOffset*sizeof(u8));
    
    u8* Pixel = Edges;
    for(int Y = 0; Y < Height; Y++)
    {
        for(int X = 0; X < Width; X++)
        {
            if(*Pixel)
            {
                for(int I = 0; I < CircleRadiusOffset; I++)
                {
                    AccumulateCircleGrayscale(Accumulator+I*AccumulatorArea, Width, Height, X, Y, MinCircleRadius+I);
                }
            }
            Pixel++;
        }
    }
    
    // NOTE(luatil): Find global peak to find the circle radius
    int MaxI = 0;
    {
        float Maxima = 0;
        Pixel = Accumulator;
        for(int I = 0; I < CircleRadiusOffset; I++)
        {
            float MaxCirc = 2*PI*(MinCircleRadius+I);
            for(int Y = 0; Y < Height; Y++)
            {
                for(int X = 0; X < Width; X++)
                {
                    if((*Pixel) / MaxCirc > Maxima)
                    {
                        Maxima = (*Pixel) / MaxCirc;
                        MaxI = I;
                        *DetectedCircleRadius = MinCircleRadius + I;
                    }
                    Pixel++;
                }
            }
        }
    }
    
    
    Position* CirclePositions = Allocate(Position, NumberOfCircles);
    for(int I = 0; I < NumberOfCircles; I++)
    {
        int MaxX = 0;
        int MaxY = 0;
        int Maxima = 0;
        Pixel = Accumulator + MaxI * AccumulatorArea;
        for(int Y = 0; Y < Height; Y++)
        {
            for(int X = 0; X < Width; X++)
            {
                if(*Pixel > Maxima) 
                {
                    Maxima = *Pixel;
                    MaxX = X;
                    MaxY = Y;
                }
                Pixel++;
            }
        }
        
        CirclePositions[I].X = MaxX;
        CirclePositions[I].Y = MaxY;
        
        // NOTE(luatil): Do Neighboorhood supression
        for(int Y = -*DetectedCircleRadius; Y < *DetectedCircleRadius; Y++)
        {
            for(int X = -*DetectedCircleRadius; X < *DetectedCircleRadius; X++)
            {
                if(MaxX + X > 0 && MaxX + X < AccumulatorWidth && 
                   MaxY + Y > 0 && MaxY + Y < AccumulatorHeight)
                {
                    Accumulator[MaxI*AccumulatorArea+MaxX+X+(MaxY+Y)*AccumulatorWidth] = 0;
                }
            }
        }
    }
    free(Accumulator);
    return CirclePositions;
}

void 
Rotate90Degrees3Channels(u8* InputImage, int Width, int Height, u8* OuputImage)
{
    u8* DestinationPixel = OuputImage;
    for(int Y = 0; Y < Width; Y++)
    {
        u8* SourcePixel = InputImage + (Height-1)*Width*3 + Y*3;
        for(int X = 0; X < Height; X++)
        {
            *DestinationPixel     = *SourcePixel;
            *(DestinationPixel+1) = *(SourcePixel+1);
            *(DestinationPixel+2) = *(SourcePixel+2);
            DestinationPixel += 3;
            SourcePixel -= Width * 3;
        }
    }
}

#endif //HOBGOBLIN_IMAGE_PROCESSING_H
