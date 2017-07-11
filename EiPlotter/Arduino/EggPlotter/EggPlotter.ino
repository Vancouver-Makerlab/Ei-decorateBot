/***************************************************************************************************

  EggPlotter - A low-cost EggBot clone

  Controls two stepper motors (28byj-48) and one servo to draw on eggs and other
  spherical shapes. It communicates over the serial (USB2serial) interface with a
  PC to receive commands.

  This sketch was written to work with InkScape and the modified EggBot
  extension V 2.5.0A. Please read about it on my web site.

  In order to compile, install these libraries:
  AccelStepper - http://www.airspayce.com/mikem/arduino/AccelStepper/
  Bounce2 - https://github.com/thomasfredericks/Bounce2

  Written by Thomas Frey  http://tomscircuits.blogspot.com/

  $Date: 2013-06-28 23:03:56 +0200 (Fr, 28 Jun 2013) $

  This work is licensed under the Creative Commons
  Attribution-NonCommercial-ShareAlike 3.0 Unported License.
  To view a copy of this license, visit
  http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a
  letter to Creative Commons, 171 Second Street, Suite 300,
  San Francisco, California, 94105, USA.

***************************************************************************************************/


//#define DEBUG
//#define DEBUG_BL

#define compensatePen 10
#define compensateEgg 30

// Command parser variables
byte index = 0;
byte line_end = 0;

// Parser result variables
char Command[3];    // the command consists of one or two letters
long int  Arg[5];   // the argument values (all arguments are numbers)
int ArgNumber = 0;  // number of arguments

// Stepper target position variables
long int positionPen = 0;
long int positionEgg = 0;

// Stepper backlash compensation variables
char oldPenDirection = 0;
char oldEggDirection = 0;
char newPenDirection = 0;
char newEggDirection = 0;

// Button state
byte button_set = 0;

// Servo variables
byte pen_up = 1;
int servo_up_pos = 1850;
int servo_down_pos = 1300;

// do the stepper stuff
#include <AccelStepper.h>
#include <MultiStepper.h>

// create two steppers
AccelStepper stepperPen(AccelStepper::HALF4WIRE, 2, 4, 3, 5); // moves the pen holder
AccelStepper stepperEgg(AccelStepper::HALF4WIRE, 6, 8, 7, 9); // turns the egg

// group them
MultiStepper steppers;

// create a debounced PRG/PAUSE button
#include <Bounce2.h>
#define BUTTON_PIN 12

// Create a servo
#include <Servo.h>
Servo myservo;  // create servo object to control a pen servo


// Instantiate a Bounce object
Bounce debouncer = Bounce();


#ifdef DEBUG
// debugging only
#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 10); // RX, TX
#endif

void setup()
{
  // initialize serial:
  Serial.begin(9600);
  // make the LED pin output:

  // Configure each stepper
  stepperPen.setMaxSpeed(500);
  stepperEgg.setMaxSpeed(500);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepperPen);
  steppers.addStepper(stepperEgg);

  // PRG button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(50);         // interval in ms


  // pen servo
  myservo.attach(13);                        // attaches servo on pin 13 to the servo object
  myservo.writeMicroseconds(servo_up_pos);   // start in pen_up position
  delay(100);                                // waits 100ms for the servo to reach the position

#ifdef DEBUG
  // debugging only
  mySerial.begin(19200);
  mySerial.println("EggPlotter Debug Info\r\n\r\n");
#endif

}


void loop()
{
  char inData[80];
  char asciiArg[20];

  // run motors if movement pending
  steppers.run();

  // is button pressed?
  check_PRGButton();

  // if there's any serial data available, read it:
  while (Serial.available() > 0)
  {
    char aChar = Serial.read();
    if (aChar == '\r')
    {
      // End of line detected.
      inData[index] = '\0';
      line_end = 1;
    }
    else
    {
      inData[index] = aChar;
    }
    index++;
  }

  // Parsing the input string
  if (line_end > 0)
  {

#ifdef DEBUG
    mySerial.print("\nInput String: ");
    mySerial.println(inData);
#endif

    // Read Command
    Command[0] = inData[0];
    Command[1] = inData[1];
    Command[2] = '\0';

    // set index to start of first argument
    if (inData[1] == ',')   // one-letter command plus argument
    {
      Command[1] = '\0';
      index = 2;
    }
    else
    {
      index = 3;
    }

    //if no argument follows, set index to the \0 location
    if (inData[1] == '\0')    // one-letter command
      index = 1;
    if (inData[2] == '\0')    // two-letter command (or one-letter command with two \0...)
      index = 2;

    // Read Arguments
    int i = 0;
    ArgNumber = 0;

    while (inData[index] != '\0')
    {
      asciiArg[i++] = inData[index++];
      if ((inData[index] == ',') || (inData[index] == '\0'))
      {
        asciiArg[i] = '\0';

        //        Serial.print("Arg: ");
        //        Serial.println(asciiArg);

        Arg[ArgNumber] = atol(asciiArg);
        if (ArgNumber < 4)          // Not more than 4 arguments
          ArgNumber++;
        if (inData[index] == ',')   // Skip next comma, but not \0
          index++;
        i = 0;
      }
    }

    index = 0;
    line_end = 0;

    // Commands to execute:
    //
    // v        - respond with ID string
    // SC 4+5   - servo limit settings
    // SC 11+12 - servo rate settings
    // SM       - move stepper motor
    // EM       - enable / disable stepper motors
    // QB, QP   - query PRG button and query pen state
    // SP       - pen up+down


    if (Command[0] == 'v') sendID();
    else if ((Command[0] == 'S') && (Command[1] == 'C')) StepperConfig();
    else if ((Command[0] == 'S') && (Command[1] == 'M')) StepperMove();
    else if ((Command[0] == 'E') && (Command[1] == 'M')) EnableMotors();
    else if ((Command[0] == 'Q') && (Command[1] == 'B')) QueryButton();
    else if ((Command[0] == 'Q') && (Command[1] == 'P')) QueryPenState();
    else if ((Command[0] == 'S') && (Command[1] == 'P')) SetPen();
    else if ((Command[0] == 'T') && (Command[1] == 'P')) TogglePen();
    else if ((Command[0] == 'P') && (Command[1] == 'O')) Engraver();
    else sendError();
    Command[0] = '\0';  // command was executed and is now invalidated
  }
}






