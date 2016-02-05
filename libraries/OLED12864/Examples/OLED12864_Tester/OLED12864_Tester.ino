#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>

// About changes of PROGMEM
//   1.6.0: https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions
//   1.6.5: http://forum.arduino.cc/index.php?topic=339011.0

#include <Wire.h>
#include "OLED12864.h"

#include "udf.c"
#include "msg.c"

const char msg_welcomeHeader[] PROGMEM = "Welcome OLED12864";
const char msg_welcomeMsg[] PROGMEM = "This is a tesing for OLED12864 Library, it will demonstrate most common features in OLED12864 library.";

char sram_welcomeMsg[] = "This is a tesing for OLED12864 Library, it will demonstrate most common features in OLED12864 library.";

const char font_demo_small[] PROGMEM = "Small: 6x8: Abc";
const char font_demo_large[] PROGMEM = "Large: 8x16: Abc";


const char msg_1[] PROGMEM = "[1] A long message is memory consumption, it's suggested to put them in flash memory instead of SRAM";
const char msg_2[] PROGMEM = "[2] By using the flash, a long message can be stored for display.";
const char msg_3[] PROGMEM = "[3] Another dummy long long long long long text in Flash, it can be much longer than the one in SRAM.  As the flash memory can have 32KB while SRAM only have 2KB,  it's recommended to use flash instead of SRAM for static text.  Good luck!";

const char * const textmsg[] PROGMEM  = {msg_1, msg_2, msg_3};
// const char const * textmsg[] PROGMEM  = {msg_1, msg_2, msg_3};

const char tname_0[] PROGMEM = "Demonstration on";
const char tname_1[] PROGMEM = "Embeded Fonts";
const char tname_2[] PROGMEM = "User Defined Font";
const char tname_3[] PROGMEM = "Scrolling Display";
const char tname_4[] PROGMEM = "Bitmap Display";
const char tname_5[] PROGMEM = "Simple Drawing";
const char tname_6[] PROGMEM = "Plotter";
const char tname_7[] PROGMEM = "Numeric Display";
const char tname_8[] PROGMEM = "Mixed Display";
const char tname_9[] PROGMEM = "DirectDraw";

const char * const tname[] PROGMEM  = {tname_0, tname_1, tname_2, tname_3, tname_4, tname_5, tname_6, tname_7, tname_8, tname_9};
// const char const * tname[] PROGMEM  = {tname_0, tname_1, tname_2, tname_3, tname_4, tname_5, tname_6, tname_7, tname_8, tname_9};

#define TN_HEADER   0
#define TN_FONT     1
#define TN_UDF      2
#define TN_TEXT     3
#define TN_BITMAP   4
#define TN_DRAWING  5
#define TN_PLOTTER  6
#define TN_NUMERIC  7
#define TN_MIXTEXT  8
#define TN_DIRECTDRAW  9


boolean enableBuffer = true;
boolean directDraw = true;

//  OLED_1306i2c - 0.96" OLED
//  OLED_1106i2c - 1.3" OLED
OLED12864 myOLED(OLED_1306i2c, enableBuffer, directDraw);

void setup()
{
  
  Serial.begin(57600);
  delay(1000);
  Serial.println("OLED12864 Tester");

  randomSeed(analogRead(0));  
  
  checkMemory();

  myOLED.begin();

  welcome();

  checkMemory();

}

void checkMemory() {
//  Serial.print("## memory: ");
//  Serial.println(myUtil::availableMemory());
}

void loop()
{

  myOLED.clr();

  demoFont();

  demoUDF();

  demoText();

  demoNumeric();

  demoBITMAP();

  demoDrawing();

  demoPlotter();

  demoDirectDraw();

  checkMemory();

  delay(2000);

}

void welcome() 
{
  myOLED.clr();
  OLEDshow();
  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsg(0,0,(char *) msg_welcomeHeader);
  myOLED.printFlashMsg(0,3,(char *) msg_welcomeMsg);
  OLEDshow();
  delay(3000);

}

void showDemoText(uint8_t tcode)
{
  Serial.println("showDemoText");
  myOLED.clr();
  OLEDshow();
  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsgArr(0,1, &(tname[TN_HEADER]));
  myOLED.printFlashMsgArr(0,4, &(tname[tcode]));
  OLEDshow();
  delay(1000);
  myOLED.clr();
  OLEDshow();
}

