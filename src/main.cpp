#include <Arduino.h>
/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)

 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo.

 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)

 Based on clock sketch by Gilchrist 6/2/2014 1.0
 Updated by Bodmer
A few colour codes:

code    color
0x0000  Black
0xFFFF  White
0xBDF7  Light Gray
0x7BEF  Dark Gray
0xF800  Red
0xFFE0  Yellow
0xFBE0  Orange
0x79E0  Brown
0x7E0   Green
0x7FF   Cyan
0x1F    Blue
0xF81F  Pink

 */
#include "config.h"
#include "font.h"
#include "sat3.h"

TTGOClass *ttgo;

uint32_t targetTime = 0;  // for next 1 second timeout

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char *p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6);  // Get H, M, S from compile time

#define background 0xB635
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;
int moove = 46;

void setup(void) {
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  //ttgo->tft->setTextFont(1);
  ttgo->tft->fillScreen(TFT_BLACK);
  //ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK);  // Note: the new fonts do not draw the background colour
  targetTime = millis() + 1000;

  ttgo->tft->init();
  ttgo->tft->setRotation(2);
  ttgo->tft->setSwapBytes(true);

  ttgo->tft->pushImage(0, 0 - moove, 240, 240, sat3);
  ttgo->tft->setTextColor(0x0081, background);
  ttgo->tft->fillRoundRect(48, 107 - moove, 138, 48, 5, background);
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    ss++;  // Advance second
    if (ss == 60) {
      ss = 0;
      omm = mm;
      mm++;  // Advance minute
      if (mm > 59) {
        mm = 0;
        hh++;  // Advance hour
        if (hh > 23) {
          hh = 0;
        }
      }
    }

    if (ss == 0 || initial) {
      initial = 0;
      // ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
      // ttgo->tft->setCursor(8, 52);
      // ttgo->tft->print(__DATE__);  // This uses the standard ADAFruit small font

      // ttgo->tft->setTextColor(TFT_BLUE, TFT_BLACK);
      // ttgo->tft->drawCentreString("It is windy", 120, 48, 2);  // Next size up font 2

      //ttgo->tft->setTextColor(0xF81F, TFT_BLACK); // Pink
      //ttgo->tft->drawCentreString("12.34",80,100,6); // Large font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 . : a p m
    }

    // Update digital time
    byte xpos = 6;
    byte ypos = 0;
    if (omm != mm) {  // Only redraw every minute to minimise flicker
      // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
      // ttgo->tft->setTextColor(0x39C4, TFT_BLACK);     // Leave a 7 segment ghost image, comment out next line!
      // ttgo->tft->setTextColor(TFT_BLACK, TFT_BLACK);  // Set font colour to black to wipe image
      // // Font 7 is to show a pseudo 7 segment display.
      // // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
      ttgo->tft->drawString("88:88", 46, 126 - moove);  // Overwrite the text to clear it
      // ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);     // Orange
      ttgo->tft->setFreeFont(&DSEG7_Classic_Bold_30);
      String hhStr = String(hh);
      if (hhStr.length() == 1) {
        hhStr = "0" + hhStr;
      }
      String mmStr = String(mm);
      if (mmStr.length() == 1) {
        mmStr = "0" + mmStr;
      }
      ttgo->tft->drawString(hhStr + ":" + mmStr, 46, 126 - moove);
      omm = mm;

      // if (hh < 10) xpos += ttgo->tft->drawChar('0', xpos, ypos, 7);
      // xpos += ttgo->tft->drawNumber(hh, xpos, ypos, 7);
      // xcolon = xpos;
      // xpos += ttgo->tft->drawChar(':', xpos, ypos, 7);
      // if (mm < 10) xpos += ttgo->tft->drawChar('0', xpos, ypos, 7);
      // ttgo->tft->drawNumber(mm, xpos, ypos, 7);
    }

    // if (ss % 2) {  // Flash the colon
    //   ttgo->tft->setTextColor(0x39C4, TFT_BLACK);
    //   xpos += ttgo->tft->drawChar(':', xcolon, ypos, 7);
    //   ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
    // } else {
    //   ttgo->tft->drawChar(':', xcolon, ypos, 7);
    // }
    //String d=String("Sun");
    ttgo->tft->setFreeFont(&DSEG7_Classic_Bold_20);
    String ssSTr = String(ss);
    if (ssSTr.length() == 1) {
      ssSTr = "0" + ssSTr;
    }
    ttgo->tft->drawString(ssSTr, 154, 136 - moove);

    ttgo->tft->setFreeFont(&DSEG14_Classic_Bold_18);
    //ttgo->tft->drawString(d.substring(0, 2), 94, 96 - moove);
    ttgo->tft->drawString(String(__DATE__), 50, 96 - moove);
  }
}
