// Low power LIDAR program

#define ZERO_PIN     4  // Arduino pin tied to Zero   pin on the OSLRF.
#define RETURN_PIN   A5  // Arduino pin tied to Return pin on the OSLRF.

int return_val = 0;
float dist = 0;
int vx = 0;         // how bright the LED is
int vy = 0;         // how bright the LED is
int i = 0;
int j = 0;


int led = 13;                // the pin that the LED is atteched to
int PIR = 2;              // the pin that the sensor is atteched to
int state = LOW;             // by default, no motion detected
int val, val2 = 0;                  // variable to store the sensor status (value)


void setup()
{
  
  pinMode(led, OUTPUT);      // initalize onboard LED as an output
  pinMode(PIR, INPUT);    // initialize PIR as an input
  // Set the main system clock to 8 MHz.
  //Serial.begin(9600); // open the serial port at 9600 bps:
  noInterrupts();
  CLKPR = _BV(CLKPCE);  // enable change of the clock prescaler
  CLKPR = _BV(CLKPS0);  // disvide frequency by 2
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

  // Set the main system clock to 8 MHz.

  Serial.begin(38400);
}

void loop()
{
  //takes a reading and then waits for a few
  /*
    val1 = digitalRead(sensor);   // read sensor value
    delay(5000);
    val2 = digitalRead(sensor);
    if (val1 == val2)
    {           // check if the sensor is HIGH
      digitalWrite(led, HIGH);   // turn LED ON
      delay(100);                // delay 100 milliseconds
      Serial.println("Motion detected!");
    }
    else
    {
        digitalWrite(led, LOW); // turn LED OFF
        delay(200);             // delay 200 milliseconds
      Serial.println("Motion stopped!");
    }
  */
  // Detect Zero signal rising edge

  //zero_val = digitalRead(ZERO_PIN);

  // Detect Return signal rising edge based on previous measurement amplitude
  
//  Serial.print(zero_val * 1000);
//  Serial.print(',');
  
//

//
//  vx = vx + fadex;
//
//  //vyi = vyi + fadey;
//  //vy = (int)vyi;
//
//  if (vx == 50 || vx == 200)
//  {
//    fadex = -fadex ;  
//    if (vy == 50 || vy == 200)
//  {
//    fadey = -fadey ;
//    vy = vy + fadey;
//  }
//    
//  }
  vx = 0;
  vy = 0;
  for(vy = 0; vy<251;vy =vy + 50 ){
    for (vx=0; vx < 251; vx = vx+50){
      analogWrite( 3,  vx);  // duty cycle = 1/160
      analogWrite( 5,  255-vx); // ~ 1/3
      analogWrite( 9, vy);  // ~ 2/3
      analogWrite(10, 255-vy); // 159/160
      Serial.print(vx);
      Serial.print(',');
      Serial.print(vy);
      Serial.print(',');
      for(j =0; j < 9; j++){
        delay(1000);
        return_val = analogRead(RETURN_PIN);
        Serial.print(return_val);
        Serial.print(',');
      }
      Serial.println(return_val);
      
//      if (vx == 250){
//        vx = 0;
//      }
//      if (vy == 250){
//        vy = 0;
//      } 
    }
  }
  
}
