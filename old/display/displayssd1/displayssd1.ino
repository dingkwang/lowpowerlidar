/***************************************************
  This is a example sketch demonstrating the graphics
  capabilities of the SSD1331 library  for the 0.96"
  16-bit Color OLED with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/684

  These displays use SPI to communicate, 4 or 5 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>


// You can use any (4 or) 5 pins
#define sclk 13
#define mosi 11
#define cs   7 //Not used 
#define rst  6
#define dc   8


// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

// Option 1: use any pins but a little slower
//Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);

float p = 3.1415926;
int16_t dxy[2][4] = {
    {0, 48, 0, 48},
    {0, 0, 32, 32}
};
int i ;
int m = 0;
uint16_t colorm[8] = {BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
uint16_t color = 0;


void setup(void) {
  TCCR0A = 0;           // undo the configuration done by...
  TCCR0B = 0;           // ...the Arduino core library
  TCNT0  = 0;           // reset timer
  TCCR0A = _BV(COM0B1)  // non-inverted PWM on ch. B
           | _BV(WGM00);  // mode 5: ph. correct PWM, TOP = OCR0A
  TCCR0B = _BV(WGM02)   // ditto
           | _BV(CS00);   // prescaler = 1
  OCR0A  = 255;         // TOP = 160

  
  
  
  Serial.begin(38400);
  display.begin();

  uint16_t time = millis();
  
  

  display.fillScreen(WHITE);
  display.setCursor(0, 0);
 
  //testfillrects(YELLOW, MAGENTA);
  Serial.println("done");
  
}

void loop() {
  //
  //color = (int16_t)(0.5*0xFFFF);
  
  for (color = 0xF800; color <=0xFFFF; 1){
    Serial.println(color, HEX);
    for (i = 0;i <=3; i++){
   // colorm[m] = color+0x0500;
      color = color +5;
      m = m+1;
      testfillrects(dxy[0][i], dxy[1][i], color);
    }
    delay(200000);
    display.fillScreen(BLACK);
  }
  
}




void testfillrects(int16_t x, int16_t y, uint16_t color){
  //
  int16_t w = 48; 
  int16_t h = 32; 
  display.fillRect(x, y, w, h, color);

}
