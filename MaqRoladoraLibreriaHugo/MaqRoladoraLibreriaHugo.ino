#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Arduino.h"
#include <digitalWriteFast.h>  // library for high performance reads and writes by jrraines
// see http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1267553811/0
// and http://code.google.com/p/digitalwritefast/

// It turns out that the regular digitalRead() calls are too slow and bring the arduino down when
// I use them in the interrupt routines while the motor runs at full speed creating more than
// 40000 encoder ticks per second per motor.

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Quadrature encoders
// Presión encoder
#define c_LeftEncoderInterrupt 0  //pin 2 arduino mega
#define c_LeftEncoderPinA 24
#define c_LeftEncoderPinB 25
#define LeftEncoderIsReversed
volatile bool _LeftEncoderBSet;
volatile long _LeftEncoderTicks = 0;
volatile float verticalmm;
volatile float paso = 0.58222;
// Avance encoder
#define c_RightEncoderInterrupt 1  //pin 3 arduino mega
#define c_RightEncoderPinA 26
#define c_RightEncoderPinB 27
volatile bool _RightEncoderBSet;
volatile long _RightEncoderTicks = 0;
// Funciones del sistema
// Pin retroceso
#define Retroceso 28
//Pin avance
#define Avance 29
// Presion +
#define Presion_mas 30
volatile int PWMwidthHigh;
volatile int PWMwidthLow;
// Presion -
#define Presion_menos 31
#define Bomba 40
// Variable de final de carrera pistón hidráulico y Recorrido prefil
//Cada uno de los estados de estas dos variables es definido por un switch mecánico
//El FinCarrera define el cero del encoder de presión + y presión -
#define PinFinCarreraInterrupt 4  //pin 19 arduino mega
#define PinRecorridoPerfilInterrupt 5 //pin 18 arduino mega
volatile bool FinCarrera;
volatile bool RecorridoPerfil;
volatile bool ClearLCDLeft;
volatile bool ClearLCDRight;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char inChar;
bool PWMOn;
volatile boolean estado = LOW;
volatile boolean prueba = HIGH;
volatile long contador = 0;    //contador propio del timer uno para las rutinas de rolado de las catenarias
volatile int contador1 = 0;  // contador para programar los tiempos de impresión por serial de la variable contador
volatile float vel;
//debug 
# define debug 1
//arreglos para diferentes perfiles 
void setup()
{
  #ifdef debug
  Serial.println("Bienvenido");
  #endif
  Timer1.initialize(10000);          //El timer se dispara cada 10 ms
  Timer1.attachInterrupt(ISR_Tiempo);  //activacion de la interrupcion por timer
  lcd.begin();
  Serial.begin(9600);
  Timer1.stop();
  contador = 0;
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  PWMwidthHigh = 500;
  PWMwidthLow = 500;

  //  _RightServo.attach(2);  // attaches the servo on specified pin to the servo object
  //  _LeftServo.attach(3);  // attaches the servo on specified pin to the servo object

  // Quadrature encoders
  // Left encoder
  pinMode(c_LeftEncoderPinA, INPUT);      // sets pin A as input
  digitalWrite(c_LeftEncoderPinA, LOW);  // turn on pullup resistors
  pinMode(c_LeftEncoderPinB, INPUT);      // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW);  // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterrupt, HandleLeftMotorInterruptA, RISING);

  // Right encoder
  pinMode(c_RightEncoderPinA, INPUT);      // sets pin A as input
  digitalWrite(c_RightEncoderPinA, LOW);  // turn on pullup resistors
  pinMode(c_RightEncoderPinB, INPUT);      // sets pin B as input
  digitalWrite(c_RightEncoderPinB, LOW);  // turn on pullup resistors
  attachInterrupt(c_RightEncoderInterrupt, HandleRightMotorInterruptA, RISING);
  // Funciones de interrupción de final de carrera y recorrido del perfil
  attachInterrupt(PinFinCarreraInterrupt, ISRFinCarreraInterrupt, RISING);
  attachInterrupt(PinRecorridoPerfilInterrupt, ISRRecorridoPerfilInterrupt, RISING);
  // Pin retroceso
  pinMode(Retroceso, OUTPUT);   //sets pin Retroceso as output
  digitalWrite(Retroceso, LOW);  //initilize in low state
  // Pin avance
  pinMode(Avance, OUTPUT); //sets pin Avance as output
  digitalWrite(Avance, LOW); //initilize in low state
  // Pin presion +
  pinMode(Presion_mas, OUTPUT);  //sets presion_mas as output
  digitalWrite(Presion_mas, LOW); //initilize in low state
  // Pin presion -
  pinMode(Presion_menos, OUTPUT);  //sets presion_menos as output
  digitalWrite(Presion_menos, LOW); //initilize in low state
  pinMode(Bomba, OUTPUT); //sets Bomba as output
  digitalWrite(Bomba, LOW); //initialize in low state
  ClearLCDLeft = 0;
  ClearLCDRight = 0;
  PWMOn = 0;
  lcd.setCursor(0, 0); lcd.print("Programa Roladora");
  lcd.setCursor(0, 1); lcd.print("Bienvenido");
  delay(2000);
  lcd.clear();
  verticalmm = _LeftEncoderTicks * paso;
  lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
  lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
  //lcd.setCursor(0, 0); lcd.print("hugo perra");
  /*
    digitalWrite(Bomba, LOW); //initialize in low state
    digitalWrite(Presion_mas,LOW);
    digitalWrite(Presion_menos, LOW);
    digitalWrite(Retroceso, LOW);
  */
}