void demoFont() 
{
  Serial.println("demoFont");
  showDemoText(TN_FONT);

  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsgArr(0,0, &(tname[TN_FONT]));

  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsg(0, 2, (char *) font_demo_small);
  myOLED.print(0, 5, "1234");

  myOLED.setFont(OLED_font8x16);
  myOLED.printFlashMsg(0, 3, (char *) font_demo_large);

  myOLED.setFont(OLED_fontNum);
  myOLED.print(0, 6, "1234");

  myOLED.setFont(OLED_fontBigNum);
  myOLED.setInverse(true);
  myOLED.print(64, 5, "1234");
  myOLED.setInverse(false);

  OLEDshow();
  delay(3000);
}


void demoUDF() 
{
  showDemoText(TN_UDF);
  char msg[6] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x00};
  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsgArr(0,0, &(tname[TN_UDF]));
  myOLED.setFont(CN16x16, false);
  for (int i=2; i < 7; i += 2) myOLED.print((i-2) * 10, i, msg);
  OLEDshow();
  delay(3000);
}

void demoText()
{
  showDemoText(TN_TEXT);
  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsgArr(0,0, &(tname[TN_TEXT]));

//  char num[] = "12345";
//  Comment out this long text to save memory
//  char msg[] = "Testing on printing a very long text which require multiple scrolling and sometime it takes time to scroll the screen.";
//  char msg[] = "A dummy long long long long text ....";

  myOLED.setFont(OLED_font6x8);
  for (int i = 0; i < 3; i++)
    myOLED.printFlashMsgArr(&(textmsg[i]));

  myOLED.setFont(OLED_font8x16);
  for (int i = 0; i < 3; i++)
    myOLED.printFlashMsgArr(&(textmsg[i]));
  OLEDshow();
  delay(3000);

}


void demoNumeric()
{
  showDemoText(TN_NUMERIC);

  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsgArr(0,0, &(tname[TN_NUMERIC]));
  
  myOLED.print(0, 2, (long) random(0,1000000), 0, false);
  myOLED.print(64, 2, (int) random(0,32767), 5, true);
  myOLED.println(0, 3, "1.2345: (2) : (5,8)");
  myOLED.printFloat(1.2345,2);
  myOLED.print(" : ");
  myOLED.printFloat(1.2345, 5, 8);
  myOLED.println(0, 6, "Display HEX 0xF");
  myOLED.printHex(0,7, 0xF, 0, false);
  myOLED.print(":");
  myOLED.printHex(0xF, 2, false);
  myOLED.print(":");
  myOLED.printHex(0xF, 2, true);
  myOLED.print(":");
  myOLED.printHex(0xF, 3, false);
  myOLED.print(":");
  myOLED.printHex(0xF, 3, true);
  myOLED.print(":");
  myOLED.printHex(0xF, 4, true);

/*
  myOLED.setFont(OLED_fontBigNum);
  for (int i= 0; i < 100; i++) 
  {
    myOLED.print(64, 5, i, 4);
    delay((i < 10 ? 200 : ( i < 30 ? 50 : 0)));
  }
  delay(1000);
*/  
  // to verify the buffer
  OLEDshow();
  delay(3000);
  
}


void demoBITMAP() 
{
  // Display BMP as font
  showDemoText(TN_BITMAP);

  myOLED.setFont(BMP12864, false);

  myOLED.print(0,0, "0");
  OLEDshow();
  delay(2000);
  
  myOLED.clr();
  OLEDshow();
  myOLED.print(0,0, "1");
  OLEDshow();
  delay(2000);
}

void drawBase()
{
  myOLED.clr();
  myOLED.drawRect(64,0,127,31,0,1);
  myOLED.drawRect(0,32,63,63,0,1);
  
  // OuterFrame 
  myOLED.drawRect(0,0,127,63,0,0);
  myOLED.drawRect(1,1,126,62,1,0);
  myOLED.drawRect(2,2,125,61,0,0);
}

