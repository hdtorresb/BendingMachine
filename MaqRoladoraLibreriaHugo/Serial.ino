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

  if(datoEntrada=="pwmon"){
    PWMdebug=1;
    inputString = "";

  }

  if(datoEntrada=="pwmoff"){
    PWMdebug=0;
    inputString = "";
  }

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

}
