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
		- [Important note for Step 3](#important-note-for-step-3)
	- [Shared Pin Plan](#shared-pin-plan)
	- [The Steps](#the-steps)
		- [Basics](#basics)
			- [1. Empty Sketch](#1-empty-sketch)
			- [2. Serial Hello World](#2-serial-hello-world)
			- [3. Blink with Serial Messages](#3-blink-with-serial-messages)
			- [4. Read a Button](#4-read-a-button)
			- [5. Button Controls an LED](#5-button-controls-an-led)
		- [LED Blade](#led-blade)
			- [6. Turn On One Pixel of the LED Strip](#6-turn-on-one-pixel-of-the-led-strip)
			- [7. Turn On the Entire Strip](#7-turn-on-the-entire-strip)
			- [8. Change Led Strip Color](#8-change-led-strip-color)
			- [9. Led Strip On and Off](#9-led-strip-on-and-off)
			- [10. Ignition Animation](#10-ignition-animation)
			- [11. Shutdown Animation](#11-shutdown-animation)
			- [12. Button Toggles Led Strip On and Off](#12-button-toggles-led-strip-on-and-off)
		- [Motion and Sensors](#motion-and-sensors)
			- [13. Print Raw Acceleration Data](#13-print-raw-acceleration-data)
			- [13B. Orientation Calibration](#13b-orientation-calibration)
			- [14. Orientation and Acceleration](#14-orientation-and-acceleration)
			- [14B. Movement Calibration](#14b-movement-calibration)
			- [15. Detect Movement](#15-detect-movement)
			- [16. Swing Detection](#16-swing-detection)
			- [17. Light Flash on Swing](#17-light-flash-on-swing)
			- [18. Detect Strong Impact](#18-detect-strong-impact)
			- [19. Clash Flash Effect](#19-clash-flash-effect)
			- [20. Idle Led Strip Effect](#20-idle-led-strip-effect)
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
	- [Extra's](#extras)
		- [special\_scripts](#special_scripts)
			- [Step 30B](#step-30b)
		- [WLED](#wled)
			- [What WLED is good for](#what-wled-is-good-for)
			- [Important note for the XIAO ESP32-C3](#important-note-for-the-xiao-esp32-c3)
			- [Flash WLED](#flash-wled)
			- [Connect to WLED and configure the strip](#connect-to-wled-and-configure-the-strip)
			- [Go back to Arduino later](#go-back-to-arduino-later)


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

### Important note for Step 3

The Seeed Studio XIAO ESP32-C3 does not use a normal built-in user LED for these beginner blink lessons.

For `step_03_blink_with_serial`, use an external LED:

- Connect `D8` to a `220-330 ohm` resistor
- Connect the resistor to the long leg of the LED
- Connect the short leg of the LED to `GND`

`220 ohm` is a good default choice.

## Shared Pin Plan

These sketches mostly use the same pins so students do not have to relearn the wiring each time.

- Single practice LED for step 3: `D2`
- LED strip data pin: `D8` on XIAO ESP32-C3, `GPIO2` on ESP32-C3 Super Mini
- Button pin: `D7` on XIAO ESP32-C3, `GPIO0` on ESP32-C3 Super Mini
- Button LED pin: `D2` on XIAO ESP32-C3, `GPIO7` on ESP32-C3 Super Mini
- Buzzer pin: `D5` on XIAO ESP32-C3, `GPIO10` on ESP32-C3 Super Mini
- Gyroscope SCL pin: `D0` on XIAO ESP32-C3, `GPIO5` on ESP32-C3 Super Mini
- Gyroscope SDA pin: `D10` on XIAO ESP32-C3, `GPIO4` on ESP32-C3 Super Mini

If your class wiring is different, update the pin numbers at the top of each sketch.

For the ESP32-C3 Super Mini, set `USE_SUPER_MINI_PINS` to `1` near the top of each relevant sketch.

## The Steps

Each lesson has its own numbered sketch folder.

Start at Step 1 and move forward in order:

- `step_01_empty_sketch`
- `step_02_serial_hello_world`
- `step_03_blink_with_serial`
- `step_04_read_button`
- `step_05_button_controls_led`
- `step_06_one_pixel`
- `step_07_full_strip`
- `step_08_change_blade_color`
- `step_09_blade_on_off`
- `step_10_ignition_animation`
- `step_11_shutdown_animation`
- `step_12_button_toggle_blade`
- `step_13a_raw_acceleration`
- `Special_Scripts/step_13b_orientation_calibration/step_13b_orientation_calibration.ino`
- `step_14_read_gyroscope`
- `Special_Scripts/step_14b_movement_calibration/step_14b_movement_calibration.ino`
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

#### 3. Blink with Serial Messages

Blink the LED while printing `ON` and `OFF` in the Serial Monitor.
This connects the code to what students can see.

#### 4. Read a Button

Connect a button and print whether it is pressed or not.
Students learn how the board reads input.

#### 5. Button Controls an LED

Pressing the button turns the LED on and releasing it turns it off.
This introduces simple `if` statements.

### LED Blade

#### 6. Turn On One Pixel of the LED Strip

Initialize the LED strip library and light one pixel.
Students learn that each LED has its own number.

#### 7. Turn On the Entire Strip

Use a loop to set every LED to the same color.
This introduces loops.

#### 8. Change Led Strip Color

Change the RGB values to make different colors.
Students learn how digital color works.

#### 9. Led Strip On and Off

Create functions that turn the strip on and off.
This introduces reusable code.

#### 10. Ignition Animation

Light the Led Strip one LED at a time from base to tip.
This introduces animation.

#### 11. Shutdown Animation

Reverse the animation from tip to base.
This shows how changing a loop changes behavior.


#### 12. Button Toggles Led Strip On and Off

Use the button to switch the Led Strip fully on or fully off.
This connects button input to a persistent output state.

### Motion and Sensors

#### 13. Print Raw Acceleration Data

Print the raw X, Y, and Z acceleration values from the MPU6050.
This helps students see the sensor data before interpreting it.

#### 13B. Orientation Calibration

Use the MPU6050 accelerometer to estimate which way the lightsaber is pointing.
This builds on the raw values by mapping them to simple orientation labels.

#### 14. Orientation and Acceleration

Print the acceleration values and the current orientation label.
Students learn how live sensor numbers map to a simple interpreted state.

#### 14B. Movement Calibration

Print the raw gyroscope values and show a simple interpreted rotation state.
This introduces that gyroscope data measures rotational speed and direction.

#### 15. Detect Movement

Use a threshold to decide if the saber is moving.

#### 16. Swing Detection

When movement is strong enough, trigger a simple reaction.
This introduces event-based behavior.

#### 17. Light Flash on Swing

When a swing is detected, flash the Led Strip brighter for a moment.

#### 18. Detect Strong Impact

Use a bigger threshold to detect a clash.

#### 19. Clash Flash Effect

When a strong impact is detected, flash the Led Strip white.

#### 20. Idle Led Strip Effect

Add a small flicker while the Led Strip is on.
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

Play a short tone pattern when the Led Strip turns on.

#### 28. Swing Sound

Play a short sound when the saber swings.

#### 29. Clash Sound

Play a sharper sound for a clash.

#### 30. Final Integrated Lightsaber

Combine button control, Led Strip animation, motion detection, and sound into one full lightsaber project.

## Extra's

### special_scripts

The `special_scripts` folder contains helper sketches that are useful for setup, troubleshooting, and calibration.
They are not part of the main step-by-step lesson flow, but they help you test hardware or tune sensor behavior before continuing with the normal lessons.

- `basic_power_up`: a simple combined demo with button, LED strip, buzzer, and optional orientation behavior
- `special_find_led_strip_pin`: tests candidate output pins one by one so you can find which pin is connected to the LED strip data line
- `special_read_button_all_pins`: reads all candidate button pins and prints which one goes `PRESSED` when the button is wired to ground
- `step_13b_orientation_calibration`: prints accelerometer values and interpreted saber orientation so you can adjust the MPU6050 axis mapping
- `step_13c_orientation_colors`: changes blade colors based on orientation to visually confirm the orientation mapping is correct
- `step_14b_movement_calibration`: prints gyroscope rotation values so you can check movement directions and tune rotation thresholds

Use these sketches when something in the hardware setup is unclear, or when the MPU6050 orientation does not match the expected lesson wiring.

#### Step 30B

Here there are different modes added.


### WLED

WLED is a ready-made LED controller firmware for ESP boards.
It gives you a web interface, colors, effects, playlists, and brightness control without writing Arduino code.

#### What WLED is good for

Use WLED if you want to:

- test that your LED strip and power wiring work
- try Led Strip colors and animations quickly
- control the saber LEDs from a phone or browser

Use the sketches in this repo if you want to:

- learn programming step by step
- read the button and gyroscope yourself
- build your own lightsaber logic in code

#### Important note for the XIAO ESP32-C3

WLED supports ESP32 boards, but ESP32-C3 support is still considered experimental.
That means it may work well for testing or demos, but it is not the safest choice for a classroom build that must behave the same on every board.

Also note that WLED and the Arduino sketches in this repo are different firmware.
Uploading WLED replaces the sketch currently on the board until you upload an Arduino sketch again.

#### Flash WLED

1. Disconnect extra jumper wires if your setup is unstable during flashing.
2. Plug the XIAO ESP32-C3 into your computer with USB.
3. Try the WLED web installer first.
4. If the web installer does not work, use the current WLED ESP32 release binary and flash it with `esptool` or an ESP flash tool.
5. After flashing, reboot the board.

If the flasher pauses on `Connecting...`, hold the board's boot button while flashing starts.

#### Connect to WLED and configure the strip

After a successful flash:

1. Join the Wi-Fi network `WLED-AP`
2. Use password `wled1234`
3. Open `http://4.3.2.1` in a browser
4. Open `Config > LED Preferences`
5. Set the LED type to match your Led Strip strip, such as `WS281x`
6. Set the data pin to `GPIO2`
7. Set the LED count to match your blade, such as `60`
8. Save and reboot if asked

For this repo's current wiring, the physical XIAO pin label for the Led Strip data line is `D0`, which maps to `GPIO2` in WLED.

WLED can drive the LED strip, but it does not automatically use the button, gyroscope, and buzzer logic from these lesson sketches.

#### Go back to Arduino later

If you want to return to the lessons:

1. Open Arduino IDE
2. Select the XIAO ESP32-C3 board and correct port
3. Open any lesson sketch from this repo
4. Click `Upload`

That upload replaces WLED on the board.



