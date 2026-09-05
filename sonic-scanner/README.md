# Ultrasonic Radar

Servo-driven ultrasonic radar made with an Arduino Uno R3, an HC-SR04 ultrasonic sensor and an SG90 servo motor. The sensor sweeps across a 180° arc in 10° increments, taking a distance reading at each angle and creating a radar UI.

## Overview

The project combines an actuator (servo) and sensor (ultrasonic sensor) into a combined system, producing output through an LCD as well as a live output radar graphic using Processing. The servo has to rotate to a target angle before the sensor's reading can be reliable, adding a sequencing issue to solve rather than just reading sensor output.

## Parts Used

- Arduino Uno R3
- HC-SR04 Ultrasonic Sensor
- SG90 Servo Motor
- LCD1602 Module
- Breadboard + Jumper Wires
- Potentiometer
- Rubber Bands (DIY solution for mounting)
- Mounting coupler to attach sensor to servo horn (ideal mounting option, 3D printed part)
- Blu-Tack for Stabilising the servo

## Design Process

1. Servo moved to target angle
2. Wait for servo to settle
3. Trigger ultrasonic sensor and measure return pulse
4. Convert pulse into a distance
5. Report angle and distance → print to LCD and Processing
6. Move to new angle and repeat, sweeping from 0°–180°

I built and tested the servo sweep and sensor readings separately, then combined them once they worked independently to help troubleshoot any bugs. I then combined code from a previous project for the LCD.

## Technical Challenges

**Variable shadowing broke the servo.**
After refactoring the code to create the `sweepStep()` function, the servo stopped functioning despite the angle printing correctly to Serial. The `for` loop declared its own local `angle` variable which shadowed the global `angle` variable that `sweepStep()` actually read from. The loop updated a variable within the loop that the rest of the code couldn't see, while the global one never changed. To fix this I removed the local redeclaration of `angle` and let the loop update the global directly.

**Readings were paired with the wrong angle.**
The initial sequence was measure distance, print, move servo, delay. This meant each measurement was happening before the servo moved, so the distance was printed for the previous angle. To fix this, I reordered the sequence to move servo, delay, measure distance, print. This allows the servo to settle and makes sure the correct distance is matched with the correct angle.

**An `int` was too narrow for the raw pulse timing.**
`duration` (the raw echo pulse length from `pulseIn()`) was briefly declared as `int` rather than `long`. This works fine when an object is in range and the pulse is short, but `pulseIn()` can return a much larger value on timeout (e.g. no echo at all, sensor pointed at open space) — large enough to overflow an `int`. Matching the variable's type to what `pulseIn()` actually returns fixed this before it caused a hard-to-diagnose bad reading in the field.

**String concatenation type errors.**
Arduino's raw string literals (`const char*`) don't support `+` concatenation with numbers the way higher-level languages do. Switching to Arduino's `String` class (not to be confused with C++'s `std::string`, which caused its own separate set of errors) resolved this and made the output easy to format cleanly.

**Serial and the LCD disagreed by exactly one step.**
Once both outputs were running side by side, the LCD's displayed angle consistently lagged one step behind the Serial monitor's. My first hypothesis was that the LCD's physical write speed couldn't keep up during a fast sweep — but slowing the whole sweep down to 3 seconds per step didn't close the gap at all, ruling that out. Tracing the code line by line: the Serial print of `angle` sat at the very top of `sweepStep()`, firing the instant a new step began, before the servo had even moved — while the LCD update only ran after `sweepStep()` fully completed (move, settle, and measure all done). At any moment mid-step, Serial was announcing the upcoming target while the LCD was still confirming the last completed position — two different moments in time, not a speed problem. The fix was moving the Serial print to the same point in the sequence as the LCD update, so both reflect "this step is now complete."

**A working LCD that displayed nothing.**
After confirming backlight and contrast were both fine, and that the problem reproduced even with the LCD wired in isolation, the cause turned out to be the RW pin — tied to Arduino GND, it had come loose during repeated breadboard changes. With RW floating or high, the LCD sits in read mode and ignores write commands entirely, while still lighting up and responding to contrast adjustment, which is what made it initially look like a power or contrast problem rather than a single control pin.

**Fixed-width display formatting.**
A 16x2 LCD doesn't clear old characters automatically — printing `"90"` over a cell that previously held `"100"` leaves a stray trailing digit behind (`"900"`). The fix was calculating each line's maximum possible width (label + widest possible value + unit) up front, then padding every printed line out to that exact width with trailing spaces, calculated dynamically from the built string's actual length rather than hardcoded.

## Physical Build

The sensor is currently mounted to the servo horn using rubber bands as a DIY solution and stabilised at the base with Blu-Tack, while a proper 3D-printed coupler remains the ideal long-term option.

## Results



## Code



## Reflection

The most useful habit in this project wasn't any single fix — it was tracing suspicious behavior through concrete example values by hand before changing code, rather than guessing at a fix and seeing if it happened to work. Several bugs here (the ordering issue, the sync lag) looked confusing at first glance but became obvious once I wrote out exactly what value each variable held at each line and compared that against what I expected. I'd also, in hindsight, pass values like `angle` into functions as explicit parameters from the start rather than relying on globals — nearly every shadowing bug in this project stemmed from mixing the two approaches inconsistently.

**Next steps:** confirm whether the SG90's behavior at 0°/180° is a genuine mechanical limit now that the sensor is mounted, calibrate angle-to-real-world direction, verify accuracy across the full sweep (not just a fixed position), finish the Processing radar UI, and source the 3D-printed coupler for a more permanent mount.
