void serialEvent() {
  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\r') {
      stringComplete = true;
    }
    if (inChar == ' ') {
      inputString = "";
    }
  }
}

void comandosSerial(String datoEntrada ) {
  if (datoEntrada == "vel") {
    Timer1.start();
    Timer1.initialize(1000);          //El timer se dispara cada 1 ms
    while (contador != 0) {
      Serial.println(contador);
      digitalWrite(Avance, HIGH);
      if (_LeftEncoderTicks == -100) {
        Timer1.stop();
        digitalWrite(Avance, LOW);
        verticalmm = _LeftEncoderTicks * paso;
        Serial.println(verticalmm);
        Serial.println(contador);
        contador = 0;
        _LeftEncoderTicks = 0;
        inputString = "";
      }
    }
  }
    if (datoEntrada == "temp") {
    Timer1.start();
    Timer1.initialize(1000);          //El timer se dispara cada 1 ms
    while (1) {
      Serial.println(contador);
    }
  }

  if (datoEntrada == "rutina2") {
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(3000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(3000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    //
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(1000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(1000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    //
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(3000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(3000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    //
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(1000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(1000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";
  }

  if (datoEntrada == "rutina1") {
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(1000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(1000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    //
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(1000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(1000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    //
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(1000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    delay(1000);
    //
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";
  }
  if (datoEntrada == "prueba3") {
    while (prueba) {
      if (inputString == "prueba3") {
        digitalWrite(Bomba, HIGH); //initialize in low state
        digitalWrite(Presion_menos, HIGH);
        inputString = "";
      }
      if (ClearLCDLeft || ClearLCDRight)
      {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print(_LeftEncoderTicks);
        lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
        ClearLCDLeft = 0;
        ClearLCDRight = 0;
      }

      /* if(_LeftEncoderTicks == -100){
        prueba = LOW;
        }*/
    }
    prueba = HIGH;
    digitalWrite(Presion_menos, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";

  }
  if (datoEntrada == "cero") {
    PWMwidthHigh = 500;
    PWMwidthLow = 500;
    Serial.println(inputString);
    Serial.print ("\n\r");
    Serial.print (PWMwidthHigh);
    Serial.print ("\n\r");
    Serial.print (PWMwidthLow);
    Serial.print ("\n\r");
    inputString = "";
  }
  if (datoEntrada == "test") {
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_mas, HIGH);
    delay(5000);
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";
  }

  if (datoEntrada == "up") {
    digitalWrite(Bomba, HIGH); //initialize in low state
    digitalWrite(Presion_menos, HIGH);
    delay(5000);
    digitalWrite(Presion_menos, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";
  }
  if (datoEntrada == "prueba1") {
    while (prueba) {
      digitalWrite(Bomba, HIGH); //initialize in low state
      digitalWrite(Presion_mas, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(_LeftEncoderTicks);
      lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
      inputString = "";
      if (_LeftEncoderTicks == -50) {
        prueba = LOW;
      }
    }
    prueba = HIGH;
    digitalWrite(Presion_mas, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";
  }

  
  if (datoEntrada == "prueba2") {
    while (prueba) {
      digitalWrite(Bomba, HIGH); //initialize in low state
      digitalWrite(Presion_menos, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(_LeftEncoderTicks);
      lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
      inputString = "";
      if (_LeftEncoderTicks == 0) {
        prueba = LOW;
      }
    }
    prueba = HIGH;
    digitalWrite(Presion_menos, LOW);
    digitalWrite(Bomba, LOW); //initialize in low state
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    inputString = "";
  }

}
