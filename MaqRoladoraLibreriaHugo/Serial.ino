void serialEvent()
{
  if (Serial.available())
  {
    // get the new byte:
    char inChar = (char) Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\r')
    {
      stringComplete = true;
    }
    if (inChar == ' ')
    {
      inputString = "";
    }
  }
}

void mensajeBienvenida()
{
  lcd.begin();
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
  PWMOn = 0;
  lcd.setCursor(0, 0);
  lcd.print("Programa Roladora");
  lcd.setCursor(0, 1);
  lcd.print("Bienvenido");
  delay(2000);
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0);
  lcd.print(verticalmm);
  lcd.print("mm");
}