void loop()
{ 

  comandosSerial(inputString);
  //Esta rutina tiene como objetivo imponer ciclos de encendido de apagado de la valvula y la bomba durante dos segundos (1 segundo prendida y uno apagado), mostrarlos desplazamientos
  //después de cada ciclo
  //La rutina2 tiene como objetivo alternar ciclos de 3 y 1 segundos de encendido y apagado del pin de presión más, de igual manera, al final de cada ciclo se visualiza la distancia recorrida
  // Condición de búsqueda del cero en el movimiento vertical
  if (FinCarrera == 0)
  {
    // digitalWrite(Bomba, HIGH); //initialize in low state
    //digitalWrite(Presion_menos, HIGH);
  }
  if (ClearLCDLeft || ClearLCDRight)
  {
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0); lcd.print(verticalmm); lcd.print("mm");
    lcd.setCursor(0, 1); lcd.print(_RightEncoderTicks);
    ClearLCDLeft = 0;
    ClearLCDRight = 0;
  }
  /*

     La siguiente es la condicional que permite iniciar el proceso de rolado de la catenaria. Este es controlado por medio del TMR1, consiste en un programa que considera desplazamientos
     en funcion de la velocidad de avance de los perfiles y el tiempo transcurrido, todo esto teniendo en cuenta la longitud de los arcos , los radios de curvatura en los puntos de interés
     y las distancias de deflexion de los perfiles para alcanzar dicho radio de curvarura

     La catenaria uno obedece a la curva descrita en la ecuación f(x)=-cosh(x)
  */
  if (inputString == "catenaria1") {
    Timer1.start();
    Timer1.initialize(1000);          //El timer se dispara cada 1 ms
    while (contador != 0 ) {
      /*
          if(contador == 1){
          Serial.print("1 OK");
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
          Serial.print("1 fin");
          digitalWrite(Bomba, LOW); //initialize in low state
          digitalWrite(Presion_mas, LOW);
          }
      */
      if (contador1 == 1) { //(contador>560 && contador<580){// == 571){
        Serial.println("1 listo");
        while (_LeftEncoderTicks >= -12) {  //-6.99
          onMotorAvance();
        }
        offMotorAvance();
      }



      if (contador1 == 2) { //(contador>1090 && contador<1110){// == 1104){
        Serial.println("2 OK");
        while (_LeftEncoderTicks >= -13) { //-7.57
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 3) { //(contador>1590 && contador<1610){// == 1602){
        Serial.println("3 OK");
        while (_LeftEncoderTicks >= -15) { //-8.73
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 4) { //(contador>2060 && contador<2080){// == 2071){
        Serial.println("4 listo");
        while (_LeftEncoderTicks >= -17) { //-9.90
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 5) { //(contador>2500 && contador<2530){// == 2515){
        Serial.println("5 listo");
        while (_LeftEncoderTicks >= -18) { //-10.48
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 6) { //(contador>2930 && contador<2950){// == 2940){
        Serial.println("6 listo");
        while (_LeftEncoderTicks >= -20) { //-11.64
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 7) { //(contador>3330 && contador<3360){// == 3348){
        Serial.println("7 listo");
        while (_LeftEncoderTicks >= -21) { //-12.23
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 8) { //(contador>3730 && contador<3760){// == 3745){
        Serial.println("8 listo");
        while (_LeftEncoderTicks >= -23) {   //-13.39
          onMotorAvance();
        }
        offMotorAvance();
      }


      if (contador1 == 9) { //(contador>4120 && contador<4150){// == 4134){
        Serial.println("9 listo");
        while (_LeftEncoderTicks > -23) { //-13.97
          onMotorAvance();
        }
        offMotorAvance();
      }
      /*

         REGRESO


      */
      if (contador1 == 10) { //(contador>5280 && contador<5310){// == 5294){
        while (_LeftEncoderTicks < -23.1) {  //-13.39
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 11) { //(contador>5680 && contador<5700){// == 5691){
        while (_LeftEncoderTicks < -21.1) {   //-12.23
          onMotor();
        }
        offMotor();
      }

      if (contador1 == 12) { //(contador>6080 && contador<6110){// == 6099){
        while (_LeftEncoderTicks <= -20.1) { //-11.64
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 13) { //(contador>6510 && contador<6540){// == 6524){
        while (_LeftEncoderTicks <= -18.1) { //-10.48
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 14) { //(contador>6950 && contador<6980){// == 6968){
        while (_LeftEncoderTicks <= -17.1) { //-9.90
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 15) { //(contador>7420 && contador<7450){// == 7437){
        while (_LeftEncoderTicks <= -15.1) {  //-8.73
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 16) { //(contador>7920 && contador<7950){// == 7935){
        while (_LeftEncoderTicks <= -13.1) { //-7.57
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 17) { //(contador>8450 && contador<8480){// == 8468){
        while (_LeftEncoderTicks <= -12.1) { //-6.99
          onMotor();
        }
        offMotor();
        inputString = "";
        Timer1.stop();
        Serial.println(contador);
        contador = 0;
        contador1 = 0;
      }
      /*
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
      */
    }
  }
  /*
     CATENARIA 2 CORRESPONDIENTE A LA ECUACIÓN F(X)=-0.5*COSH(X/0.5)
  */
  if (inputString == "catenaria2") {
    Timer1.start();
    Timer1.initialize(1000);          //El timer se dispara cada 1 ms
    while (contador != 0) {
      if (contador1 == 1) {
        while (_LeftEncoderTicks >= -10) { //-5.82
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 2) {
        while (_LeftEncoderTicks >= -14) { //-8,15
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 3) {
        while (_LeftEncoderTicks >= -20) { //-11.64
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 4) {
        while (_LeftEncoderTicks >= -26) { //15.13
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 5) {
        while (_LeftEncoderTicks >= -34) { //-19,79
          onMotorAvance();
        }
        offMotorAvance();
      }

      if (contador1 == 6) {
        while (_LeftEncoderTicks >= -41) { //23,87
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 7) {
        while (_LeftEncoderTicks >= -46) { //26,78
          onMotorAvance();
        }
        offMotorAvance();
      }
      /*
         REGRESO
      */
      if (contador1 == 8) {
        while (_LeftEncoderTicks <= -46.1) {
              onMotor();
        }
        offMotor();
      }
      if (contador1 == 9) {
        while (_LeftEncoderTicks <= -41.1) {
              onMotor();
        }
        offMotor();
      }
      if (contador1 == 10) {
        while (_LeftEncoderTicks <= -34.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 11) {
        while (_LeftEncoderTicks <= -26.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 12) {
        while (_LeftEncoderTicks <= -20.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 13) {
        while (_LeftEncoderTicks <= -14.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 14) {
        while (_LeftEncoderTicks <= -10.1) {
          onMotor();
        }
        offMotor();
        inputString = "";
        Timer1.stop();
        Serial.println(contador);
        contador = 0;
        contador1 = 0;
      }
    }
  }
  /*
     LA SIGUIENTE ES LA RUTINA PARA ROLAR LAS CATENARIAS QUE SE REQUIEREN EL EL PROYECTO QUE OBEDECEN A LA FUNCIÓN F(X)=-1,85COSH(X/1,85)
  */
  if (inputString == "catenaria3") {
    Timer1.start();
    Timer1.initialize(1000);          //El timer se dispara cada 1 ms
    while (contador != 0) {
      if (contador1 == 1) {
        while (_LeftEncoderTicks >= -4) { //-5.82
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 2) {
        while (_LeftEncoderTicks >= -5) { //-8,15
          onMotorAvance();
        }
        offMotorAvance();
      }

      if (contador1 == 3) {
        while (_LeftEncoderTicks >= -6) { //-11.64
          onMotorAvance();
        }
        offMotorAvance();
      }

      if (contador1 == 4) {
        while (_LeftEncoderTicks >= -7) { //15.13
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 5) {
        while (_LeftEncoderTicks >= -8) { //-19,79
          onMotorAvance();
        }
        offMotorAvance();
      }

      if (contador1 == 6) {
        while (_LeftEncoderTicks >= -9) { //23,87
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 7) {
        while (_LeftEncoderTicks >= -10) { //26,78
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 8) {
        while (_LeftEncoderTicks >= -11) { //26,78
          onMotorAvance();
        }
        offMotorAvance();
      }
      if (contador1 == 9) {
        while (_LeftEncoderTicks >= -12) { //26,78
          onMotorAvance();
        }
        offMotorAvance();
      }
      /*
         REGRESO
      */
      if (contador1 == 10) {
        while (_LeftEncoderTicks <= -11.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 11) {
        while (_LeftEncoderTicks <= -10.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 12) {
        while (_LeftEncoderTicks <= -9.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 13) {
        while (_LeftEncoderTicks <= -8.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 14) {
        while (_LeftEncoderTicks <= -7.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 15) {
        while (_LeftEncoderTicks <= -6.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 16) {
        while (_LeftEncoderTicks <= -5.1) {
          onMotor();
        }
        offMotor();
      }
      if (contador1 == 17) {
        while (_LeftEncoderTicks <= -4.1) {
          onMotor();
        }
        offMotor();
        inputString = "";
        Timer1.stop();
        Serial.println(contador);
        contador = 0;
        contador1 = 0;
      }
    }
  }//fin Catenaria 3
}// fin loop 
// Interrupt service routines for the left motor's quadrature encoder
void HandleLeftMotorInterruptA()
{
  //     for (int i=0; i<2; i++)
  estado = digitalRead(2);
  if (estado) {
    // Test transition; since the interrupt will only fire on 'rising' we don't need to read pin A
    _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB);   // read the input pin

    // and adjust counter + if A leads B
#ifdef LeftEncoderIsReversed
    _LeftEncoderTicks -= _LeftEncoderBSet ? -1 : +1;
#else
    _LeftEncoderTicks += _LeftEncoderBSet ? -1 : +1;
#endif
    ClearLCDLeft = !ClearLCDLeft;
    if (_LeftEncoderTicks == -100) {
      prueba = LOW;
    }
  }
}
// Interrupt service routines for the right motor's quadrature encoder
void HandleRightMotorInterruptA()
{
  //  for (int i=0; i<1; i++)
  estado = digitalRead(3);
  if (estado) {
    // Test transition; since the interrupt will only fire on 'rising' we don't need to read pin A
    _RightEncoderBSet = digitalReadFast(c_RightEncoderPinB);   // read the input pin

    // and adjust counter + if A leads B
#ifdef RightEncoderIsReversed
    _RightEncoderTicks -= _RightEncoderBSet ? -1 : +1;
#else
    _RightEncoderTicks += _RightEncoderBSet ? -1 : +1;
#endif
    ClearLCDRight = !ClearLCDRight;
  }
}
//Interrupt service fot set zero of the presure piston encoder
void ISRFinCarreraInterrupt()
{

  for (int i = 0; i < 10; i++)
    estado = digitalRead(19);
  if (estado) {
    // digitalWrite(Bomba, LOW); //initialize in low state
    //digitalWrite(Presion_menos, LOW);
    FinCarrera = 1;
    _LeftEncoderTicks = 0;
    ClearLCDRight = !ClearLCDRight;
    ClearLCDLeft = !ClearLCDLeft;
  }
}

// Interrupt service for set the begining of measure total distance traveled
void ISRRecorridoPerfilInterrupt()
{

}
/*

*/
void ISR_Tiempo()               //funcion de interrupcion por timer
{
  contador++;
  if (inputString == "catenaria1") {
    if (contador > 0 && contador < 20) { //560,580
      contador1 = 1;
    }
    if (contador > 510 && contador < 530) { //1090,1110
      contador1 = 2;
    }
    if (contador > 1010 && contador < 1030) { //1590,1610
      contador1 = 3;
    }
    if (contador > 1480 && contador < 1500) { //2060,2080
      contador1 = 4;
    }
    if (contador > 1920 && contador < 1950) { //2500,2530
      contador1 = 5;
    }
    if (contador > 2350 && contador < 2370) { //2930,2950
      contador1 = 6;
    }
    if (contador > 2750 && contador < 2780) { //3330,3360
      contador1 = 7;
    }
    if (contador > 3150 && contador < 3180) { //3730,3760
      contador1 = 8;
    }
    if (contador > 3540 && contador < 3570) { //4120,4150
      contador1 = 9;
    }
    if (contador > 4700 && contador < 4730) { //5280,5310
      contador1 = 10;
    }
    if (contador > 5100 && contador < 5130) { //5680,5700
      contador1 = 11;
    }
    if (contador > 5500 && contador < 5530) { //6080,6110
      contador1 = 12;
    }
    if (contador > 5930 && contador < 5960) { //6510,6540
      contador1 = 13;
    }
    if (contador > 6370 && contador < 6400) { // 6950,6980
      contador1 = 14;
    }
    if (contador > 6840 && contador < 6870) { // 7420,7450
      contador1 = 15;
    }
    if (contador > 7340 && contador < 7370) { // 7920,7950
      contador1 = 16;
    }
    if (contador > 7870 && contador < 7900) { //8450,8480
      contador1 = 17;
    }
  }
  if (inputString == "catenaria2") {
    if (contador > 0 && contador < 30) { //3300,3330
      contador1 = 1;
    }
    if (contador > 720 && contador < 760) { //4050,4090
      contador1 = 2;
    }
    if (contador > 1370 && contador < 1400) { //4700,4730
      contador1 = 3;
    }
    if (contador > 1910 && contador < 1950) { //5240,5280
      contador1 = 4;
    }
    if (contador > 2400 && contador < 2440) { //5730,5770
      contador1 = 5;
    }
    if (contador > 2830 && contador < 2870) { //6160,6200
      contador1 = 6;
    }
    if (contador > 3230 && contador < 3270) { //6560,6600
      contador1 = 7;
    }
    if (contador > 4010 && contador < 4050) { //7340,7380
      contador1 = 8;
    }
    if (contador > 4410 && contador < 4450) { //7740,7780
      contador1 = 9;
    }
    if (contador > 4850 && contador < 4890) { //8180,8220
      contador1 = 10;
    }
    if (contador > 5330 && contador < 5370) { //8660,8700
      contador1 = 11;
    }
    if (contador > 5890 && contador < 5930) { //9220,9250
      contador1 = 12;
    }
    if (contador > 6530 && contador < 6570) { //9860,9890
      contador1 = 13;
    }
    if (contador > 7280 && contador < 7320) { //10610,10650
      contador1 = 14;
    }
  }
  if (inputString == "catenaria3") {
    if (contador > 0 && contador < 40) {
      contador1 = 1;
    }
    if (contador > 1300 && contador < 1340) {
      contador1 = 2;
    }
    if (contador > 2520 && contador < 2560) {
      contador1 = 3;
    }
    if (contador > 3630 && contador < 3670) {
      contador1 = 4;
    }
    if (contador > 4670 && contador < 4710) {
      contador1 = 5;
    }
    if (contador > 5640 && contador < 5680) {
      contador1 = 6;
    }
    if (contador > 6550 && contador < 6590) {
      contador1 = 7;
    }
    if (contador > 7410 && contador < 7450) {
      contador1 = 8;
    }
    if (contador > 8240 && contador < 8280) {
      contador1 = 9;
    }
    if (contador > 13340 && contador < 13380) {
      contador1 = 10;
    }
    if (contador > 14180 && contador < 14220) {
      contador1 = 11;
    }
    if (contador > 15070 && contador < 15110) {
      contador1 = 12;
    }
    if (contador > 16000 && contador < 16040) {
      contador1 = 13;
    }
    if (contador > 17000 && contador < 17040) {
      contador1 = 14;
    }
    if (contador > 18080 && contador < 18120) {
      contador1 = 15;
    }
    if (contador > 19250 && contador < 19290) {
      contador1 = 16;
    }
    if (contador > 21180 && contador < 21220) {
      contador1 = 17;
    }

  }
}

