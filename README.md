# lcis_lightsaber
code for the lightsaber

## Lightsaber Programming Curriculum

This progression is designed to introduce programming concepts step by step while building toward a fully interactive lightsaber using an ESP32-C3, LED strip, gyroscope, and buzzer. Each step introduces one or two new ideas so students can focus on understanding a specific concept before moving on.

---

### 1. Empty Sketch

Upload a sketch with empty `setup()` and `loop()` functions.
The goal is simply to confirm that the ESP32-C3 is correctly connected and that students understand how to upload code.

### 2. Serial Hello World

Print a short message to the Serial Monitor every second.
This introduces serial communication and shows how the microcontroller can report information for debugging.

### 3. Blink an LED

Blink a single LED on and off using `delay()`.
Students learn how to control a pin and see the first physical output from their program.

### 4. Blink with Serial Messages

Blink the LED while printing "ON" and "OFF" in the Serial Monitor.
This helps students connect program logic with observable behavior.

### 5. Read a Button

Connect a button and print whether it is pressed or not.
Students learn how to read input from the real world.

### 6. Button Controls an LED

Pressing the button turns the LED on and releasing it turns it off.
This demonstrates simple conditional logic and interaction.

---

## LED Blade

### 7. Turn On One Pixel of the LED Strip

Initialize the LED strip library and light a single pixel.
Students learn that each LED in the strip has an index and can be controlled individually.

### 8. Turn On the Entire Strip

Use a loop to set every LED in the strip to the same color.
This introduces loops as a way to control many LEDs efficiently.

### 9. Change Blade Color

Let students modify RGB values to change the color of the strip.
This teaches how colors are created digitally and encourages experimentation.

### 10. Blade On and Off

Create functions that turn the strip fully on or fully off.
This introduces the idea of reusable functions and cleaner code.

### 11. Ignition Animation

Light the LEDs one by one from the base to the tip of the strip.
Students learn that animation is created by updating LEDs sequentially over time.

### 12. Shutdown Animation

Reverse the ignition animation so the blade turns off from tip to base.
This teaches students how reversing loops changes behavior.

### 13. Brightness Control

Add a variable that adjusts brightness of the entire strip.
Students see how configuration variables make a program easier to tweak.

---

## Motion and Sensors

### 14. Read Gyroscope Data

Initialize the gyroscope and print the X, Y, and Z rotation values.
Students learn how sensors provide continuous data.

### 15. Detect Movement

Define a threshold that indicates the saber is moving.
When the threshold is exceeded, print a message to the Serial Monitor.

### 16. Swing Detection

When a movement threshold is reached, trigger a simple visual reaction.
This introduces event driven programming.

### 17. Light Flash on Swing

When a swing is detected, briefly increase brightness or flash the blade.
Students see how sensor events can trigger visual effects.

### 18. Detect Strong Impact

Add a second higher threshold to represent a clash.
This teaches students how to interpret sensor data in different ways.

### 19. Clash Flash Effect

When a strong impact is detected, flash the blade white for a short moment.
This simulates a lightsaber clash.

### 20. Idle Blade Effect

Add a subtle flicker while the blade is on.
Students learn how randomness or small variations create more realistic effects.

---

## Program Structure

### 21. Introduce Program States

Define states such as OFF, TURNING_ON, and ON.
This helps organize the program and control complex behavior.

### 22. Button Toggles Saber Power

Pressing the button switches the saber between OFF and ON states.
Students learn how state transitions control system behavior.

### 23. Non Blocking Timing with Millis

Replace some `delay()` calls with `millis()` timing.
This allows the program to react to input while animations are running.

### 24. Organize Code into Functions

Split the program into functions such as `readGyro()` and `updateBlade()`.
This makes the code easier to understand and maintain.

### 25. Configuration Section

Move important settings such as brightness and thresholds to the top of the sketch.
This allows easy tuning without changing the main logic.

---

## Sound

### 26. Buzzer Test

Add the buzzer and play a simple tone.
Students learn how to generate sound using a digital pin.

### 27. Startup Sound

Play a tone sequence when the blade turns on.
This synchronizes sound with the ignition animation.

### 28. Swing Sound

When the gyroscope detects a swing, play a short sound.
This adds audio feedback to motion events.

### 29. Clash Sound

Play a sharper sound when a strong impact is detected.
The saber now reacts with both light and sound.

---

### 30. Final Integrated Lightsaber

Combine button control, blade animations, motion detection, and sound effects.
Students finish with a fully interactive lightsaber they programmed step by step.

## Arduino IDE setup

Before compiling a sketch, make sure the Arduino IDE is configured for the correct ESP32-C3 board under `Tools > Board`.

If the wrong board is selected, the sketch can fail with a generic `exit status 1` compilation error even though the code itself is valid.

Required libraries:

- `Adafruit NeoPixel`
- `Adafruit MPU6050`
- `Adafruit Unified Sensor`

## Shared hardware assumptions

These sketches use the same pin choices throughout:

- LED strip data pin: `GPIO 2`
- Button pin: `GPIO 0`
- Built-in LED: `GPIO 8`
- Buzzer pin: `GPIO 10`

## Sketch folders

Each lesson has its own numbered Arduino sketch folder, from `step_01_empty_sketch` through `step_30_final_integrated_lightsaber`.
