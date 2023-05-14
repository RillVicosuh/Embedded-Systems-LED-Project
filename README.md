# Embedded Systems LED Project README

## Overview

This embedded systems project is designed to simulate a runner who changes their speed based on different external factors. The project uses an Arduino and various sensors to detect the environment and adjust the runner's speed accordingly. The speed is represented by a series of LEDs, with the position of the lit LED indicating the runner's speed.

## Components

- Arduino board
- DHT11 temperature and humidity sensor
- Ultrasonic distance sensor (HC-SR04)
- Joystick module
- Tilt switch sensor
- Piezo buzzer
- 7 LEDs
- Resistors and jumper wires

## Features

The project has the following features:

1. **Temperature and humidity adjustments**: The runner's speed is adjusted based on the temperature and humidity readings from the DHT11 sensor.
2. **Ultrasonic distance sensor**: The runner's speed is adjusted based on the distance to an obstacle detected by the HC-SR04 ultrasonic sensor.
3. **Joystick control**: The user can control the direction of the runner (left or right) and make the runner trip or rest using the joystick module.
4. **Tilt switch**: The runner's speed is adjusted based on the tilt state of the tilt switch sensor.
5. **Wind simulation**: A piezo buzzer simulates wind by generating a tone. The wind intensity affects the runner's speed and is randomly generated.

## Setup

1. Connect the components as described in the code.
2. Upload the provided code to your Arduino board.
3. Open the Arduino IDE's Serial Monitor to view any debug information.

## Operation

The program runs in a loop, continuously checking the various sensors and adjusting the runner's speed accordingly. The user can interact with the joystick to control the runner's direction and trip or rest the runner. The runner's speed is represented by a series of LEDs, with the position of the lit LED indicating the speed. The LED will move left or right depending on the direction the runner is running.

The speed of the runner will change based on the following factors:

- Running LED sequence: The LEDs will light up in sequence depending on the different environmental and input conditions. The sequence will slow down or speed up based on the readings from the sensors and user input.
- Joystick control: The user can interact with the joystick to control the LED sequence. By moving the joystick left or right, the user can change the direction of the sequence. Pressing the joystick button will pause the LED sequence, and pressing it again will resume the sequence. Moving the joystick up will speed up the sequence, and moving it down will slow down the sequence.
- Ultrasonic sensor: The device will detect obstacles within a certain range and adjust the LED sequence accordingly.
- Temperature and humidity sensor: The device will adjust the LED sequence based on temperature and humidity readings.
- Wind simulation: The device simulates wind conditions by generating random wind patterns and adjusting the LED sequence accordingly.
- Tilt switch: The device will detect a tilted position and adjust the LED sequence accordingly.
- Please note that these actions will have a combined effect on the LED sequence depending on the readings from the sensors and user input.

## Setup Visual
![image](https://github.com/RillVicosuh/Embedded-Systems-LED-Project/assets/65578557/c152d760-5d9c-4c1d-9cd5-41c452dcc978)

