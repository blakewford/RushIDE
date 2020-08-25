#include "Models.h"
#include <assert.h>

#include <Arduboy2.h>
extern Arduboy2Base arduboy;

enum parse_state: int8_t
{
    CONSTANTS,
    SHAPE,
    NAME,
    UNKNOWN
};

void MatMul3x1(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]) + (A[1]*B[1]) + (A[2]*B[2]);
    C[1] = (A[3]*B[0]) + (A[4]*B[1]) + (A[5]*B[2]);
    C[2] = (A[6]*B[0]) + (A[7]*B[1]) + (A[8]*B[2]);
}

void MatMul3x1Sparse(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]);
    C[1] = (A[4]*B[1]);
    C[2] = (A[8]*B[2]);
}

void MatMul4x1Sparse(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]);
    C[1] = (A[5]*B[1]);
    C[2] = 0;
    C[3] = (A[15]*B[3]);
}

param X_AT_15_DEGREES;

void rotationEntry(const int16_t angle, param& parameter, const rotation_axis axis)
{
    const float radians = (angle%360)*0.0174533;

//    parameter.shape[0] = 3;
    parameter.shape[1] = 3;

    const float sine = sin(radians);
    const float cosine = cos(radians);

    switch(axis)
    {
        case X:
            parameter.value[0] = 1;
/*
            parameter.value[1] = 0;
            parameter.value[2] = 0;
            parameter.value[3] = 0;
*/
            parameter.value[4] = cosine;
            parameter.value[5] = sine;
//            parameter.value[6] = 0;
            parameter.value[7] = -sine;
            parameter.value[8] = cosine;
            break;
        case Y:
            parameter.value[0] = cosine;
//            parameter.value[1] = 0;
            parameter.value[2] = -sine;
//            parameter.value[3] = 0;
            parameter.value[4] = 1;
//            parameter.value[5] = 0;
            parameter.value[6] = sine;
//            parameter.value[7] = 0;
            parameter.value[8] = cosine;
            break;
        case Z:
            parameter.value[0] = cosine;
            parameter.value[1] = sine;
//            parameter.value[2] = 0;
            parameter.value[3] = -sine;
            parameter.value[4] = cosine;
//            parameter.value[5] = 0;
//            parameter.value[6] = 0;
//            parameter.value[7] = 0;
            parameter.value[8] = 1;
            break;
        default:
            break;
    }
}

