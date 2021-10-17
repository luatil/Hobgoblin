/* date = October 16th 2021 4:16 pm */

#ifndef HOBGOBLIN_DRAW_H
#define HOBGOBLIN_DRAW_H

void 
DrawPoint(u8* Image, int Width, int Height, 
          int X, int Y,
          u8 R, u8 G, u8 B)
{
    if(X > 0 && X < Width &&
       Y > 0 && Y < Height)
    {
        Image[X*3+Y*Width*3]   = R;
        Image[X*3+Y*Width*3+1] = G;
        Image[X*3+Y*Width*3+2] = B;
    }
}

void
DrawRectangle(u8* Image, int Width, int Height, 
              int TopLeftX, int TopLeftY, 
              int BottonRightX, int BottonRightY,
              u8 R, u8 G, u8 B)
{
    if(TopLeftX > BottonRightX || TopLeftY > BottonRightY)
    {
        return;
    }
    int PX = TopLeftX;
    while(PX != BottonRightX)
    {
        DrawPoint(Image, Width, Height, PX, TopLeftY, R, G, B);
        DrawPoint(Image, Width, Height, PX, BottonRightY, R, G, B);
        PX++;
    }
    int PY = TopLeftY;
    while(PY != BottonRightY)
    {
        DrawPoint(Image, Width, Height, TopLeftX, PY, R, G, B);
        DrawPoint(Image, Width, Height, BottonRightX, PY, R, G, B);
        PY++;
    }
}

int
DrawCircleCost(int CenterX, int CenterY, int Radius,
           int X, int Y)
{
    int Result = (X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY) - Radius * Radius;
    return(Result >= 0 ? Result : -Result);
}



void 
DrawCircle(u8* Image, int Width, int Height, 
           int CenterX, int CenterY, int Radius,
           u8 R, u8 G, u8 B)
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
        
        DrawPoint(Image, Width, Height, p1x, p1y, R, G, B);
        DrawPoint(Image, Width, Height, p2x, p2y, R, G, B);
        DrawPoint(Image, Width, Height, p3x, p3y, R, G, B);
        DrawPoint(Image, Width, Height, p4x, p4y, R, G, B);
        
        // Need to find the minimum bettwen 4 options
        int UpCost    = DrawCircleCost(CenterX, CenterY, Radius, p1x, p1y-1);
        //int DownCost  = DrawCircleCost(CenterX, CenterY, Radius, p1x, p1y+1);
        int LeftCost  = DrawCircleCost(CenterX, CenterY, Radius, p1x-1, p1y);
        //int RigthCost = DrawCircleCost(CenterX, CenterY, Radius, p1x+1, p1y);
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

float LineCost(float M, int B, int X, int Y)
{
    float Result = M*(float)X+(float)B - (float)Y;
    if(Result >= 0)
    {
        return Result;
    }
    else 
    {
        return -Result;
    }
}

void 
DrawLineRhoThetaDegrees(u8* Image, int Width, int Height, 
                        int Rho, int Theta, 
                        u8 R, u8 G, u8 B)
{
    if(Rho < 0)
    {
        if(Theta < 90) 
        {
            return;
        }
        float ThetaRadians = Theta*PI/180.0f;
        Rho = -Rho;
        int x = Rho / cos(PI - ThetaRadians);
        int b = - Rho / cos(ThetaRadians - PI/2.0f); 
        int y = 0;
        float m = tan((ThetaRadians-PI/2));
        
        while(y < Height && x < Width)
        {
            DrawPoint(Image, Width, Height, x, y, R, G, B);
            float MoveRight = LineCost(m, b, x+1, y);
            float MoveDown = LineCost(m, b, x, y+1);
            if(MoveRight < MoveDown)
            {
                x += 1;
            }
            else 
            {
                y += 1;
            }
        }
        
        return;
        
    }
    
    float ThetaRadians = Theta*PI/180.0f;
    
    if(Theta == 0)
    {
        int x = Rho;
        int y = 0;
        while(y != Height)
        {
            DrawPoint(Image, Width, Height, x, y, R, G, B);
            y += 1;
        }
        return;
    }
    
    int b, x, y;
    float m;
    b = Rho / sin(ThetaRadians);
    m = -1 / tan(ThetaRadians);
	
    if (Theta <= 90)
    {
        if(b < Height)
        {
            x = 0;
            y = b;
        }
        else 
        {
            x = (b - Width) * tan(ThetaRadians);
            y = Height - 1;
        }
        while(y >= 0 && x <= Width)
        {
            DrawPoint(Image, Width, Height, x, y, R, G, B);
            float MoveRight = LineCost(m, b, x+1, y);
            float MoveUp = LineCost(m, b, x, y-1);
            if(MoveRight <= MoveUp)
            {
                x += 1;
            }
            else 
            {
                y -= 1;
            }
        }
    }
    else if(Theta < 180)
    { 
        if(b > Height)
        {
            return;
        }
        x = 0;
        y = b;
        //m = 1.0f * tan(ThetaRadians - PI/2.0f);
        while(y < Height && x < Width)
        {
            DrawPoint(Image, Width, Height, x, y, R, G, B);
            float MoveRight = LineCost(m, b, x+1, y);
            float MoveDown = LineCost(m, b, x, y+1);
            if(MoveRight < MoveDown)
            {
                x += 1;
            }
            else 
            {
                y += 1;
            }
        }
    }
}


#endif //HOBGOBLIN_DRAW_H
