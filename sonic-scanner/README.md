Ultrasonic Radar
Servo-driven ultrasonic radar made with an Arduino Uno R3, an HC-SR04 ultrasonic sensor and an SG90 servo motor. 
The sensor sweeps across a 180° arc in 10° increments, taking a distance reading at each angle and creating a radar UI.

Overview
The project combines an actuator (servo) and sensor (ultrasonic sensor) into a combined system.
The servo rotates to a target angle before the sensor's reading can be reliable adding a sequencing issue to solve rather than just reading sensor output.

Parts Used
Arduino Uno R3
HC-SR04 Ultrasonic Sensor
SG90 Servo Motor
Breadboard + Jumper Wires
Mounting coupler to attach Sensor to Servo horn

Design Process
1. Servo moved to target angle
2. Wait for servo to settle
3. Trigger ultrasonic sensor and measure return pulse
4. Convert pulse into a distance
5. Report angle and distance
6. Move to new angle and repeat sweeping from 0° - 180°.

I built and tested the servo sweep and sensor readings separately then combined them once they worked independently to help troubleshoot any bugs
