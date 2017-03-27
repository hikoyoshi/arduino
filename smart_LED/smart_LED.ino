#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN    6 // PIN number for strip 
#define CPIN   7 // PIN number for RING 

#define BLUE 21  //藍色燈從 21 ~ 15
#define ORG  14  //橘色燈從 14 ~ 8
#define RED   7  //紅色燈從 7 ~ 1

#define COUNTS 21  // 燈條總數 
#define RCOUNTS 8  // 環形總數

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(COUNTS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel cpixels = Adafruit_NeoPixel(CCOUNTS, CPIN, NEO_GRB + NEO_KHZ800);

SoftwareSerial BT(10, 11); //RX:10 TX:11
int delayval = 800; // delay time

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.
  cpixels.begin(); // This initializes the NeoPixel library.

  CLEAR_LED();
  CLEAR_RING_LED();

  for (int x = 0; x < 5; x++) { // 狀態1：開機時閃爍 5次
    for (int i = 0; i < RCOUNTS; i++) {
      cpixels.setPixelColor(i, cpixels.Color(0, 10, 0)); // setting ring Led color to green.
      cpixels.show();
    }
    delay(500);
    CLEAR_RING_LED();
    delay(300);
  }
  Serial.begin(9600);
  BT.begin(9600);  // 藍牙
}

void loop() {
  byte cmmd[20];
  int insize;    //宣告變數insize為一整數

  while (1) {
    if ((insize = (BT.available())) > 0)
    { //判斷有沒有訊息接收
      Serial.print("input size = ");
      Serial.println(insize); //顯示接收多少訊息
      for (int i = 0; i < insize; i++) {
        Serial.print(cmmd[i] = char(BT.read())); //將接收的訊息顯示出來
        Serial.println(" ");
      }//for
    } //if

    switch (cmmd[0]) {
      case 97:
        // 代碼A ，已處理狀態
        Serial.println(cmmd[0]);
        pixels.clear();
        CLEAR_RING_LED();
        RING_COLORS(2);
        SMART_STRIP(20);
        delay(delayval);
        break;

      case 98:
        // 代碼B ，未處理狀態與容量過滿
        Serial.println(cmmd[0]);
        pixels.clear();
        CLEAR_RING_LED();
        RING_COLORS(1);
        SMART_STRIP(6);
        delay(delayval);
        break;

      case 99:
        Serial.println(cmmd[0]);
        pixels.clear();
        SMART_STRIP(6);
        delay(delayval);
        break;

      case 100:
        // 代碼D ，白血球2級狀態
        Serial.println(cmmd[0]);
        pixels.clear();
        CLEAR_RING_LED();
        RING_COLORS(0);
        SMART_STRIP(16);
        delay(delayval);
        break;

      case 101:
        // 代碼E ，白血球3級狀態
        Serial.println(cmmd[0]);
        pixels.clear();
        CLEAR_RING_LED();
        RING_COLORS(1);
        SMART_STRIP(16);
        delay(delayval);
        break;

      case 102:
        // 代碼F ，正常狀態
        Serial.println(cmmd[0]);
        pixels.clear();
        CLEAR_RING_LED();
        RING_COLORS(2);
        SMART_STRIP(16);
        delay(delayval);
        break;
      case 111:
        // 代碼 ○ ，配對成功的狀態
        Serial.println(cmmd[0]);
        CLEAR_LED();
        CLEAR_RING_LED();
        RING_PAIR();
        delay(delayval);
        break;
      default:
        BT.print(cmmd[0]);
        CLEAR_LED();
        CLEAR_RING_LED();
        delay(delayval);
        break;
    }
  }//while
}
void CLEAR_LED() { // clear LED
  for (int i = 0; i <= BLUE; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0, 10)); // Moderately bright green color.
    pixels.show();
  }
}

void CLEAR_RING_LED() { // clear ring LED
  for (int i = 0; i < RCOUNTS; i++) {
    cpixels.setPixelColor(i, cpixels.Color(0, 0, 0)); // Moderately bright green color.
    cpixels.show();
  }
}

void SMART_STRIP(int num) {
  if (num < BLUE && num > ORG ) {
    for (int i = BLUE; i >= num; i--) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 8)); // setting 21~15 Led color to blue
      pixels.show();
    }
  } else if (num == ORG) {
    for (int i = BLUE; i >= ORG; i--) {
      pixels.setPixelColor(i, pixels.Color(15, 8, 0)); // setting 14 Led color to yellow.
      pixels.show();
    }
  } else if (num < ORG && num > RED) {
    for (int i = BLUE; i >= num; i--) {
      pixels.setPixelColor(i, pixels.Color(15, 8, 0)); // setting 21~8 Led color to yellow.
      pixels.show();
    }
  } else if (num == RED) {
    for (int i = BLUE; i >= num; i--) {
      pixels.setPixelColor(i, pixels.Color(8, 0, 0)); // setting 21 ~ 7 Led color to RED.
      pixels.show();
    }
  } else if (num < RED && num > 0) {
    for (int i = BLUE; i >= num; i--) {
      pixels.setPixelColor(i, pixels.Color(8, 0, 0)); //setting 21~ 1 Led color to RED.
      pixels.show();
    }
  }
}

void RING_COLORS(int state) {
  // 0 是橘色，1是紅色，2是綠色
  if (state == 0) {
    for (int i = 0; i < RCOUNTS; i++) {
      cpixels.setPixelColor(i, cpixels.Color(45, 10, 0)); // setting ring Led color to green.
      cpixels.show();
    }
  } else if (state == 1) {
    for (int i = 0; i < RCOUNTS; i++) {
      cpixels.setPixelColor(i, cpixels.Color(8, 0, 0)); // setting ring Led color to green.
      cpixels.show();
    }
  } else if (state == 2) {
    for (int i = 0; i < RCOUNTS; i++) {
      cpixels.setPixelColor(i, cpixels.Color(0, 10, 0)); // setting ring Led color to green.
      cpixels.show();
    }
  }
}

void RING_PAIR() {  // 持續亮綠色  -- 配對成功
  for (int i = 0; i < RCOUNTS; i++) {
    cpixels.setPixelColor(i, cpixels.Color(0, 10, 0)); // setting ring Led color to green.
    cpixels.show();
  }
  delay(200);
}


