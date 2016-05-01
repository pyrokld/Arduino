
#include <Adafruit_SoftServo.h>
//#include <SoftwareSerial.h>

#define SERVO1PIN 0   // Servo control line (orange) on Trinket Pin #0
#define SERVO2PIN 1   // Servo control line (orange) on Trinket Pin #1
#define EZ1pin 2      // Trinket GPIO #1 (Pin #2)

Adafruit_SoftServo servo_left, servo_rght;
//SoftwareSerial Serial(3,4);
  
// Moderate speed forward for both servos. Given the orientation of the servos
// one will be going forward, and the other backward. You may need to adjust
// these slightly to get the rover to move straight forward.
const int left_speed = 75;
const int rght_speed = 90;

// Number in cm when the rover will reverse and try to navigate around.
const int obstacle = 20;

// Multiplier used to determine how far the rover will back-up.
const int back_track = 100;

// These values are for calculating a mathematical median for a number of samples as
// suggested by Maxbotix instead of a mathematical average
int8_t arraysize = 3; // quantity of values to find the median (sample size). Needs to be an odd number
//declare an array to store the samples. not necessary to zero the array values here, it just makes the code clearer
uint16_t rangevalue[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t modE;        // calculated median distance

void setup() {
  pinMode(EZ1pin, INPUT); // Sey ultrasonic sensor pin as input
  //Serial.begin(115200);
  servo_left.attach(SERVO1PIN);
  servo_rght.attach(SERVO2PIN);
}

void loop() {
  int16_t pulse;  // number of pulses from sensor
  int i=0;
  
  while( i < arraysize )
  {   
      servo_left.refresh();
      servo_rght.refresh();                 
    pulse = pulseIn(EZ1pin, HIGH);  // read in time for pin to transition
    rangevalue[i]=pulse/58;         // pulses to centimeters (use 147 for inches)
    if( rangevalue[i] < 645 && rangevalue[i] >= 15 ) i++;  // ensure no values out of range
    servo_left.refresh();
    servo_rght.refresh();
    delay(10);                      // wait between samples
  }
  isort(rangevalue,arraysize);        // sort samples
  modE = mode(rangevalue,arraysize);  // get median

  if ( modE > 50 ) {
    modE = 50;
  }
  // Setting servos in forward motion.
  servo_left.write(left_speed - modE);
  servo_left.refresh();
  servo_rght.write(rght_speed + modE);
  servo_rght.refresh();
//  Serial.print("Forward");
//  Serial.print("\t");
//  Serial.print("Left:");
//  Serial.print("\t");
//  Serial.print(left_speed - modE);
//  Serial.print("\t");
//  Serial.print("Right:");
//  Serial.print("\t");
//  Serial.print(rght_speed + modE);
//  Serial.print("\t");
//  Serial.print("Dist:");
//  Serial.print("\t");
//  Serial.print(modE);
//  Serial.println("");
  
  delay(15);                        // Read every half second
 if ( modE > 50 ) {
    modE = 50;
  }
    if ( modE < obstacle ) {
    // back_track * delay(15) = distance the rover will back-up during
    // obstacle avoidance.
    for (int i = 0; i < back_track; i++) {
      // Magic numbers... will always backup the same direction. Can you
      // think of a better way to navigate obstacles?
      servo_left.write(150);
      servo_left.refresh();
      servo_rght.write(50);
      servo_rght.refresh();
//      Serial.print("Backwards");
//      Serial.print("\t");
//      Serial.print("Dist:");
//      Serial.print("\t");
//      Serial.print(modE);
//      Serial.println("");
      delay(15);
    }
  }
}

// Sorting function (Author: Bill Gentles, Nov. 12, 2010)
void isort(uint16_t *a, int8_t n){
  for (int i = 1; i < n; ++i)  {
    uint16_t j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--) {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}

// Mode function, returning the mode or median.
uint16_t mode(uint16_t *x,int n){
  int i = 0;
  int count = 0;
  int maxCount = 0;
  uint16_t mode = 0;
  int bimodal;
  int prevCount = 0;
  while(i<(n-1)){
    prevCount=count;
    count=0;
    while( x[i]==x[i+1] ) {
      count++;
      i++;
    }
    if( count > prevCount & count > maxCount) {
      mode=x[i];
      maxCount=count;
      bimodal=0;
    }
    if( count == 0 ) {
      i++;
    }
    if( count == maxCount ) {      //If the dataset has 2 or more modes.
      bimodal=1;
    }
    if( mode==0 || bimodal==1 ) {  // Return the median if there is no mode.
      mode=x[(n/2)];
    }
    return mode;
  }
}



