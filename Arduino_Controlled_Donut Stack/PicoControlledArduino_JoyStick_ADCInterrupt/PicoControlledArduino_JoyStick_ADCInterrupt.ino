// Defining the digital pins controlling the actuator D6, D5, D3
#define power1 6
#define power2 5
#define power3 3

// Defining the pins controlling the H_Bridge D10, D11
#define OPTORIGHT A1
#define OPTOLEFT A2

// Three states of the Optocoupler H_Bridge
#define DRAIN 0
#define LEFT 1
#define RIGHT 2

volatile uint16_t Measurement1;
volatile uint16_t Measurement2;

// Variables used to determine a change of if to else or vise versa.
int state =1;
int memory = 0;
int VALUE = 1;

// Variables from analog input pins
int valx;
int valy;
int InitialValx;
int InitialValy;

// Variables that determine the magnitude of the high voltage output
int val1;
int val2;
int val3;

// Projected x and y components of the position of the joystick in cartersian coordinate.
double proj1x;
double proj1y;
double proj2x;
double proj2y;
double proj3x;
double proj3y;

// Magnitude of the projected vector of
int magProj1;
int magProj2;
int magProj3;

double dotProj1;
double dotProj2;
double dotProj3;

void setup() {
  
// Setting the three digital pins to the buck converter as outputs
pinMode(power1, OUTPUT);
pinMode(power2, OUTPUT);
pinMode(power3, OUTPUT);

// Setting the controller pins of the H_Bridge as the outputs
pinMode(OPTORIGHT,OUTPUT);
pinMode(OPTOLEFT,OUTPUT);
Serial.begin(9600);
  cli();
  ADMUX  =  bit (REFS0) | (4 & 0x07);   // Set default reference voltage and set measurement to A4
  ADCSRA |= 1<<ADPS2;     // Set 16 prescaler
  ADCSRA |= 1<<ADIE;      // enable interrupt when measurement is complete
  ADCSRA |= 1<<ADEN;      // Enable ADC
  sei();
  ADCSRA |= 1<<ADSC;      // Start ADC Conversion

delay(100);
InitialValx = Measurement1; // Calibrating valx to be 0, the valx range is -511 to 511
InitialValy = Measurement2; // Calibrating valy to be o, the valy range is -511 to 511

}

ISR(ADC_vect) {
   switch(ADMUX){
    case 0x44:
      Measurement2 =  ADCL | ADCH << 8;
      ADMUX = 0x45;
      break;
    case 0x45:
      Measurement1 =  ADCL | ADCH << 8;
        ADMUX = 0x44;
     break;
  }
  ADCSRA |= 1 << ADSC;  
}

void loop()
{
// Reading the analog values from the joystick: valx and valy correspond to the x and y axis of the joystick
// When the joystick not moved, valx and val2y are ~ 500 in the range of  0 to 1023 values

cli();
valx = Measurement1-InitialValx; // Calibrating valx to be 0, the valx range is -511 to 511
valy = Measurement2-InitialValy; // Calibrating valy to be o, the valy range is -511 to 511
sei();

Serial.print("Valx");
Serial.print('\t');
Serial.print("valy");
Serial.print('\t');

Serial.print("val1");
Serial.print('\t');
Serial.print("val2");
Serial.print('\t');
Serial.print("val3");
Serial.print('\n');

Serial.print(valx);
Serial.print('\t');
Serial.print(valy);
Serial.print('\t');

if ( valx < 70 && valx >-70 && valy <70 && valy >-70){    // if the joystick is not moved do the following:
  digitalWrite(power1,HIGH);
  digitalWrite(power2,HIGH);
  digitalWrite(power3,HIGH);
  val1 = 256;
val2 = 256;
val3 = 256;
  if (state != memory) {                                  // Check if the joystick has been previously moved, if yes
    H_Bridge(DRAIN);                                      // Drain the Actuator
    state = 0;                                            // Give the system the new state for the next joystick movement check
    H_Bridge(VALUE);                                      // After the DRAIN, set the H_Bridge to one of the following VALUES
    switch (VALUE) {                                      // If H_Bridge actuated LEFT, then now actuate RIGHT, and vise versa
      case LEFT:
        VALUE = RIGHT;
      break;
      case RIGHT:
        VALUE = LEFT;
      break;
      default:
      break;
    }
  }
}
else {                                                                // If the joystick is moved do the following:
proj1x = (valx*1+valy*0)/1*1;                                         // Perform projection to map the joystick position to the state of the actuators
proj1y = (valx*1+valy*0)/1*0;
proj2x = (valx*(-0.5)+valy*0.866025)/1*(-0.5);
proj2y = (valx*(-0.5)+valy*0.866025)/1*(0.866025); 
proj3x = (valx*(-0.5)+valy*(-0.866025))/1*(-0.5); 
proj3y = (valx*(-0.5)+valy*(-0.866025))/1*(-0.866025); 

dotProj1 = proj1x*1+proj1y*0;
magProj1 = sqrt(pow(proj1x,2)+pow(proj1y,2));
if (dotProj1 > 0){
  val1 = map((int)magProj1, 0, 430, 256, 0);
  if (val1 < 10) { val1 = 1;}
   analogWrite(power1,val1);
}
else { digitalWrite(power1,HIGH);}

dotProj2 = proj2x*(-0.5)+proj2y*(0.866025);
magProj2 = sqrt(pow(proj2x,2)+pow(proj2y,2));
if (dotProj2 > 0){
  val2 = map((int)magProj2, 0, 430, 256, 0);
  if (val2 < 10) { val2 = 1;}
   analogWrite(power2,val2);
}
else {   digitalWrite(power2,HIGH);}

dotProj3 = proj3x*(-0.5)+proj3y*(-0.866025);
magProj3 = sqrt(pow(proj3x,2)+pow(proj3y,2));
if (dotProj3 > 0){
  val3 = map((int)magProj3, 0, 430, 256, 0);
  if (val3 < 10) { val3 = 1;}
  analogWrite(power3,val3);
}
else {digitalWrite(power3,HIGH);}

state = 1;                                                            // Give the system the new state for the next joystick movement check
}

Serial.print(val1);
Serial.print('\t');
Serial.print(val2);
Serial.print('\t');
Serial.print(val3);
Serial.print('\n');
Serial.print('\n');

}

void H_Bridge(int VALUE) {                                            // Function H_Bridge to activate the optocoupler depending on the VALUE
  switch (VALUE) {
    case DRAIN:
      digitalWrite(OPTORIGHT, LOW);
      digitalWrite(OPTOLEFT, LOW);
      
    break;
    case LEFT:
      digitalWrite(OPTORIGHT, LOW);
      digitalWrite(OPTOLEFT, HIGH);
    break;
    case RIGHT:
      digitalWrite(OPTORIGHT, HIGH);
      digitalWrite(OPTOLEFT, LOW);
    
    default:
    break;
  }
}

