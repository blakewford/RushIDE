#include "Models.h"

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

void MatMul4x1(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]) + (A[1]*B[1]) + (A[2]*B[2]) + (A[3]*B[3]);
    C[1] = (A[4]*B[0]) + (A[5]*B[1]) + (A[6]*B[2]) + (A[7]*B[3]);
    C[2] = (A[8]*B[0]) + (A[9]*B[1]) + (A[10]*B[2]) + (A[11]*B[3]);
    C[3] = (A[12]*B[0]) + (A[13]*B[1]) + (A[14]*B[2]) + (A[15]*B[3]);
}

void rotationEntry(const int16_t angle, param& parameter, rotation_axis axis)
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
            parameter.value[0] = cos(radians);
            parameter.value[1] = sin(radians);
//            parameter.value[2] = 0;
            parameter.value[3] = -sin(radians);
            parameter.value[4] = cos(radians);
//            parameter.value[5] = 0;
//            parameter.value[6] = 0;
//            parameter.value[7] = 0;
            parameter.value[8] = 1;
            break;
        default:
            break;
    }
}

float ortho[4][4] =
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

float copy[40];

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
    memcpy(s_Ortho.value, ortho, 16*sizeof(float));
}

void Models::drawCompressedModel(const uint8_t* model, const float* map, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
    int16_t count = (int16_t)map[0];
    count*=3;

    copy[0] = 3;
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
        drawModel(xAngle, yAngle, zAngle, color);
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
        float C[(int8_t)(A.shape[0]*B.shape[1])];
        MatMul3x1(C, A.value, B.value);
        memcpy(&copy[start], &C[0], 3*sizeof(float));
    }
}

void Models::drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{

    modifyAngle(yAngle, Y);
    modifyAngle(xAngle, X);
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
        float I[(int8_t)(s_zAngle.shape[0]*H.shape[1])];
        MatMul3x1(I, s_zAngle.value, H.value);
        memcpy(&copy[start], &I[0], 3*sizeof(float));
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

        float L[(int8_t)(s_Ortho.shape[0]*K.shape[1])];
        MatMul4x1(L, s_Ortho.value, K.value);
        memcpy(&copy[start], &L[0], 3*sizeof(float));
    }

    int8_t offsetX = WIDTH/2;
    int8_t offsetY = HEIGHT/2;

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

        arduboy.fillTriangle(x1, y1, x2, y2, x3, y3, color);
    }
}