void demoDrawing()
{
  showDemoText(TN_DRAWING);
  unsigned long t = micros();  

  drawBase();

  myOLED.drawVLine(16, 8,56,0);
  myOLED.drawVLine(32, 8,56,1);
  myOLED.drawVLine(48, 8,56,2);

  myOLED.drawVLine(79, 8,56,0);
  myOLED.drawVLine(95, 8,56,1);
  myOLED.drawVLine(111, 8,56,2);

  myOLED.drawHLine(16, 111, 8, 0);
  myOLED.drawHLine(16, 111, 16, 1);
  myOLED.drawHLine(16, 111, 24, 2);

  myOLED.drawHLine(16, 111, 40, 0);
  myOLED.drawHLine(16, 111, 48, 1);
  myOLED.drawHLine(16, 111, 56, 2);

  unsigned long d1, d2;  
  d1 = micros() - t;  

  OLEDshow();
  d2 = micros() - t - d1;  
  Serial.print("testLine: ");
  Serial.print(d1);
  Serial.print(" : ");
  Serial.println(d2);
  
  delay(2000);

  demoRect();
}


void demoRect()
{
  unsigned long t = micros();  
  
  drawBase();

  myOLED.drawRect(16,8,111,55,0,0);
  myOLED.drawRect(32,16,95,47,1,0);
  myOLED.drawRect(44,20,83,43,2,1);
  myOLED.drawRect(46,24,79,39,2,1);

  unsigned long d1, d2;  
  d1 = micros() - t;  
  
  OLEDshow();
  d2 = micros() - t - d1;  
  
  delay(3000);

  myOLED.clr();
  OLEDshow();
}

void demoPlotter()
{
  showDemoText(TN_PLOTTER);
  myOLED.plotter(-100,100,-20,120,0,0,true);
  myOLED.plotterDraw(0,100);
  myOLED.plotterDrawTo(60,0);
  myOLED.plotterDrawTo(-70,70);
  myOLED.plotterDrawTo(70,70);
  myOLED.plotterDrawTo(-60,0);
  myOLED.plotterDrawTo(0,100);
  myOLED.plotterReset();
  for (int x = - 100; x < 100; x++) myOLED.plotterDrawTo(x, x * x / 80);
  OLEDshow();
  delay(3000);
}

void demoMix()
{
  showDemoText(TN_NUMERIC);

  myOLED.setFont(OLED_font6x8);
  myOLED.printFlashMsgArr(0,0, &(tname[TN_NUMERIC]));
  
  myOLED.print(0, 2, (long) random(0,1000000), 0, false);
  myOLED.print(64, 2, (int) random(0,32767), 5, true);
  myOLED.println(0, 3, "1.2345: (2) : (5,8)");
  myOLED.printFloat(1.2345,2);
  myOLED.print(" : ");
  myOLED.printFloat(1.2345, 5, 8);
  myOLED.println(0, 6, "Display HEX 0xF");
  myOLED.printHex(0,7, 0xF, 0, false);
  myOLED.print(":");
  myOLED.printHex(0xF, 2, false);
  myOLED.print(":");
  myOLED.printHex(0xF, 2, true);
  myOLED.print(":");
  myOLED.printHex(0xF, 3, false);
  myOLED.print(":");
  myOLED.printHex(0xF, 3, true);
  myOLED.print(":");
  myOLED.printHex(0xF, 4, true);

/*
  myOLED.setFont(OLED_fontBigNum);
  for (int i= 0; i < 100; i++) 
  {
    myOLED.print(64, 5, i, 4);
    delay((i < 10 ? 200 : ( i < 30 ? 50 : 0)));
  }
  delay(1000);
*/  
  // to verify the buffer
  OLEDshow();
  delay(3000);
  
}

void demoDirectDraw()
{
  showDemoText(TN_DIRECTDRAW);
  myOLED.setDirectDraw(true);
  myOLED.print(0,0,"DirectDraw 0.1");
  for (int i = 0; i < 127; i++) {
    showBar(i);
  }
  delay(1000);
  for (int i = 127; i >= 0;  i--) {
    showBar(i);
  }
  delay(3000);
}

void showBar(uint8_t val) 
{
  boolean b = myOLED.setDirectDraw(false);
  myOLED.clr(2);
  myOLED.drawRect(0,16,127,23, OLED_MODE_DRAW, OLED_STYLE_EMPTY);
  myOLED.drawRect(0,17,val,22, OLED_MODE_DRAW, OLED_STYLE_FILL);
  myOLED.show(2);
  myOLED.setDirectDraw(b);
}

void OLEDshow() {
  if (!directDraw) myOLED.show();
}
