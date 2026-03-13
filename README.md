# lcis_lightsaber

Code for a classroom lightsaber project using a Seeed Studio XIAO ESP32-C3.

## Lightsaber Programming Curriculum

This progression is designed to introduce programming concepts step by step while building toward a fully interactive lightsaber using an ESP32-C3, LED strip, gyroscope, and buzzer. Each step introduces one or two new ideas so students can focus on understanding a specific concept before moving on.

- [lcis\_lightsaber](#lcis_lightsaber)
	- [Lightsaber Programming Curriculum](#lightsaber-programming-curriculum)
	- [Before You Start](#before-you-start)
	- [Step 0: Get Ready](#step-0-get-ready)
		- [1. Plug in the board](#1-plug-in-the-board)
		- [2. Open Arduino IDE](#2-open-arduino-ide)
		- [3. Install the ESP32 board package](#3-install-the-esp32-board-package)
		- [4. Choose the correct board](#4-choose-the-correct-board)
		- [5. Choose the correct USB port](#5-choose-the-correct-usb-port)
		- [6. Install the libraries](#6-install-the-libraries)
		- [7. Test uploading](#7-test-uploading)
	- [Wiring Notes](#wiring-notes)
		- [Important note for Step 3 and Step 4](#important-note-for-step-3-and-step-4)
	- [Shared Pin Plan](#shared-pin-plan)
	- [The Steps](#the-steps)
		- [Basics](#basics)
			- [1. Empty Sketch](#1-empty-sketch)
			- [2. Serial Hello World](#2-serial-hello-world)
			- [3. Blink an LED](#3-blink-an-led)
			- [4. Blink with Serial Messages](#4-blink-with-serial-messages)
			- [5. Read a Button](#5-read-a-button)
			- [6. Button Controls an LED](#6-button-controls-an-led)
		- [LED Blade](#led-blade)
			- [7. Turn On One Pixel of the LED Strip](#7-turn-on-one-pixel-of-the-led-strip)
			- [8. Turn On the Entire Strip](#8-turn-on-the-entire-strip)
			- [9. Change Blade Color](#9-change-blade-color)
			- [10. Blade On and Off](#10-blade-on-and-off)
			- [11. Ignition Animation](#11-ignition-animation)
			- [12. Shutdown Animation](#12-shutdown-animation)
			- [13. Brightness Control](#13-brightness-control)
		- [Motion and Sensors](#motion-and-sensors)
			- [14. Read Gyroscope Data](#14-read-gyroscope-data)
			- [15. Detect Movement](#15-detect-movement)
			- [16. Swing Detection](#16-swing-detection)
			- [17. Light Flash on Swing](#17-light-flash-on-swing)
			- [18. Detect Strong Impact](#18-detect-strong-impact)
			- [19. Clash Flash Effect](#19-clash-flash-effect)
			- [20. Idle Blade Effect](#20-idle-blade-effect)
		- [Program Structure](#program-structure)
			- [21. Introduce Program States](#21-introduce-program-states)
			- [22. Button Toggles Saber Power](#22-button-toggles-saber-power)
			- [23. Non-Blocking Timing with Millis](#23-non-blocking-timing-with-millis)
			- [24. Organize Code into Functions](#24-organize-code-into-functions)
			- [25. Configuration Section](#25-configuration-section)
		- [Sound](#sound)
			- [26. Buzzer Test](#26-buzzer-test)
			- [27. Startup Sound](#27-startup-sound)
			- [28. Swing Sound](#28-swing-sound)
			- [29. Clash Sound](#29-clash-sound)
			- [30. Final Integrated Lightsaber](#30-final-integrated-lightsaber)


## Before You Start

You need:

- 1 Seeed Studio XIAO ESP32-C3
- 1 USB-C cable that can send data
- A computer with Arduino IDE installed
- 1 LED strip for the blade
- 1 button
- 1 buzzer
- 1 gyroscope sensor
- Breadboard and jumper wires
- 1 single LED for the early lessons
- 1 resistor between `220 ohm` and `330 ohm` for that single LED

## Step 0: Get Ready

Follow these setup steps before opening any sketch.

### 1. Plug in the board

Plug the XIAO ESP32-C3 into your computer with the USB-C cable.

If nothing shows up in Arduino IDE later, the cable may be a charging-only cable. Try another one.

### 2. Open Arduino IDE

Start Arduino IDE on your computer.

### 3. Install the ESP32 board package

In Arduino IDE:

1. Open `File > Preferences`
2. Find the box called `Additional Boards Manager URLs`
3. Paste this link:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

4. Click `OK`
5. Open `Tools > Board > Boards Manager...`
6. Search for `esp32`
7. Install `esp32 by Espressif Systems`

### 4. Choose the correct board

In Arduino IDE, go to `Tools > Board` and choose the XIAO ESP32-C3 board.

If the wrong board is selected, your sketch can fail with a message like:

```text
exit status 1
```

That message by itself is not the real problem. It often just means the board setup is wrong or a library is missing.

### 5. Choose the correct USB port

In Arduino IDE, go to `Tools > Port` and click the port for your XIAO ESP32-C3.

If you are not sure which port is correct:

1. Unplug the board
2. Look at the list of ports
3. Plug the board back in
4. The new port is the one you want

### 6. Install the libraries

Open `Sketch > Include Library > Manage Libraries...`

Install these libraries:

- `Adafruit NeoPixel`
- `Adafruit MPU6050`
- `Adafruit Unified Sensor`

### 7. Test uploading

Open the folder `step_01_empty_sketch` and upload that sketch first.

If it uploads, your board and Arduino IDE are working.

## Wiring Notes

### Important note for Step 3 and Step 4

The Seeed Studio XIAO ESP32-C3 does not use a normal built-in user LED for these beginner blink lessons.

For `step_03_blink_led` and `step_04_blink_with_serial`, use an external LED:

- Connect `D10` to a `220-330 ohm` resistor
- Connect the resistor to the long leg of the LED
- Connect the short leg of the LED to `GND`

`220 ohm` is a good default choice.

## Shared Pin Plan

These sketches mostly use the same pins so students do not have to relearn the wiring each time.

- Single practice LED for step 3 and 4: `D10`
- LED strip data pin: `GPIO 2`
- Button pin: `GPIO 0`
- Buzzer pin: `GPIO 10`

If your class wiring is different, update the pin numbers at the top of each sketch.

## The Steps

Each lesson has its own numbered sketch folder.

Start at Step 1 and move forward in order:

- `step_01_empty_sketch`
- `step_02_serial_hello_world`
- `step_03_blink_led`
- `step_04_blink_with_serial`
- `step_05_read_button`
- `step_06_button_controls_led`
- `step_07_one_pixel`
- `step_08_full_strip`
- `step_09_change_blade_color`
- `step_10_blade_on_off`
- `step_11_ignition_animation`
- `step_12_shutdown_animation`
- `step_13_brightness_control`
- `step_14_read_gyroscope`
- `step_15_detect_movement`
- `step_16_swing_detection`
- `step_17_light_flash_on_swing`
- `step_18_detect_strong_impact`
- `step_19_clash_flash_effect`
- `step_20_idle_blade_effect`
- `step_21_program_states`
- `step_22_button_toggle_power`
- `step_23_non_blocking_millis`
- `step_24_organize_into_functions`
- `step_25_configuration_section`
- `step_26_buzzer_test`
- `step_27_startup_sound`
- `step_28_swing_sound`
- `step_29_clash_sound`
- `step_30_final_integrated_lightsaber`

### Basics

#### 1. Empty Sketch

Upload a sketch with empty `setup()` and `loop()` functions.
The goal is to make sure the board is connected and code can be uploaded.

#### 2. Serial Hello World

Print a short message to the Serial Monitor every second.
This shows how the board can send messages back to the computer.

#### 3. Blink an LED

Blink one external LED on and off using `delay()`.
This is the first visible output.

#### 4. Blink with Serial Messages

Blink the LED while printing `ON` and `OFF` in the Serial Monitor.
This connects the code to what students can see.

#### 5. Read a Button

Connect a button and print whether it is pressed or not.
Students learn how the board reads input.

#### 6. Button Controls an LED

Pressing the button turns the LED on and releasing it turns it off.
This introduces simple `if` statements.

### LED Blade

#### 7. Turn On One Pixel of the LED Strip

Initialize the LED strip library and light one pixel.
Students learn that each LED has its own number.

#### 8. Turn On the Entire Strip

Use a loop to set every LED to the same color.
This introduces loops.

#### 9. Change Blade Color

Change the RGB values to make different colors.
Students learn how digital color works.

#### 10. Blade On and Off

Create functions that turn the strip on and off.
This introduces reusable code.

#### 11. Ignition Animation

Light the blade one LED at a time from base to tip.
This introduces animation.

#### 12. Shutdown Animation

Reverse the animation from tip to base.
This shows how changing a loop changes behavior.

#### 13. Brightness Control

Use a variable to change the whole blade brightness.
This makes the sketch easier to tune.

### Motion and Sensors

#### 14. Read Gyroscope Data

Print the X, Y, and Z gyroscope values.
Students learn that sensors produce live data.

#### 15. Detect Movement

Use a threshold to decide if the saber is moving.

#### 16. Swing Detection

When movement is strong enough, trigger a simple reaction.
This introduces event-based behavior.

#### 17. Light Flash on Swing

When a swing is detected, flash the blade brighter for a moment.

#### 18. Detect Strong Impact

Use a bigger threshold to detect a clash.

#### 19. Clash Flash Effect

When a strong impact is detected, flash the blade white.

#### 20. Idle Blade Effect

Add a small flicker while the blade is on.
This makes it feel more like a real lightsaber.

### Program Structure

#### 21. Introduce Program States

Create states like `OFF`, `TURNING_ON`, and `ON`.

#### 22. Button Toggles Saber Power

Use the button to switch between off and on.

#### 23. Non-Blocking Timing with Millis

Replace some `delay()` calls with `millis()`.
This helps the saber keep reacting while it runs animations.

#### 24. Organize Code into Functions

Split the code into smaller functions like `readGyro()` and `updateBlade()`.

#### 25. Configuration Section

Move important settings to the top of the file.
This makes the sketch easier to adjust.

### Sound

#### 26. Buzzer Test

Play one simple tone on the buzzer.

#### 27. Startup Sound

Play a short tone pattern when the blade turns on.

#### 28. Swing Sound

Play a short sound when the saber swings.

#### 29. Clash Sound

Play a sharper sound for a clash.

#### 30. Final Integrated Lightsaber

Combine button control, blade animation, motion detection, and sound into one full lightsaber project.
