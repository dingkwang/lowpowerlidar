// Low power LIDAR program
#define RETURN_PIN   A5  // Arduino pin tied to Return pin on the OSLRF.
int LRF = 13;                // the output pin that the Delay Unit is atteched to
int PIR = 2;              // the input pin that the PIR sensor is atteched to
int t_state;
int return_val = 0;
float pd[4] = {0};
float dist[4] = {0};
float trans[2][4] = {
    {0.433, 0.458, 0.449, 0.428},
    {-25.05, -29.4, -28.85, -25.55}
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

int xyf[2][4] = { // Large Area
  {0, 200, 200, 0},
  {0, 0, 200, 200}
};
        
int pir_val = 0; // by default, no motion detected
int t = 0;

void setup()
{
  pinMode(LRF, OUTPUT);      // initalize onboard LED as an output
  pinMode(PIR, INPUT);    // initialize PIR as an input
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
  t_state = digitalRead(PIR);  // read PIR

  if (t_state == HIGH) {
    t = 2;  // Delay time for Motion leave 
    if (pir_val == LOW){
      pir_val = HIGH;
      digitalWrite(LRF, LOW); // LOW is ON
      Serial.println("Motion Detected, wait to turn on LRF");
      delay(200000);
      Serial.println("LRF ON");
    }
  }
   else {
   digitalWrite(LRF, HIGH); //HIGH is OFF
   pir_val = LOW;
   Serial.println("OFF");
  }
  
  while (t >0){
    t = t-1;   //Keep scan t freams until stop
    Serial.print("t = ");
    Serial.println(t);
    m = 0;
    temp = 0;
    float pd[4] = {0};

    for (k = 0; k <= 3; k++ ) { //Large Area 
      temp = 0;
      for (i = 0; i <= 3; i++) { //Small Area
        vx = xy[0][i] + xyf[0][k];
        vy = xy[1][i] + xyf[1][k];
        analogWrite( 3,  vx);  // duty cycle = 1/160
        analogWrite( 5,  255 - vx); // ~ 1/3
        analogWrite( 9, vy);  // ~ 2/3
        analogWrite(10, 255 - vy); // 159/160
        //      Serial.print(vx);
        //      Serial.print(',');
        //      Serial.print(vy);
        //      Serial.print(',');
        return_val = 0;
        delay(2000); // Wait for the MEMS mirror to stablize
        for (j = 0; j < 10; j++) { // Take Average of 10 measurement at each scanning location 
          //delay(1000);
          return_val = return_val + analogRead(RETURN_PIN);
          
        }
        temp = temp + return_val / 10;
      }
      pd[m] = pd[m] + temp / 4;  //Average the 4 distance at each large area
      
      m = m + 1;
    }
    
    for (i = 0; i<=3;i++) {
        if (pd[i]>=50 && pd[i]<=130){ //Outside the range is invalid
            dist[i] = pd[i]*trans[0][i]+trans[1][i];
        } 
        else{
            dist[i] = 0;
        }
        Serial.print(dist[i]);   //Print the average distnaces in the 4 large Areas
        Serial.print(',');
    }
    
    // m = 0;
    // ms = dist[m];
    
    // for (m = 0; m < 4; m++) // Find the closest area in area 0, 1, 2, 3
    // {
      // if (dist[m] < ms)
      // {
        // ms = pd[m];
        // mi = m;
      // }
    // }
    // Serial.println(mi);
  }


}
