/*
 * 
 * Las sigguientes son similares a las dos rutinas anteriores con la diferencia de suspender el movimiento del perfil horizontal mientras se completa la deflexión
 * para alcanzar el radio de curvatura requerido en dicho punto, para esto se han habilitado dos pines adicionales. Hay que garantizar que sichos pines nunca estarán en estado alto
 * al mismo tiempo.
 * 
 * catenaria3 corresponde a catenaria1
 * catenaria4 corresponde a catenaria2
 * 
 * 
 */

/*
   if (inputString == "catenaria3"){
  Timer1.start();
  Timer1.initialize(1000);          //El timer se dispara cada 1 ms
  if(contador == 1){
    while(verticalmm != -5.82){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }



    if(contador == 571){
    while(verticalmm != -6.99){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }



      if(contador == 1104){
    while(verticalmm != -7.57){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 1602){
    while(verticalmm != -8.73){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 2071){
    while(verticalmm != -9.90){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 2515){
    while(verticalmm != -10.48){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 2940){
    while(verticalmm != -11.64){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 3348){
    while(verticalmm != -12.23){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 3745){
    while(verticalmm != -13.39){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }


      if(contador == 4134){
    while(verticalmm != -13.97){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_mas, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_mas, LOW);
  }





  /*
   * 
   * REGRESO
   * 
   * 
   */

/*
        if(contador == 5294){
    while(verticalmm != -13.39){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 5691){
    while(verticalmm != -12.23){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 6099){
    while(verticalmm != -11.64){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 6524){
    while(verticalmm != -10.48){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 6968){
    while(verticalmm != -9.90){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 7437){
    while(verticalmm != -8.73){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 7935){
    while(verticalmm != -7.57){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 8468){
    while(verticalmm != -6.99){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }


          if(contador == 9040){
    while(verticalmm != -5.82){
  digitalWrite(Bomba, HIGH); //initialize in low state
  digitalWrite(Presion_menos, HIGH);
if (ClearLCDLeft || ClearLCDRight){
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
}
    }
  digitalWrite(Bomba, LOW); //initialize in low state
  digitalWrite(Presion_menos, LOW);
  }

  inputString = "";
  Timer1.stop();
  contador = 0;
}
*/
