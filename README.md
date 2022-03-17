![My Arduino Car](/media/pose-2.jpeg)

# Arduino Car

A simple remote-controlled car with built-in crash avoidance.

## Features

This simple Arduino car comes with the following features:

- Website to connect to and control the car via bluetooth ([app folder](/app)).
  - Website is deployed to Cloudflare Workers (via wrangler).
  - Website implements the Web Bluetooth API to connect to the car (Note: Does not work on all browsers and devices).
- Arduino Uno connected to:
  - Bluetooth Module for remote control.
  - Two Motors for the car (four motors installed but only two connected).
  - A sonar sensor to detect obstacles (distance) in the front.
  - A passive buzzer to warn for a crash (based on distance).
  - A debug LED that signals the status of the car.
- ([Arduino Uno Sketch](/sketch)) implements a execution loop including:
  - Check for command from the website (Bluetooth).
  - Check for obstacles in front.
  - Execute next command based on the two checks.

![My Arduino Car](/media/pose-3.jpeg)

## TODO LIST

- Deploy to Cloudflare Workers [X]
- Add ESLint/Prettier Setup [X]
- Set up Tailwind CSS [X]
- Add Cool Font [X]
- Add Connect Button [X]
- Add [Bluetooth Logic to App](https://web.dev/bluetooth/#write) (GATT connect and write) [X]
- Add Bluetooth Logic to Arduino [X]
- Add Car Controls to Website [X]
- Send Controls to Arduino [X]

![My Arduino Car](/media/pose-1.jpeg)

## Setting up the Bluetooth connection

1. Connect Bluetooth Device to your PC (might require a passcode depending on Bluetooth module - `AT+PIN` command - set to 123456 through Arduino)

2. Click "Connect"-Button in web app

References:

- [Great article](https://loginov-rocks.medium.com/how-to-make-a-web-app-for-your-own-bluetooth-low-energy-device-arduino-2af8d16fdbe8) detailing how the Bluetooth Web API works.
- [Characteristic Properties Sample](https://googlechrome.github.io/samples/web-bluetooth/characteristic-properties.html)
- [Notifications Sample](https://googlechrome.github.io/samples/web-bluetooth/notifications.html)
- [Arduino Sample](https://create.arduino.cc/projecthub/mayooghgirish/arduino-bluetooth-basic-tutorial-d8b737)
