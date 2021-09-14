#define time_delay 100000 // Takes time_delay/200 to get the actual value out
#define a_pin 0 // The analog reading pin
#define buck 3  // Digital PWM pin to control the buck DC-DC converter

volatile uint16_t Measurement = 0;

void setup()
{
  pinMode(buck, OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  Serial.begin(500000);

  noInterrupts();           // disable all interrupts

  // Setting timer0 as PWM source (note that when set as PWM mode, timer does not have interrupt vector)
  TCCR5B = 0; // Initialize timer counter control register to 0
  TCCR5A = 0; // Initialize timer counter control register to 0

  // 1. Set the compare output mode to clear OC2A and OC2B on compare match.
  //    To achieve this, we set bits COM2A1 and COM2B1 to high.
  // 2. Set the waveform generation mode to fast PWM (mode 3 in datasheet).
  //    To achieve this, we set bits WGM21 and WGM20 to high.
  TCCR5A = _BV(COM5A1) | _BV(COM5B1) | _BV(WGM51) | _BV(WGM50);

  // 1. Set the waveform generation mode to fast PWM mode 7 —reset counter on
  //    OCR2A value instead of the default 255. To achieve this, we set bit
  //    WGM22 to high.
  // 2. Set the prescaler divisor to 1, so that our counter will be fed with
  //    the clock's full frequency (16MHz). To achieve this, we set CS20 to
  //    high (and keep CS21 and CS22 to low by not setting them).
  TCCR5B = _BV(WGM52) | _BV(CS50);
  
  // OCR2A holds the top value of our counter, so it acts as a divisor to the
  // clock. When our counter reaches this, it resets. Counting starts from 0.
  // Thus 255 equals to 256 divs.
  OCR5A = 255;
  // This is the duty cycle. Think of it as the last value of the counter our
  // output will remain high for. Can't be greater than OCR2A of course. A
  // value of 0 means a duty cycle of 1/255 in this case.
  OCR5B = 0;

  //Timer 0 interrupt service routine CTC settings, 1000 uS:
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  //set CTC mode
  TCCR0A |= (1 << WGM01);
  //prescaler 64 for timer0
  TCCR0B |= (1 << CS01) | (1 << CS00);

  // value for 1 msec
  OCR0A = 250;

  //set compare match for register OCRA
  TIMSK0 |= (1 << OCIE0A);
  interrupts();             // enable all interrupts
}

ISR(TIMER0_COMPA_vect){
 Measurement = analogRead(a_pin); // Measurement ranges from 0 - 1023
 OCR2B = Measurement/4; // Since maximum value of OCR2A is 255, the OCR2B mapped by Measurement must have the value from 0 -1023/4 (255)
}

//This loop controls the optocouplers and reverses the actuator's polarity periodically
void loop()
{

digitalWrite(4,HIGH);
digitalWrite(5,LOW);
delay(time_delay);

digitalWrite(4,LOW);
digitalWrite(5,LOW);
delay(time_delay);

digitalWrite(4,LOW);
digitalWrite(5,HIGH);
delay(time_delay);

digitalWrite(4,LOW);
digitalWrite(5,LOW);
delay(time_delay);

}
