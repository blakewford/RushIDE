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
            x++;
            y++;
            if(pattern != nullptr)
            {
                pattern[i] = y;
            }
            i++;
        }
    }

    return score;
}

void renderModelProfile()
{
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
}

int calculateResistance(uint8_t* pattern)
{
    int score = 0;

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

    int multiplier = 16;
    switch(gSelection)
    {
        case 3:
            multiplier = 6;
            break;
    }

    return score*multiplier;
}

uint8_t pointerX = 0;
int8_t findPixel()
{
    uint8_t y = 1;
    uint8_t value = 0;
    while(value == 0 && y < 64)
    {
        y++;
        value = arduboy.getPixel(pointerX, y);
    }

    if(value) return y;

    return -1;
}

void modify()
{
    if(arduboy.pressed(LEFT_BUTTON))
    {
        arduboy.drawPixel(pointerX, 0, 0);
        if (pointerX > 0)
            pointerX--;
        else
            pointerX = 0;
    }

    if(arduboy.pressed(RIGHT_BUTTON))
    {
        arduboy.drawPixel(pointerX, 0, 0);
        if (pointerX < 127)
            pointerX++;
        else
            pointerX = 127;
    }

    arduboy.drawPixel(pointerX, 0, 1);

    int8_t y = findPixel();
    if(arduboy.pressed(UP_BUTTON) && y != -1)
    {
        if(y > 2)
        {
            y--;
            arduboy.drawPixel(pointerX, y, 1);
        }
    }

    if(arduboy.pressed(DOWN_BUTTON) && y != -1)
    {
        if(arduboy.getPixel(pointerX, y+1))
        {
            arduboy.drawPixel(pointerX, y, 0);
        }
    }
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
    int8_t previous = gScene;
    if(arduboy.justPressed(A_BUTTON))
    {
        gScene--;
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

    if(previous != gScene && gScene == 1)
    {
        arduboy.clear();
        renderModelProfile();
    }
}

void drawScore(int score)
{
    const int16_t x = 128;
    const int16_t y = 44;

    char buffer[6];
    ltoa(score, buffer, 10);

    const uint8_t zero = '0';
    uint8_t count = sizeof(buffer)-1;
    uint8_t offset = 5;
    while(count--)
    {
        uint8_t value = buffer[count];
        if (value >= '0' && value <= '9')
        {
            sprites.drawSelfMasked(x - offset, y, numbers, value - zero);
        }
        offset +=5;
    }
}

int gScore = 0;
uint8_t pattern[32];
void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();

    switch(gScene)
    {
        case 0:
            selection();
            pointerX = 0;
            break;
        case 1:
            gScore = calculateResistance(pattern);
            modify();
            break;
        case 2:
            tunnel(pattern);
            drawScore(gScore);
            break;
    }

    checkScene();
    if(gScene != 1)
    {
        arduboy.display(CLEAR_BUFFER);
    }
    else
    {
        arduboy.display(false);
    }

#ifdef TRACE
    if(arduboy.everyXFrames(24))
    {
        Serial.println(arduboy.cpuLoad(), DEC);
    }
#endif
}
