#include "assets.h"

#include "Models.h"
#include <Arduino.h>
#include <Arduboy2.h>

Models models;
Sprites sprites;
Arduboy2Base arduboy;

const int8_t TOTAL_SCENES = 5;
const int8_t SKIP_DRAW_SCENE = 3;
const int16_t START_ANGLE = 180;

int8_t gPowertrain  = 0;
int8_t gSelection  = 0;
int8_t gScene = 0;

int16_t yAngle = START_ANGLE;
uint8_t xPosition = 32;
uint8_t yPosition = 40;

//#define TRACE

void setup()
{
#ifdef TRACE
    Serial.begin(9600);
#endif

    arduboy.boot();
    arduboy.setFrameRate(24);
    arduboy.initRandomSeed();

    models.begin();
}

void drawElements(const uint8_t* vehicle, const float* modelMap, const uint8_t* name)
{
    const int16_t xAngle = 15;
    models.drawCompressedModel(vehicle, modelMap, nullptr, xAngle, yAngle, 0);

    const int16_t x = 43;
    const int16_t y = 56;   
    sprites.drawSelfMasked(x, y, name, 0); 
}

void splash()
{
    sprites.drawSelfMasked(32, 20, logo, 0);
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

void powertrain()
{
    if(gSelection == 3)
    {
        gPowertrain = 2;
        arduboy.drawPixel(4, 30, 1);
        sprites.drawSelfMasked(0, 0, powersport, 0);
    }
    else
    {
        if(arduboy.justPressed(UP_BUTTON))
        {
            gPowertrain = 0;
        }
        if(arduboy.justPressed(DOWN_BUTTON))
        {
            gPowertrain = 1;
        }

        if(gPowertrain == 0)
        {
            arduboy.drawPixel(4, 18, 1);
        }
        else
        {
            arduboy.drawPixel(4, 42, 1);
        }

        sprites.drawSelfMasked(0, 0, engine, 0);
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
    const float* modelMap = nullptr;
    const uint8_t* vehicle = nullptr;
    const uint8_t* name = nullptr;

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

    models.drawCompressedModel(vehicle, modelMap, nullptr, 0, 270, 0);
}

int calculateWeight()
{
    int weight = 0;
    for(int x = 0; x < 128; x++)
    {
        for(int y = 0; y < 64; y++)
        {
            weight += arduboy.getPixel(x, y) > 0 ? 1: 0;
        }
    }

    return weight;
}

int calculatePowerToWeight()
{
    float weight = calculateWeight();
    if(gSelection == 0)
    {
       weight *= 4;
    }
    else if(gSelection != 3)
    {
       weight *= 5;
    }


    int32_t power = 1;
    switch(gPowertrain)
    {
        case 0:
            power = 17200;
            break;
        case 1:
            power = 22350;
            weight += 150;
            break;
        case 2:
            power = 7450;
            break;
        default:
            power = weight;
            break;
    }

    return (power/weight)*10;
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

uint8_t pointerX = 32;
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

int gSpaceLeft = 0;
void modify()
{
    if(arduboy.pressed(LEFT_BUTTON))
    {
        arduboy.drawPixel(pointerX, 0, 0);
        if (pointerX > 32)
            pointerX--;
        else
            pointerX = 32;
    }

    if(arduboy.pressed(RIGHT_BUTTON))
    {
        arduboy.drawPixel(pointerX, 0, 0);
        if (pointerX < 96)
            pointerX++;
        else
            pointerX = 96;
    }

    arduboy.drawPixel(pointerX, 0, 1);

    int8_t y = findPixel();
    if(arduboy.pressed(UP_BUTTON) && y != -1)
    {
        if(y > 2)
        {
            y--;
            arduboy.drawPixel(pointerX, y, 1);
            gSpaceLeft++;
        }
    }

    if(arduboy.pressed(DOWN_BUTTON) && y != -1 && gSpaceLeft)
    {
        if(arduboy.getPixel(pointerX, y+1))
        {
            arduboy.drawPixel(pointerX, y, 0);
            gSpaceLeft--;
        }
    }
}

void tunnel(uint8_t* pattern)
{
    const float* modelMap = nullptr;
    const uint8_t* vehicle = nullptr;
    const uint8_t* name = nullptr;
    const uint8_t* fill = nullptr;
    switch(gSelection)
    {
        case 0:
            modelMap = ndxToValueCar;
            vehicle = car;
            name = overland;
            fill = fillCar2;
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
            fill = fillBus2;
            break;
        case 3:
             modelMap = ndxToValueBike;
            vehicle = bike;
            name = moto;
            break;
    }

    models.drawCompressedModel(vehicle, modelMap, fill, 15, yAngle, 0);

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
        if(flex == 0)
        {
            yPosition--;
        }
        else
        {
            yPosition = 28;
        }
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
            i+=2;
        }
    }
    else
    {
        uint8_t count = 4;
        while(count--)
        {
            uint8_t modify = random() % 8;
            arduboy.drawPixel(xPosition, yPosition + modify, 1);
        }
    }

    xPosition+=arduboy.cpuLoad()/100;
    if(xPosition >= 96)
    {
        xPosition = 32;
        yPosition = 40;
    }
}

const int8_t xRoadPositionReset = 30;
int8_t xRoadPosition = xRoadPositionReset;
void road()
{
    const float* modelMap = nullptr;
    const uint8_t* vehicle = nullptr;
    const uint8_t* name = nullptr;
    const uint8_t* fill = nullptr;
    switch(gSelection)
    {
        case 0:
            modelMap = ndxToValueCar;
            vehicle = car;
            name = overland;
            fill = fillCar1;
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
            fill = fillBus1;
            break;
        case 3:
             modelMap = ndxToValueBike;
            vehicle = bike;
            name = moto;
            break;
    }

    yAngle = 215;
    yAngle += random() % 4;

    models.drawCompressedModel(vehicle, modelMap, fill, 15, yAngle, 0);
    arduboy.drawLine(10, 0, 74, 63, 1);
    arduboy.drawLine(45, 0, 109, 63, 1);

    int8_t x = xRoadPosition;
    int8_t y = xRoadPosition - xRoadPositionReset;
    arduboy.drawLine(x, y, x+1, y+1, 1);
    for(int i = 0; i < 6; i++)
    {
        x+=10;
        y+=10;
        arduboy.drawLine(x, y, x+1, y+1, 1);
    }
    xRoadPosition+=arduboy.cpuLoad()/100;

    if(y >= 63) xRoadPosition = xRoadPositionReset;
}

int gScore = 0;
int gRatio = 0;
uint8_t pattern[32];
void checkScene()
{
    int8_t previous = gScene;
    if(arduboy.justPressed(A_BUTTON))
    {
        gScene++;
    }

    if(arduboy.justPressed(B_BUTTON))
    {
        gScene--;
    }

    if(gScene <= 0)
    {
        gScene = 0;
    }

    if(gScene >= TOTAL_SCENES)
    {
        gScene = TOTAL_SCENES-1;
    }

    if(previous != gScene && gScene == SKIP_DRAW_SCENE)
    {
        arduboy.clear();
        renderModelProfile();
    }

    if(previous != gScene && gScene == 3)
    {
        gSpaceLeft = 34;
        if(gPowertrain == 0)
        {
            gSpaceLeft = 48;
        }
    }

    if(previous == 3 && gScene == 4)
    {
        gRatio = calculatePowerToWeight();
        gScore = calculateResistance(pattern);
    }
}

void drawScore(int score, const int16_t y)
{
    const int16_t x = 128;

    char buffer[6] = {0};
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

bool gToggle = false;
void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();

    if(arduboy.everyXFrames(96))
    {
        gToggle = !gToggle;
    }

    switch(gScene)
    {
        case 0:
            splash();
            break;
        case 1:
            selection();
            pointerX = 32;
            gPowertrain = 0;
            break;
        case 2:
            powertrain();
            break;
        case 3:
            modify();
            break;
        case 4:
            if(gToggle)
            {
                tunnel(pattern);
            }
            else
            {
                road();
            }
            sprites.drawSelfMasked(96, 5, label, 0);
            sprites.drawSelfMasked(96, 22, wind, 0);
            drawScore(gRatio, 15);
            drawScore(gScore, 32);
            break;
    }
    checkScene();
    if(gScene != SKIP_DRAW_SCENE)
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
