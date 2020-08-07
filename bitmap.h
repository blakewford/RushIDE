#pragma once

const unsigned char PROGMEM left[] =
{
// width, height,
32, 32,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xe0, 0xf8, 0xfc, 0xfc, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xc0, 0xe0, 0xe0, 0xf0, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char PROGMEM right[] =
{
// width, height,
32, 32,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x0f, 0x07, 0x07, 0x03, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x3f, 0x3f, 0x1f, 0x07, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char PROGMEM overland[] =
{
// width, height,
42, 7,
0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x24, 0x22, 0x32, 0x0e, 0x04, 0x3c, 0x10, 0x0c, 0x30, 0x1c, 0x2c, 0x00, 0x3c, 0x04, 0x04, 0x00, 0x3f, 0x10, 0x28, 0x24, 0x3c, 0x00, 0x3c, 0x04, 0x3c, 0x00, 0x38, 0x24, 0x24, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char PROGMEM baja[] =
{
// width, height,
42, 7,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x2a, 0x3e, 0x00, 0x18, 0x14, 0x1c, 0x50, 0x7e, 0x00, 0x18, 0x14, 0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char PROGMEM burningman[] =
{
// width, height,
42, 7,
0x00, 0x3e, 0x2a, 0x3e, 0x00, 0x3c, 0x20, 0x3c, 0x00, 0x3c, 0x04, 0x00, 0x3c, 0x04, 0x3c, 0x00, 0x3a, 0x00, 0x3c, 0x04, 0x3c, 0x00, 0x4c, 0x54, 0x7c, 0x00, 0x00, 0x3c, 0x0e, 0x10, 0x0c, 0x3e, 0x00, 0x38, 0x24, 0x3c, 0x20, 0x00, 0x3c, 0x04, 0x3c, 0x00, 
};

const unsigned char PROGMEM moto[] =
{
// width, height,
42, 7,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3c, 0x0e, 0x10, 0x0c, 0x3e, 0x00, 0x3c, 0x24, 0x1c, 0x00, 0x04, 0x3c, 0x04, 0x38, 0x24, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};