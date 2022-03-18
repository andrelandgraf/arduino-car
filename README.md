![Screenshot of Car Control Webapp](/media/website.png)

# Arduino Car

A simple [remote-controlled](https://arduino-car.andre-landgraf.workers.dev/) car with built-in crash avoidance.

![My Arduino Car](/media/pose-2.jpeg)

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
- Arduino Uno Sketch implements a execution loop including ([sketch folder](/sketch)):
  - Check for command from the website (Bluetooth).
  - Check for obstacles in front.
  - Execute next command based on the two checks.

![My Arduino Car](/media/pose-3.jpeg)

## TODO LIST

- Develop website:
  - Add ESLint/Prettier Setup [X]
  - Set up Tailwind CSS [X]
  - Add Cool Font [X]
  - Add Connect Button [X]
  - Integrate with [Bluetooth API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Bluetooth_API) (GATT connect and write) [X]
  - Add Car Controls to Website [X]
- Deploy website to Cloudflare Workers [X]
- Develop Arduino Sketch (embedded code):
  - Add Bluetooth Logic [X]
  - Add Active Buzzer Logic [X]
  - Add Crash Detection Logic (Sonar) [X]
  - Add Motor Control Logic [X]
  - Add Debug LED Logic [X]
- Develop Car [X]
  - Assemble car chassis [X]
  - Connect motors to arduino [X]
  - Connect sonar to arduino [X]
  - Connect buzzer to arduino [X]
  - Connect debug LED to arduino [X]

![My Arduino Car](/media/pose-1.jpeg)

## Setting up the Bluetooth connection

**Note:** Not all browsers support the Web Bluetooth API.

1. Connect Bluetooth Device to your PC (might require a passcode depending on Bluetooth module - `AT+PIN` command - set to 123456 through Arduino)

2. Click "Connect"-Button in web app

## References:

- [Great article](https://loginov-rocks.medium.com/how-to-make-a-web-app-for-your-own-bluetooth-low-energy-device-arduino-2af8d16fdbe8) detailing how the Bluetooth Web API works.
- [Characteristic Properties Sample](https://googlechrome.github.io/samples/web-bluetooth/characteristic-properties.html)
- [Notifications Sample](https://googlechrome.github.io/samples/web-bluetooth/notifications.html)
- [Arduino Sample](https://create.arduino.cc/projecthub/mayooghgirish/arduino-bluetooth-basic-tutorial-d8b737)
