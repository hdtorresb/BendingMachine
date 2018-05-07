void pwmFunctions() {
  if (inputString == "pwmOn") {
    PWMOn = 1;
    inputString = "";
  }

  if (inputString == "pwmOff") {
    PWMOn = 0;
    digitalWrite(Presion_mas, LOW);
    inputString = "";
  }
}


void onMotor() {
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
  digitalWrite(Retroceso, HIGH);
  if (ClearLCDLeft || ClearLCDRight) {
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    ClearLCDLeft = 0;
    ClearLCDRight = 0;
  }
}
void onMotorAvance() {
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
  digitalWrite(Avance, HIGH);
  if (ClearLCDLeft || ClearLCDRight) {
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    ClearLCDLeft = 0;
    ClearLCDRight = 0;
  }
  }

  void offMotor() {
    digitalWrite(Bomba, LOW); //initialize in low state
    digitalWrite(Presion_menos, LOW);
    digitalWrite(Retroceso, LOW);
  }

  void offMotorAvance() {
    digitalWrite(Bomba, LOW); //initialize in low state
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Avance, LOW);
  }

