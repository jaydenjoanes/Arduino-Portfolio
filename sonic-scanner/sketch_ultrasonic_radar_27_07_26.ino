#include <Servo.h> // including libraries for Servo and LCD
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // RS, E, D4, D5, D6, D7 - RW is tied directly to GND on the breadboard, not driven by code
const int servoPin = 13; // defining variables
const int echoPin = 9;
const int trigPin = 10;
Servo myServo;

int angle = 0; // current sweep angle
int distance;

String print_angle("Angle:"); // creating strings to concatenate for later
String print_distance("Distance:");
String print_cm("cm");
String print_deg(" deg");
String print_error("INVALID"); // shown on LCD when reading is outside of reliable range
String comma(","); // field separator for Serial values to be used in Processing

void setup() {
  Serial.begin(9600); // initiate Serial Monitor, LCD and Servo
  lcd.begin(16, 2);
  myServo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
}

// Prints "Angle:<value> deg" on row 0, starting at colnumbers, then pads with
// spaces out to a fixed 14-character width. The LCD never clears old characters
// on its own, so without this padding a shorter new value (e.g. "90") would leave
// a stray trailing digit behind from a longer previous value (e.g. "100").
void printpaddedangle(int angle, int colnumbers) { 
  String lcdprint_angle(print_angle + angle + print_deg); 
  lcdprint_angle.length(); 
  int calc_gap; // the number of blank characters needed to reach fixed width
  int loop_iteration = 0; // counter for padding loop
  
  lcd.setCursor(colnumbers, 0);
  lcd.print(lcdprint_angle);
  for (calc_gap = 14 - lcdprint_angle.length(); loop_iteration < calc_gap; loop_iteration += 1) {
    lcd.print(" ");
  }
}

// Prints either "Distance:<value>cm" or "Distance:INVALID" on row 1, then pads
// to a fixed 16-character width. Both the numeric and error cases are decided
// here and share one print+pad sequence afterward, so the padding logic only
// has to be correct in one place rather than duplicated per branch.
void printpaddeddistance(int distance, int colnumbers) { 
  String distanceoutput;
  
  int calc_gap;
  int loop_iteration = 0;
  
  if (distance < 750) { // HC-SR04 reliable range based on own testing
    distanceoutput = (print_distance + distance + print_cm);
  } else {  // no object in range
    distanceoutput = (print_distance + print_error);
  }
  
  lcd.setCursor(colnumbers, 1);
  lcd.print(distanceoutput);
  for (calc_gap = 16 - distanceoutput.length(); loop_iteration < calc_gap; loop_iteration += 1) {
    lcd.print(" ");
  }
}

// Moves the servo to the current global angle, waits for it to settle, then
// triggers the ultrasonic sensor and calculates distance.
// Order matters here: write -> delay -> measure, in that order, guarantees the
// servo has actually arrived and stopped moving before the reading is taken.
// Measuring first (an earlier version of this project) paired each reading
// with the angle the servo was leaving, not the one it was heading to.
void sweepStep() {
  long duration;
  
  myServo.write(angle);
  delay(250); // servo settle time
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(500);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(500);
  digitalWrite(trigPin, LOW);
    
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; //speed of sound (cm/microsecond) x roundtrip time / 2
  
  // Comma-separated, one line per reading - simple for Processing to split()
  // on, unlike the human-readable "Angle: X Distance: Y cm" format used for
  // the LCD, which isn't a good fit for another program to parse.
  Serial.println(angle + comma + distance);
}

void loop() {
  // Sweep angle stays within 10-170 deg rather than the servo's full 0-180
  // range. Testing showed no difference in behaviour at the two boundaries
  // regardless of which numbers were used for the range, and separately
  // confirmed (by counting ticks by hand) that "one fewer visible movement
  // than the number of angle values" is expected behaviour for any sweep of
  // N positions, not a bug. 10-170 is kept simply to give the mount a small
  // safety margin at the extremes.
  for (angle = 10; angle <= 170; angle += 10) {
    sweepStep();
    printpaddedangle(angle, 0);
    printpaddeddistance(distance, 0);
  }

  for (angle = 170; angle >= 10; angle -= 10) {
    sweepStep();
    printpaddedangle(angle, 0);
    printpaddeddistance(distance, 0);   
  }
}