const float ortho[4][4] =
{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

struct vertex
{
    float x;
    float y;
    float z;
};

float copy[10*sizeof(float)];

param Models::s_Ortho;
param Models::s_zAngle;

void Models::begin()
{
    s_zAngle.value[0] = 1;
//    s_zAngle.value[1] = 0;
//    s_zAngle.value[2] = 0;
//    s_zAngle.value[3] = -0;
    s_zAngle.value[4] = 1;
//    s_zAngle.value[5] = 0;
//    s_zAngle.value[6] = 0;
//    s_zAngle.value[7] = 0;
    s_zAngle.value[8] = 1;
//    s_zAngle.shape[0] = 3;
    s_zAngle.shape[1] = 3;

    s_Ortho.shape[0] = 4;
    s_Ortho.shape[1] = 4;
    s_Ortho.value[0] = ortho[0][0];
//    s_Ortho.value[1] = ortho[0][1];
//    s_Ortho.value[2] = ortho[0][2];
//    s_Ortho.value[3] = ortho[0][3];
//    s_Ortho.value[4] = ortho[1][0];
    s_Ortho.value[5] = ortho[1][1];
//    s_Ortho.value[6] = ortho[1][2];
//    s_Ortho.value[7] = ortho[1][3];
//    s_Ortho.value[8] = ortho[2][0];
//    s_Ortho.value[9] = ortho[2][1];
//    s_Ortho.value[10] = ortho[2][2];
//    s_Ortho.value[11] = ortho[2][3];
//    s_Ortho.value[12] = ortho[3][0];
//    s_Ortho.value[13] = ortho[3][1];
//    s_Ortho.value[14] = ortho[3][2];
    s_Ortho.value[15] = ortho[3][3];

    X_AT_15_DEGREES.value[0] = 1;
//    X_AT_15_DEGREES.value[1] = 0;
//    X_AT_15_DEGREES.value[2] = 0;
//    X_AT_15_DEGREES.value[3] = 0;
    X_AT_15_DEGREES.value[4] = 0.965925813;
    X_AT_15_DEGREES.value[5] = 0.258819163;
    X_AT_15_DEGREES.value[6] = -0.258819163;
    X_AT_15_DEGREES.value[7] = 0.965925813;
//    X_AT_15_DEGREES.value[8] = 0;
//    X_AT_15_DEGREES.shape[0] = 3;
    X_AT_15_DEGREES.shape[1] = 3;
}

uint16_t fillIndex = 0;
void Models::drawCompressedModel(const uint8_t* model, const float* map, const uint8_t* fill, int16_t xAngle, int16_t yAngle, int16_t zAngle)
{
    int16_t count = (int16_t)map[0];
    count*=3;

    copy[0] = 3;

    fillIndex = 0;

    int16_t ndx = 0;
    while(ndx < count)
    {
        copy[1] = map[pgm_read_byte(&model[ndx])];
        copy[2] = map[pgm_read_byte(&model[ndx+1])];
        copy[3] = map[pgm_read_byte(&model[ndx+2])];
        copy[4] = map[pgm_read_byte(&model[ndx+3])];
        copy[5] = map[pgm_read_byte(&model[ndx+4])];
        copy[6] = map[pgm_read_byte(&model[ndx+5])];
        copy[7] = map[pgm_read_byte(&model[ndx+6])];
        copy[8] = map[pgm_read_byte(&model[ndx+7])];
        copy[9] = map[pgm_read_byte(&model[ndx+8])];
        drawModel(xAngle, yAngle, zAngle, fill);
        ndx+=9;
    }
}

void Models::modifyAngle(const int16_t angle, const rotation_axis axis)
{
    param A, B;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];

    rotationEntry(angle, A, axis);
    while(count--)
    {
        int16_t start = current;
        B.value[2] = copy[current++];
        B.value[1] = copy[current++];
        B.value[0] = copy[current++];
//        B.shape[0] = 3;
//        B.shape[1] = 1;
        float C[3];
        MatMul3x1(C, A.value, B.value);
        copy[start]   = C[0];
        copy[start+1] = C[1];
        copy[start+2] = C[2];
    }
}

namespace std
{
    template< class T >
    void swap(T& A, T& B)
    {
        T C = A;
        A = B;
        B = C;
    }
}

uint8_t currentColor = 0;
uint8_t getColor(uint8_t color)
{
    if(color == 0) return 0;
    if(color == 1) return 1;

    if(color == 2)
    {
        int fill = random() % color;
        return fill != 0;
    }

    uint8_t value = currentColor == 0;
    currentColor++;
    if(currentColor == color) currentColor = 0;

    return value;
}

void fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color)
{
    currentColor = 0;

// Fill a triangle - Bresenham method
// Original from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    uint8_t e1,e2;
    int8_t signx1,signx2,dx1,dy1,dx2,dy2;
    uint8_t t1x,t2x,y,minx,maxx,t1xp,t2xp;

    bool changed1 = false;
    bool changed2 = false;

    if(y1>y2) { std::swap(y1,y2); std::swap(x1,x2); }
    if(y1>y3) { std::swap(y1,y3); std::swap(x1,x3); }
    if(y2>y3) { std::swap(y2,y3); std::swap(x2,x3); }

    t1x=t2x=x1; y=y1;

    dx1 = (int8_t)(x2 - x1); if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
    dy1 = (int8_t)(y2 - y1);

    dx2 = (int8_t)(x3 - x1); if(dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
    dy2 = (int8_t)(y3 - y1);

    if(dy1 > dx1)
    {
        std::swap(dx1,dy1);
        changed1 = true;
    }
    if(dy2 > dx2)
    {
        std::swap(dy2,dx2);
        changed2 = true;
    }

    e2 = (uint8_t)(dx2>>1);

    if(y1 == y2) goto next;
    e1 = (uint8_t)(dx1>>1);

    for(uint8_t i = 0; i < dx1;)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x)
        {
            minx=t1x; maxx=t2x;
        }
        else
        {
            minx=t2x;
            maxx=t1x;
        }

        while(i<dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if(changed1) t1xp=signx1;
                else goto next1;
            }
            if(changed1) break;
            else t1x += signx1;
        }
  next1:
        while(true)
        {
            e2 += dy2;
            while(e2 >= dx2)
            {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else goto next2;
            }
            if(changed2) break;
            else t2x += signx2;
        }
  next2:
        if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
        if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
        arduboy.drawFastHLine(minx, y, maxx-minx, getColor(color));
        if(!changed1) t1x += signx1;
        t1x+=t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;

        y += 1;
        if(y == y2) break;
    }
    next:
    dx1 = (int8_t)(x3 - x2); if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
    dy1 = (int8_t)(y3 - y2);
    t1x=x2;

    if(dy1 > dx1)
    {
        std::swap(dy1,dx1);
        changed1 = true;
    } else changed1=false;

    e1 = (uint8_t)(dx1>>1);

    for(uint8_t i = 0; i<=dx1; i++)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x)
        {
            minx=t1x;
            maxx=t2x;
        }
        else
        {
            minx=t2x;
            maxx=t1x;
        }
        while(i<dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if(changed1)
                {
                    t1xp=signx1; break;
                }
                else goto next3;
            }
            if(changed1) break;
            else t1x += signx1;
            if(i < dx1) i++;
        }
  next3:
        while(t2x != x3)
        {
            e2 += dy2;
            while(e2 >= dx2)
            {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else goto next4;
            }
            if(changed2) break;
            else t2x += signx2;
        }
  next4:
        if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
        if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
        arduboy.drawFastHLine(minx, y, maxx-minx, getColor(color));

        if(!changed1) t1x += signx1;
        t1x += t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;
        y += 1;

        if(y > y3) return;
    }
}

void Models::modifyXAngle()
{
    param B;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];
    while(count--)
    {
        int16_t start = current;
        B.value[2] = copy[current++];
        B.value[1] = copy[current++];
        B.value[0] = copy[current++];
//        B.shape[0] = 3;
//        B.shape[1] = 1;
        float C[3];
        MatMul3x1(C, X_AT_15_DEGREES.value, B.value);
        copy[start]   = C[0];
        copy[start+1] = C[1];
        copy[start+2] = C[2];
    }
}

void Models::drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, const uint8_t* fill)
{

    modifyAngle(yAngle, Y);
    modifyXAngle();
//    modifyAngle(zAngle, Z);

    param H;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];
    while(count--)
    {
        int16_t start = current;
        H.value[2] = copy[current++];
        H.value[1] = copy[current++];
        H.value[0] = copy[current++];
//        H.shape[0] = 3;
//        H.shape[1] = 1;
        float I[3];
        MatMul3x1Sparse(I, s_zAngle.value, H.value);
        copy[start]   = I[0];
        copy[start+1] = I[1];
        copy[start+2] = I[2];
    }

    current = 1;
    count = (int16_t)copy[0];
    while(count--)
    {
        param K;
        int16_t start = current;
        K.value[3] = 1.0f;
        K.value[2] = copy[current++];
        K.value[1] = copy[current++];
        K.value[0] = copy[current++];
        K.shape[0] = 4;
//        K.shape[1] = 1;

        float L[4];
        MatMul4x1Sparse(L, s_Ortho.value, K.value);
        copy[start]   = L[0];
        copy[start+1] = L[1];
        copy[start+2] = L[2];
    }

    const int8_t offsetX = WIDTH/2;
    const int8_t offsetY = HEIGHT/2;

    current = 1;
    count = (int16_t)copy[0];
    while(current < count*3)
    {
        int16_t x1 = copy[current++] + offsetX;
        int16_t y1 = copy[current++] + offsetY;
        current++;
        int16_t x2 = copy[current++] + offsetX;
        int16_t y2 = copy[current++] + offsetY;
        current++;
        int16_t x3 = copy[current++] + offsetX;
        int16_t y3 = copy[current++] + offsetY;
        current++;

        fillTriangle(x1, y1, x2, y2, x3, y3, fill == nullptr ? 1: pgm_read_byte(&fill[fillIndex++]));
    }
}
