//www.elegoo.com
//2016.12.08

// libaries
/* Libraries */
// Sonar sensor
#include "SR04.h"
// Bluetooth Serial
#include <SoftwareSerial.h>

/* Pins */
// Sonar sensor
#define TRIG_PIN 12
#define ECHO_PIN 11
// Active buzzer
#define BUZZER_PIN 13
// LED
// #define LED_BLUE 3 // => currently not connected
#define LED_GREEN 5 // => waiting for data
#define LED_RED 6 // => error / warning
// Bluetooth
#define RECEIVE_PIN 9
#define TRANSMIT_PIN 8
// Motor Left
#define MOTOR_ENABLE 3
#define MOTOR_DIRA 10
#define MOTOR_DIRB 2


// Note: Everything below 10cm becomes very inaccurate. Below 5cm almost never works. We use 10cm to be safe!
#define SAFETY_DISTANCE 10

// Possible Commands
#define FORWARDS 'W'
#define BACKWARDS 'S'
#define STOP 'Q'
#define NOOP 'N'

// Setup sonar sensor interface
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);

// Serial Bluetooth
SoftwareSerial SerialBt(TRANSMIT_PIN, RECEIVE_PIN);

// Can be FORWARDS | BACKWARDS | STOP | NOOP
char nextCommand = NOOP;

/*
 * soundAlarm triggers the active buzzer
 */
void soundAlarm() {
  Serial.println("Starting alarm...");
  digitalWrite(BUZZER_PIN, HIGH);
}

/*
 * soundAlarm silences the active buzzer
 */
void soundSilence() {
  Serial.println("Stopping alarm...");
  digitalWrite(BUZZER_PIN, LOW);
}

/*
 * setLED can be called to set the LED into a state 
 * Param red, green, and blue are color values between 0 and 255
 */
void setLED(int red, int green, int blue) {
  if(red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255) {
    Serial.println("Error! Not a valid LED color value");
    return;
  }
  // Serial.println("Switching color of LED...");
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  // analogWrite(LED_BLUE, blue);
}

void setupLED() {
  Serial.println("Setting up LED...");
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  // pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  // digitalWrite(LED_BLUE, LOW);
}

void setupMotor() {
  Serial.println("Setting up motor...");
  pinMode(MOTOR_ENABLE,OUTPUT);
  pinMode(MOTOR_DIRA,OUTPUT);
  pinMode(MOTOR_DIRB,OUTPUT);
}

void driveTest() {
  //---back and forth example
  Serial.println("One way, then reverse");
  digitalWrite(MOTOR_ENABLE,HIGH); // enable on
  for (int i=0;i<5;i++) {
    digitalWrite(MOTOR_DIRA,HIGH); //one way
    digitalWrite(MOTOR_DIRB,LOW);
    delay(500);
    digitalWrite(MOTOR_DIRA,LOW);  //reverse
    digitalWrite(MOTOR_DIRB,HIGH);
    delay(500);
  }
  digitalWrite(MOTOR_ENABLE,LOW); // disable
  delay(2000);
  
  Serial.println("fast Slow example");
  //---fast/slow stop example
  digitalWrite(MOTOR_ENABLE,HIGH); //enable on
  digitalWrite(MOTOR_DIRA,HIGH); //one way
  digitalWrite(MOTOR_DIRB,LOW);
  delay(3000);
  digitalWrite(MOTOR_ENABLE,LOW); //slow stop
  delay(1000);
  digitalWrite(MOTOR_ENABLE,HIGH); //enable on
  digitalWrite(MOTOR_DIRA,LOW); //one way
  digitalWrite(MOTOR_DIRB,HIGH);
  delay(3000);
  digitalWrite(MOTOR_DIRA,LOW); //fast stop
  delay(2000);
  
  Serial.println("PWM full then slow");
  //---PWM example, full speed then slow
  analogWrite(MOTOR_ENABLE,255); //enable on
  digitalWrite(MOTOR_DIRA,HIGH); //one way
  digitalWrite(MOTOR_DIRB,LOW);
  delay(2000);
  analogWrite(MOTOR_ENABLE,180); //half speed
  delay(2000);
  analogWrite(MOTOR_ENABLE,128); //half speed
  delay(2000);
  analogWrite(MOTOR_ENABLE,50); //half speed
  delay(2000);
  analogWrite(MOTOR_ENABLE,128); //half speed
  delay(2000);
  analogWrite(MOTOR_ENABLE,180); //half speed
  delay(2000);
  analogWrite(MOTOR_ENABLE,255); //half speed
  delay(2000);
  digitalWrite(MOTOR_ENABLE,LOW); //all done
  delay(10000);
}

void driveBackwards() {
  Serial.println("Driving backwards...");
  digitalWrite(MOTOR_ENABLE,HIGH);
  digitalWrite(MOTOR_DIRA,HIGH);
  digitalWrite(MOTOR_DIRB,LOW);
}

void driveForwards() {
  Serial.println("Driving forwards...");
  digitalWrite(MOTOR_ENABLE,HIGH);
  digitalWrite(MOTOR_DIRA,LOW);
  digitalWrite(MOTOR_DIRB,HIGH);
}

void stopDriving() {
  Serial.println("Stop driving...");
  digitalWrite(MOTOR_ENABLE,LOW);
}

/* 
 * executeCommand takes the next global command and executes it
 */
