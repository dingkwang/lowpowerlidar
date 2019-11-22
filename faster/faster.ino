// Low power LIDAR program
#define RETURN_PIN   A5  // Arduino pin tied to Return pin on the OSLRF.
#define LRF  4                // the output pin that the Delay Unit is atteched to
#define PIR  2              // the input pin that the PIR sensor is atteched to

#define xp 3
#define xm 5
#define yp 9
#define ym 10

int T = 1000;

int t_state;
int return_val = 0;
float pd[4] = {0};
float dist[4] = {0};
float trans[2][4] = {
  {0.433, 0.458, 0.449, 0.428},
  { -25.05, -29.4, -28.85, -25.55}
};
int vx = 0;
int vy = 0;
int i = 0;
int j = 0;
int k = 0;
int m = 0;
int mi = 0;
float temp = 0;
float ms = 0;
int xy[2][4] = {  // Small Area
  {0, 50, 50, 0},
  {0, 0, 50, 50}
};
float r;

int xyf[2][4] = { // Large Area
  {0, 200, 200, 0},
  {0, 0, 200, 200}
};

int pir_val = 0; // by default, no motion detected
int t = 0;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

#define sclk 13
#define mosi 11
#define cs   7    //Not used 
#define rst  6
#define dc   8


#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

uint16_t colorm[8] = {BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
uint16_t color;
uint16_t blu;
uint16_t red;
uint16_t grn;


// You can use any (4 or) 5 pins

Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);
int16_t dxy[2][4] = {
  {0, 48, 48, 0},
  {32, 32, 0, 0}
};



void setup()
{
  pinMode(LRF, OUTPUT);      // initalize onboard LED as an output
  pinMode(PIR, INPUT);    // initialize PIR as an input

  //  noInterrupts();
  //  CLKPR = _BV(CLKPCE);  // enable change of the clock prescaler
  //  CLKPR = _BV(CLKPS0);  // disvide frequency by 2
  //  interrupts();

  // Configure Timer 0 for phase correct PWM @ 25 kHz.
  TCCR0A = 0;           // undo the configuration done by...
  TCCR0B = 0;           // ...the Arduino core library
  TCNT0  = 0;           // reset timer
  TCCR0A = _BV(COM0B1)  // non-inverted PWM on ch. B
           | _BV(WGM00);  // mode 5: ph. correct PWM, TOP = OCR0A
  TCCR0B = _BV(WGM02)   // ditto
           | _BV(CS00);   // prescaler = 1
  OCR0A  = 255;         // TOP = 160

  // Same for Timer 1.
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch. B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
  TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
  ICR1   = 255;

  // Same for Timer 2.
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  TCCR2A = _BV(COM2B1)  // non-inverted PWM on ch. B
           | _BV(WGM20);  // mode 5: ph. correct PWM, TOP = OCR2A
  TCCR2B = _BV(WGM22)   // ditto
           | _BV(CS20);   // prescaler = 1
  OCR2A  = 255;

  // Set the main system clock to 8 MHz.

  Serial.begin(38400);
  display.begin();
  //Serial.println("Display ON");
  display.fillScreen(WHITE);
  display.setCursor(0, 0);

}

void loop()
{
  t_state = digitalRead(PIR);  // read PIR
  if (t_state == HIGH) {
    t = T;  // Delay time for Motion leave
    if (pir_val == LOW) {
      pir_val = HIGH;
      digitalWrite(LRF, LOW); // LOW is ON
      Serial.println("Motion Detected, wait to turn on LRF");
      delay(250000);
      Serial.println("LRF ON");
    }
  }
  else {
    display.fillScreen(BLACK);
    //t = 10;
    digitalWrite(LRF, HIGH); //HIGH is OFF
    pir_val = LOW;
    Serial.println("OFF");
  }

  while (t > 0) {
    t = t - 1; //Keep scan t freams until stop
    Serial.print("t = ");
    Serial.println(t);
    m = 0;
    temp = 0;
    float pd[4] = {0};

    for (k = 0; k <= 3; k++ ) { //Large Area
      temp = 0;
      for (i = 0; i <= 0; i++) { //Small Area
        vx = xy[0][i] + xyf[0][k];
        vy = xy[1][i] + xyf[1][k];
        analogWrite(xp,  vx);  // duty cycle = 1/160
        analogWrite(xm,  255 - vx); // ~ 1/3
        analogWrite(yp, vy);  // ~ 2/3
        analogWrite(ym, 255 - vy); // 159/160
        //      Serial.print(vx);
        //      Serial.print(',');
        //      Serial.print(vy);
        //      Serial.print(',');
        return_val = 0;
        delay(20000); // Wait for the MEMS mirror to stablize 2000
        for (j = 0; j < 10; j++) { // Take Average of 10 measurement at each scanning location
          return_val = return_val + analogRead(RETURN_PIN);

        }
        temp = temp + return_val / 10;
      }
      pd[m] = pd[m] + temp;  //Average the 4 distance at each large area
      //Serial.print(pd[m]);
      m = m + 1;
    }

    for (i = 0; i <= 3; i++) {
      if (pd[i] >= 50 && pd[i] <= 130) { //Outside the range is invalid
        dist[i] = pd[i] * trans[0][i] + trans[1][i];
        r = 2*(dist[i] - 4) / (30 - 4); // 5cm: min; 25cm: max
        //r = 1 / (1 + pow(r / (1 - r), -3));
        // GREY
        //color = (int16_t)(r * BLUE) + (((int16_t)(r * BLUE)) << 5) + (((int16_t)(r * BLUE)) << 11);
        //RED_BLUE
        if (r > 1) {
          color = BLUE + (((int16_t)((2 - r) * BLUE)) << 11);
        }
        else {
          color = RED + (int16_t)(BLUE * r);
        }
        //delay(500);
        //red = (int16_t)(r * 0x1F);
        //blu = (int16_t)((1-r)*0x1F);
        //testfillrects(dxy[0][i], dxy[1][i], color);
        testfillrects(dxy[0][i], dxy[1][i], color);
        //delay(1000);
        Serial.print("dist = ");
        Serial.print(dist[i]);
        Serial.print(", r =  ");
        Serial.print(r, 2);
        Serial.print(",");
        //Serial.print(", color = ");
        //Serial.print(color, BIN);   //Print the average distnaces in the 4 large Areas
        //Serial.print(',');

      }
      else {
        dist[i] = 0;
        testfillrects(dxy[0][i], dxy[1][i], BLACK);
      }
    }
  }


}

void testfillrects(int16_t x, int16_t y, uint16_t color) {
  //
  int16_t w = 48;
  int16_t h = 32;
  display.fillRect(x, y, w, h, color);

}
