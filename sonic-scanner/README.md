Ultrasonic Radar
Servo-driven ultrasonic radar made with an Arduino Uno R3, an HC-SR04 ultrasonic sensor and an SG90 servo motor. 
The sensor sweeps across a 180° arc in 10° increments, taking a distance reading at each angle and creating a radar UI.

Overview
The project combines an actuator (servo) and sensor (ultrasonic sensor) into a combined system and producing output through an LCD as well as creating a live output radar graphic using Processing.
The servo rotates to a target angle before the sensor's reading can be reliable adding a sequencing issue to solve rather than just reading sensor output.

Parts Used
Arduino Uno R3
HC-SR04 Ultrasonic Sensor
SG90 Servo Motor
LCD1602 Module
Breadboard + Jumper Wires
Potentiometer
Rubber Bands (DIY solution for mounting)
Mounting coupler to attach Sensor to Servo horn (Ideal mounting option, 3D printed part)

Design Process
1. Servo moved to target angle
2. Wait for servo to settle
3. Trigger ultrasonic sensor and measure return pulse
4. Convert pulse into a distance
5. Report angle and distance -> Print to LCD and Processing
6. Move to new angle and repeat sweeping from 0° - 180°.

I built and tested the servo sweep and sensor readings separately then combined them once they worked independently to help troubleshoot any bugs. I then combined code from a previous project for the LCD. 

Technical Challenges
After refactoring the code to create the sweepStep() function, the servo stopped functioning despite the angle printing correctly to serial. The for loop declared its own local angle variable which shadowed the global angle variable that sweepStep() actually read from. The loop updated a variable within the loop that the rest of the code couldn't see while the global one never changed. To fix this I removed the redeclaring angle locally and let the loop update the global directly.
Readings were paired with the wrong angle. The initial sequence was measure distance, print, move servo, delay. This meant each measurement was happening before the servo moved so the distance was printed for the previous angle. To fix this, I reordered the sequence to move servo, delay, measure distance, print. This allows the servo to settle and make sure the correct distance with the correct angle.
An int was too narrow for the raw pulse timing. duration (the raw echo pulse length from pulseIn()) was briefly declared as int rather than long. This works fine when an object is in range and the pulse is short, but pulseIn() can return a much larger value on timeout (e.g. no echo at all, sensor pointed at open space) — large enough to overflow an int. Matching the variable's type to what pulseIn() actually returns fixed this before it caused a hard-to-diagnose bad reading in the field.
String concatenation type errors. Arduino's raw string literals (const char*) don't support + concatenation with numbers the way higher-level languages do. Switching to Arduino's String class (not to be confused with C++'s std::string, which caused its own separate set of errors) resolved this and made the Serial output easy to format cleanly on one line per reading.