void executeCommand() {
  Serial.print("Executing command...");
  Serial.println(nextCommand);
  if(nextCommand == FORWARDS) {
    driveForwards();
  } else if(nextCommand == BACKWARDS) {
    driveBackwards();
  } else if(nextCommand == STOP) {
    stopDriving();
    nextCommand = NOOP;
  }
}


/* 
 * monitorEnvironment monitors the environment using the sonar sensor
 * The function sounds alarm and stops the car if we are about to collide with an object
 */
int lastDistance = 0;
void monitorEnvironment() {
  // Serial.println("Monitoring environment...");
  long sonarDistance = sr04.Distance();
  if(sonarDistance != lastDistance) {
    Serial.print(sonarDistance);
    Serial.println("cm");
    lastDistance = sonarDistance;
  }
  if(sonarDistance <= SAFETY_DISTANCE) {
    Serial.println("Stop the car, we are about to crash!");
    soundAlarm();
    setLED(255, 0, 0);
    if(nextCommand == FORWARDS) {
      // the security feature: stop if forward and about to crash
      nextCommand = STOP;
    }
  } else {
    soundSilence();
    setLED(0, 255, 0);
  }
}

/*
 * Note: The default password for the bluetooth connection is 1234
 */
String msg = "";
bool receivedMessage = false;
void monitorBluetooth() {
  // Serial.println("Monitoring for bluetooth messages...");
  while (SerialBt.available()) {
    // Read the incoming data
    char value = SerialBt.read();  
    msg = msg + value; 
    if(msg[msg.length() - 1] == '\n' && msg[msg.length() - 2] == '\r') {
        receivedMessage = true;
        Serial.print("Received new message from bluetooth module: "); 
        Serial.println(msg);
        if(msg[0] == FORWARDS || msg[0] == BACKWARDS || msg[0] == STOP) {
          nextCommand = msg[0];
        }
        msg = "";
     }   
  }
}

/* 
 * writeCmdToBluetoothModule writes one AT command to the connected Bluetooth module.
 */
void writeCmdToBluetoothModule(String cmdInput) {
  Serial.println("Writing command to bluetooth module...");
  if(cmdInput[cmdInput.length() - 1] == '\n' && cmdInput[cmdInput.length() - 2] == '\r') {
    Serial.print("Writing cmd to bluetooth module: "); 
    Serial.println(cmdInput);
    char * hmacKey = new char [cmdInput.length() + 1];
    strcpy (hmacKey, cmdInput.c_str());
    SerialBt.write(hmacKey);
  } else {
    Serial.println("Error: cmdInput has no ending CR+LF"); 
  }
}

/* 
 * transmitCmdsToBluetoothDevice can be included in the loop function
 * Once included, you can send AT commands to the connected Bluetooth module.
 * Note: Make sure to select "Both NL & CR" and 9600 baud
 */
String cmdInput = "";
void transmitCmdsToBluetoothDevice() {
  Serial.println("Transmitting commands to bluetooth device...");
  while (Serial.available()) {
    char next = Serial.read();
    cmdInput = cmdInput + next;
    if(cmdInput[cmdInput.length() - 1] == '\n' && cmdInput[cmdInput.length() - 2] == '\r') {
      Serial.println(cmdInput);
      writeCmdToBluetoothModule(cmdInput);
      cmdInput = "";
    }
  }
}

/* 
 * initBluetoothModule writes required AT commands with values to the connected Bluetooth module.
 * The function writes one command per loop iteration to ensure that we receive a reply before we write the next one
 */
int cmdCount = 0;
void initBluetoothModule() {
  Serial.println("Installing bluetooth module state...");
  if(cmdCount == 0) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT\r\n");
    cmdCount = cmdCount + 1;
  } else if(receivedMessage && cmdCount == 1) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT+NAME\r\n");
    cmdCount = cmdCount + 1;
  } else if(receivedMessage && cmdCount == 2) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT+NAMECarCtrl\r\n"); // set module name to "CarCtrl"
    cmdCount = cmdCount + 1;
  } else if(receivedMessage && cmdCount == 3) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT+UUID0xFFE0\r\n"); // set service UUID as 0xFFE0
    cmdCount = cmdCount + 1;
  } else if(receivedMessage && cmdCount == 4) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT+CHAR0xFFE1\r\n"); // set characteristic UUID as 0xFFE1
    cmdCount = cmdCount + 1;
  } else if(receivedMessage && cmdCount == 5) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT+PIN\r\n"); // reading pin
    cmdCount = cmdCount + 1;
  } else if(receivedMessage && cmdCount == 6) {
    receivedMessage = false;
    writeCmdToBluetoothModule("AT+PIN123456\r\n"); // set pin to 123456
    cmdCount = cmdCount + 1;
  }
}

void setup() {
  Serial.println("Setting up...");
  Serial.begin(9600);  //Sets the data rate in bits per second (baud) for serial data transmission
  SerialBt.begin(9600); // Sets the data rate in bits per second (baud) for the Bluetooth serial data transmission
  pinMode(BUZZER_PIN, OUTPUT); //initialize the buzzer pin as an output
  setupLED(); // initialize the LED
  setupMotor(); // initialize the Motor
  setLED(0, 255, 0);
}

void loop() {
  // Serial.println("Looping...");
  monitorBluetooth();
  delay(200);
  monitorEnvironment();
  //initBluetoothModule();
  //transmitCmdsToBluetoothDevice();
  executeCommand();
  //driveTest();
}
