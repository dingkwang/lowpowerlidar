int fadex = 3;         // how many points to fade the LED by
float fadey = 1;         // how many points to fade the LED by
int vx = 0;         // how bright the LED is
float vyi = 0;
int vy = 0;         // how bright the LED is

#define xp 3
#define xm 5
#define yp 6
#define ym 10

void setup()
{
  // Set the main system clock to 8 MHz.
  Serial.begin(9600); // open the serial port at 9600 bps:
  noInterrupts();
  CLKPR = _BV(CLKPCE);  // enable change of the clock prescaler
  CLKPR = _BV(CLKPS0);  // divide frequency by 2
  interrupts();

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
}

void loop()
{
  Serial.print(11);  // prints a label
  Serial.print("\t");      // prints a tab
  
  //analogWrite( xp,  vx);  // duty cycle = 1/160
  //analogWrite( xm,  255-vx);  // ~ 1/3
  //analogWrite( yp, vy);  // ~ 2/3
  analogWrite(ym, 255-vy);  // 159/160

  vx = vx + fadex;

  
  vyi = vyi+fadey;
  vy = (int)vyi;

  if (vx == 0 || vx == 255) {
    fadex = -fadex ;
  }
  if (vyi == 0 || vyi == 255) {
    fadey = -fadey ;
  }
  //delay(100);
}
