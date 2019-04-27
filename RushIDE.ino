#include "assets.h"

#include "Models.h"
#include <Arduino.h>
#include <Arduboy2.h>

Models models;
Sprites sprites;
Arduboy2Base arduboy;

const int16_t START_ANGLE = 180;

int8_t gSelection  = 0;
int16_t yAngle = START_ANGLE;

void setup()
{
    arduboy.begin();
    arduboy.setFrameRate(24);
    arduboy.initRandomSeed();

    models.begin();
}

void drawElements(const uint8_t* vehicle, const float* modelMap, const uint8_t* name)
{
    const int16_t xAngle = 15;
    models.drawCompressedModel(vehicle, modelMap, xAngle, yAngle, 0, 1);

    const int16_t x = 43;
    const int16_t y = 56;   
    sprites.drawSelfMasked(x, y, name, 0); 
}

void selection()
{
    switch(gSelection)
    {
        case 0:
            drawElements(car, ndxToValueCar, overland);
            break;
        case 1:
            drawElements(truck, ndxToValueTruck, baja);
            break;
        case 2:
            drawElements(bus, ndxToValueBus, burningman);
            break;
        case 3:
            drawElements(bike, ndxToValueBike, moto);
            break;
    }

    const int16_t leftX = 3;
    const int16_t leftY = 16;

    const int16_t rightX = 93;
    const int16_t rightY = 16; 
    sprites.drawSelfMasked(leftX, leftY, left, 0);
    sprites.drawSelfMasked(rightX, rightY, right, 0);

    switch(gSelection)
    {
        case 0:
            yAngle+=7;
            break;
        case 1:
            yAngle+=5;
            break;
        case 2:
            yAngle+=4;
            break;
        case 3:
            yAngle+=3;
            break;
    }

    if(arduboy.justPressed(LEFT_BUTTON))
    {
        gSelection--;
    }

    if(arduboy.justPressed(RIGHT_BUTTON))
    {
        gSelection++;
    }

    if(gSelection < 0)
    {
        gSelection = 3;
    }

    if(gSelection > 3)
    {
        gSelection = 0;
    }
}

void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();
    arduboy.clear();

    selection();

    arduboy.display();
}
