/* to run two timer interrupts at the same time, care must be taken when choosing prescaler and OCRnA to set the timer intervals. If the timer is going faster than
the number of cycle counts needed for each interrupt to happen,  error occurs. The interrupt will work fine individially, but will not work when put together if the frequency is too fast
 */

volatile uint16_t Measurement1 =0;
volatile uint16_t Measurement2 =0;

void setup()
{
  pinMode(buck, OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  Serial.begin(500000);
  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  
  TCCR1B = 0;
  TCCR1A = 0;


  //set CTC mode
  TCCR1B |= (1 << WGM12);
  // enable compare match interrupt
  TIMSK1 |= (1 << OCIE1A);


  // set OCR0A value for 1 msec
  OCR1A = 62;
  //set 256 prescaler
  TCCR1B |= (1 << CS12);

  //Timer 2 interrupt service routine CTC settings, 20 uS:
  TCCR2A = 0;
  TCCR2B = 0;
  //set CTC mode
  TCCR2A |= (1 << WGM21);
  //prescaler 1 for timer2
  TCCR2B |= (1 << CS20) | (1 << CS21);

  // value for 20 usec
  OCR2A = 10;

  //set compare match for register OCRA
  TIMSK2 |= (1 << OCIE2A);
  interrupts();             // enable all interrupts
}

ISR(TIMER2_COMPA_vect){
Measurement2 +=1;
}

ISR(TIMER1_COMPA_vect){
Measurement1 +=1;
}

void loop()
{


Serial.print(Measurement2);
Serial.print("  ");
Serial.println(Measurement1);
}
