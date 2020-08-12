#include "assets.h"

#include "Models.h"
#include <Arduino.h>
#include <Arduboy2.h>

Models models;
Sprites sprites;
Arduboy2Base arduboy;

const int8_t TOTAL_SCENES = 3;
const int16_t START_ANGLE = 180;

int8_t gSelection  = 0;
int8_t gScene = 0;

int16_t yAngle = START_ANGLE;
uint8_t xPosition = 32;
uint8_t yPosition = 48;

//#define TRACE

void setup()
{
#ifdef TRACE
    Serial.begin(9600);
#endif

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

    yAngle+=arduboy.cpuLoad()/100;

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

int singlePass(int y, uint8_t* pattern)
{
    int score = 0;
    const int STARTX = 48;

    int i = 0;
    int x = STARTX;
    const int32_t length = 32;
    while(i < length)
    {
        uint8_t pixel = arduboy.getPixel(x, y);
        if(pixel !=0/*WHITE*/) // Hit
        {
            score++;
            y--;
        }
        else
        {
            y++;
        }
        if(pattern != nullptr)
        {
            pattern[i] = y;
        }
        x++;
        i++;
    }

    return score;
}

int calculateResistance(uint8_t* pattern)
{
    int score = 0;

    float* modelMap = nullptr;
    uint8_t* vehicle = nullptr;
    uint8_t* name = nullptr;

    int multiplier = 16;
    switch(gSelection)
    {
        case 0:
            modelMap = ndxToValueCar;
            vehicle = car;
            name = overland;
            break;
        case 1:
            modelMap = ndxToValueTruck;
            vehicle = truck;
            name = baja;
            break;
        case 2:
            modelMap = ndxToValueBus;
            vehicle = bus;
            name = burningman;
            break;
        case 3:
             modelMap = ndxToValueBike;
            vehicle = bike;
            name = moto;
            multiplier = 6;
            break;
    }

    models.drawCompressedModel(vehicle, modelMap, 0, 270, 0, 1);

    const int STARTY = 32;
    int y = STARTY;

    singlePass(y, pattern);
    y = STARTY - 8;
    while(y < STARTY + 8)
    {
        if(y != STARTY) // Save a recalcuation
        {
            score += singlePass(y, nullptr);
        }
        y++;
    }
    gScene++;

    return score*multiplier;
}

void tunnel(uint8_t* pattern)
{
    float* modelMap = nullptr;
    uint8_t* vehicle = nullptr;
    uint8_t* name = nullptr;
    switch(gSelection)
    {
        case 0:
            modelMap = ndxToValueCar;
            vehicle = car;
            name = overland;
            break;
        case 1:
            modelMap = ndxToValueTruck;
            vehicle = truck;
            name = baja;
            break;
        case 2:
            modelMap = ndxToValueBus;
            vehicle = bus;
            name = burningman;
            break;
        case 3:
             modelMap = ndxToValueBike;
            vehicle = bike;
            name = moto;
            break;
    }

    models.drawCompressedModel(vehicle, modelMap, 15, yAngle, 0, 1);
    sprites.drawSelfMasked(43, 56, name, 0);

    yAngle = 300;
    uint8_t flex = 0;
    bool resistanceZone = false;
    if(xPosition > 48 && xPosition < 80)
    {
        flex += random() % 16;
        resistanceZone = true;
    }

    if(xPosition % 2 == 0)
    {
        yAngle += flex;
        yPosition--;

    }
    else
    {
        yAngle -= flex;
    }

    if(resistanceZone)
    {
        int i = 0;
        const int32_t length = 32;
        while(i < length)
        {
            uint8_t modify = random() % 3;
            arduboy.drawPixel(48 + i, pattern[i] - 6 + modify);
            i++;
        }
    }
    else
    {
        uint8_t count = 4;
        while(count--)
        {
            uint8_t modify = random() % 8;
            arduboy.drawPixel(xPosition, yPosition + modify, 1);
            int tails = 4;
            while(tails--)
            {
                arduboy.drawPixel(xPosition - tails, yPosition + modify + tails, 1);
            }
        }
    }

    xPosition++;
    if(xPosition >= 96)
    {
        xPosition = 32;
        yPosition = 48;
    }
}

void checkScene()
{
    if(arduboy.justPressed(A_BUTTON))
    {
        gScene--;
        if(gScene == 1) // Render math scene
        {
            gScene--;
        }
    }

    if(arduboy.justPressed(B_BUTTON))
    {
        gScene++;
    }

    if(gScene <= 0)
    {
        gScene = 0;
    }

    if(gScene >= TOTAL_SCENES)
    {
        gScene = TOTAL_SCENES-1;
    }
}

uint8_t pattern[32];
void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();

    int score = 0;
    switch(gScene)
    {
        case 0:
            selection();
            break;
        case 1:
            score = calculateResistance(pattern);
            arduboy.clear();
            tunnel(pattern);
            break;
        case 2:
            tunnel(pattern);
            break;
    }

    checkScene();
    arduboy.display(CLEAR_BUFFER);

#ifdef TRACE
    if(arduboy.everyXFrames(24))
    {
        Serial.println(arduboy.cpuLoad(), DEC);
    }
#endif
}
