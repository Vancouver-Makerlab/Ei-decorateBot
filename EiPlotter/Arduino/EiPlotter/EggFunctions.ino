
void check_PRGButton()
{
  // Update the Bounce instance :
  debouncer.update();

  // Get the updated value :
  int value = debouncer.read();

  // Turn on or off the LED as determined by the state :
  if (value == LOW)
  {
    button_set = 1;
  }
}


void sendOK()
{
  Serial.print("OK\r\n");

#ifdef DEBUG2
  mySerial.print("OK\r\n");
#endif
}

void sendError()
{
  Serial.print("I'm sorry Dave, I'm afraid I can't do that.");
}

void sendParser()
{

#ifdef DEBUG2
  mySerial.println("Parser Results");

  mySerial.print("Command: ");
  mySerial.println(Command);

  mySerial.print("ArgNumber: ");
  mySerial.println(ArgNumber);

  mySerial.print("Int: ");
  for (byte i = 0; i < ArgNumber; i++)
  {
    mySerial.print(Arg[i]);
    mySerial.print(" / ");

  }
  mySerial.print("\r\n");
#endif

}

void send0()
{
  Serial.print("0\r\n");
#ifdef DEBUG2
  mySerial.print("0\r\n");
#endif
  sendOK();
}

void send1()
{
  Serial.print("1\r\n");
#ifdef DEBUG2
  mySerial.print("1\r\n");
#endif
  sendOK();
}

// waits until all steppers reach their target position and are stopped
void wait_for_motors()
{
  while (steppers.run())
    check_PRGButton();
}


