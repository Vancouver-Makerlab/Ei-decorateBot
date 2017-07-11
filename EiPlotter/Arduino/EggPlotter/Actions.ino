
void sendID()
{
  Serial.println("EBB Hacker Version");
#ifdef DEBUG
  mySerial.println("EBB Hacker Version");
#endif
}

void StepperConfig()
{
  if (ArgNumber != 2) // 2 arguments are expected
  {
    sendError();
    return;
  }

  sendOK();

  if (Arg[0] == 4)
  {
    servo_up_pos = Arg[1] / 12;
  }
  else if (Arg[0] == 5)
  {
    servo_down_pos = Arg[1] / 12;
  }
}


// Execute the SM command
void StepperMove()
{
  long positions[2];      // Array of desired new stepper positions (needed for multistepper)
  long maxstep = 0;       // Used to calculate stepper speed
  long stepperspeed = 0;  // Stepper speed for mulstistepper

  // check for wrong number of arguments
  if ((ArgNumber < 2) || (ArgNumber > 3)) // 2 or three arguments are expected
  {
    sendError();
    return;
  }

  // Acknowledge command is correct
  sendOK();
  //sendParser(); // debug only

  // If there is only speed argument plus one position argument, egg movement is ZERO
  if (ArgNumber == 2)
    Arg[2] = 0;

  if ((Arg[1] == 0) && (Arg[2] == 0))     // if both motors are not to move, just execute delay
  {
    delay(Arg[0]);
    return;
  }

  // wait for the end of the previous operation
  wait_for_motors();

  //
  // Backlash Compensation
  // introduces extra steps in new direction, where necessary

  // Calculate new pen/egg directions
  newPenDirection = constrain(Arg[1], -1, 1); // get the sign
  newEggDirection = constrain(Arg[2], -1, 1); // get the sign

  // if at least one of the motors was running in the opposite direction (last time it ran)
  // do a backlash compensation by inserting an extra movement. This extra movement compensates
  // the backlash, but shouldn't actually move pen or egg

  if ((newPenDirection == -1 * oldPenDirection) || (newEggDirection == -1 * oldEggDirection))
  {
    // calculate step to insert
    if (newPenDirection == -1 * oldPenDirection)
      positionPen += compensatePen * newPenDirection;
    if (newEggDirection == -1 * oldEggDirection)
      positionEgg += compensateEgg * newEggDirection;

    // move there
    positions[0] = positionPen;
    positions[1] = positionEgg;

#ifdef DEBUG_BL
    //DEBUG
    mySerial.print("PositionBL, ");
    mySerial.print(positionPen);
    mySerial.print(", ");
    mySerial.println(positionEgg);
#endif


    // calculate stepper speeds
    maxstep = max(abs(Arg[1]), abs(Arg[2]));
    stepperspeed = maxstep * 1000 / Arg[0];

    stepperPen.setMaxSpeed(stepperspeed);
    stepperEgg.setMaxSpeed(stepperspeed);

    // set new target positions
    steppers.moveTo(positions);

    // wait for end of movement
    wait_for_motors();


  }

#ifdef DEBUG_BL
  mySerial.print("Directions, ");
  mySerial.print((int)oldPenDirection);
  mySerial.print(", ");
  mySerial.print((int)oldEggDirection);
  mySerial.print(", ");
  mySerial.print((int)newPenDirection);
  mySerial.print(", ");
  mySerial.println((int)newEggDirection);
#endif

  // update old directions
  if ((oldPenDirection == 0) or (newPenDirection != 0)) // if we just started, oldDir is 0 and we need to update
  { // but keep old direction if new direction is 0
    oldPenDirection = newPenDirection;
  }
  if ((oldEggDirection == 0) or (newEggDirection != 0)) // if we just started, oldDir is 0 and we need to update
  { // but keep old direction if new direction is 0
    oldEggDirection = newEggDirection;
  }

  // Calculate new target positions from command arguments
  positionPen += Arg[1];
  positionEgg += Arg[2];

  positions[0] = positionPen;
  positions[1] = positionEgg;

  maxstep = max(abs(Arg[1]), abs(Arg[2]));
  stepperspeed = maxstep * 1000 / Arg[0];

#ifdef DEBUG_BL
  mySerial.print("Position, ");
  mySerial.print(positionPen);
  mySerial.print(", ");
  mySerial.println(positionEgg);
#endif


#ifdef DEBUG2
  mySerial.print("Speed: ");
  mySerial.println(stepperspeed);
  mySerial.print("MAXStep: ");
  mySerial.println(maxstep);
  mySerial.println(Arg[1]);
  mySerial.println(Arg[2]);
#endif

  stepperPen.setMaxSpeed(stepperspeed);
  stepperEgg.setMaxSpeed(stepperspeed);

  steppers.moveTo(positions);

}

void EnableMotors()
{
  sendOK();
}

void Engraver()
{
  sendOK();
}


void QueryButton()
{
  if (button_set == 1)
  {
    send1();
    button_set = 0;
  }
  else
    send0();
}

void QueryPenState()
{
  if (pen_up == 1)
    send1();
  else
    send0();
}


void SetPen()
{
  if ((ArgNumber < 1) || (ArgNumber > 3)) // 1 .. 3 arguments are expected
  {
    sendError();
    return;
  }
  sendOK();

  // wait for the end of the previous operation
  wait_for_motors();


  if (Arg[0] == 0)
  {
    myservo.writeMicroseconds(servo_down_pos);  // pen down
    pen_up = 0;
  }
  else
  {
    myservo.writeMicroseconds(servo_up_pos);  // pen up
    pen_up = 1;
  }

  if (ArgNumber > 1)
  {
    delay(Arg[1]);
  }
}

void TogglePen()
{
  if (ArgNumber > 1)
  {
    sendError();
    return;
  }

  sendOK();

  // wait for the end of the previous operation
  wait_for_motors();


  if (pen_up == 1)
  {
    myservo.writeMicroseconds(servo_down_pos);  // pen down
    pen_up = 0;
  }
  else
  {
    myservo.writeMicroseconds(servo_up_pos);  // pen up
    pen_up = 1;
  }

  if (ArgNumber > 1)
  {
    delay(Arg[0]);
  }
}


