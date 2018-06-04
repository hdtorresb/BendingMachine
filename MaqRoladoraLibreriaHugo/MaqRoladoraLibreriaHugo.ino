#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Arduino.h"
#include <digitalWriteFast.h> // library for high performance reads and writes by jrraines
#include <math.h>
// see http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1267553811/0
// and http://code.google.com/p/digitalwritefast/
// It turns out that the regular digitalRead() calls are too slow and bring the arduino down when
// I use them in the interrupt routines while the motor runs at full speed creating more than
// 40000 encoder ticks per second per motor.
LiquidCrystal_I2C lcd(0x27, 20, 4);
// Quadrature encoders
// Presión encoder
#define c_LeftEncoderInterrupt 0 // pin 2 arduino mega
#define c_LeftEncoderPinA 24
#define c_LeftEncoderPinB 25
#define LeftEncoderIsReversed
volatile bool _LeftEncoderBSet;
volatile long _LeftEncoderTicks = 0;
volatile float verticalmm;
volatile float paso = 0.58222;
// Avance encoder
#define c_RightEncoderInterrupt 1 // pin 3 arduino mega
#define c_RightEncoderPinA 26
#define c_RightEncoderPinB 27
volatile bool _RightEncoderBSet;
volatile long _RightEncoderTicks = 0;
// Funciones del sistema
// Pin retroceso
#define Retroceso 28
// Pin avance
#define Avance 29
// Presion +
#define Presion_mas 30
volatile int PWMwidthHigh;
volatile int PWMwidthLow;
// Presion -
#define Presion_menos 31
#define Bomba 40
// Variable de final de carrera pistón hidráulico y Recorrido prefil
// Cada uno de los estados de estas dos variables es definido por un switch mecánico
// El FinCarrera define el cero del encoder de presión + y presión -
#define PinFinCarreraInterrupt 4 // pin 19 arduino mega
#define PinRecorridoPerfilInterrupt 5 // pin 18 arduino mega
volatile bool FinCarrera;
volatile bool RecorridoPerfil;
volatile bool ClearLCDLeft;
volatile bool ClearLCDRight;
String inputString = ""; // a string to hold incoming data
boolean stringComplete = false; // whether the string is complete
char inChar;
bool PWMOn;
volatile boolean estado = LOW;
volatile boolean prueba = HIGH;
volatile long contador = 0; // contador propio del timer uno para las rutinas de rolado de las catenarias
volatile int contador1 = 0; // contador para programar los tiempos de impresión por serial de la variable contador
volatile float vel;
// debug
#define debug 0
// # define debugtime 0
// arreglos para diferentes perfiles
// Variables para tiempos
#define TiempoEncoder8cm 44 // 44 milisegundos
volatile unsigned int i = 0;
unsigned int sizeArray = 0;
// Arreglos tiempo acumulados en ms
int tiemposAcumulados[] =
{
  0, 2540, 4768, 6105, 7339, 8483, 9547, 10544, 11019, 11932,
  12800, 13631, 14436, 15994, 17150, 18722, 19539, 20387, 21276, 22214,
  22705, 23734, 24837, 26024, 27308, 28702, 31028
};

volatile unsigned int contadorRutinatiempos = 0;
volatile bool banderaConteo = 0;
unsigned int maxValueArray = 0;
bool cambioRadio = 0;
bool comenzar = 0;
bool comenzarEncoder = 0;
int Radios[] =
{
  1310, 1750, 2180, 2620, 3060, 3490, 3930, 4370, 4800, 5240, 5680, 6110,
  6550, 6990, 6550, 6110, 5680, 5240, 4800, 4370, 3930, 3490, 3060, 2620,
  2180, 1750, 1310,
};

int diferenciasRadio = 0;
volatile int TicksExtra = 0;
void setup()
{
  Serial.begin(9600);

#ifdef debug
  Serial.println("Bienvenido");
  Serial.println(sizeof(tiemposAcumulados) / (sizeof(tiemposAcumulados[0])));
#endif

  // encontramos tamaño del arreglo y el mayor valor
  sizeArray = sizeof(tiemposAcumulados) / (sizeof(tiemposAcumulados[0]));
  maxValueArray = tiemposAcumulados[0];
  for (i = 0; i < sizeArray; i++)
  {
    if (tiemposAcumulados[i] > maxValueArray)
    {
      maxValueArray = tiemposAcumulados[i];
    }
  }
  Serial.println(maxValueArray);
  i = 0;
  Timer1.initialize(1000); // El timer se dispara cada 1 ms
  Timer1.attachInterrupt(ISR_Tiempo); // activacion de la interrupcion por timer
  lcd.begin();
  // Timer1.stop();
  contador = 0;
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  PWMwidthHigh = 500;
  PWMwidthLow = 500;
  // _RightServo.attach(2);  // attaches the servo on specified pin to the servo object
  // _LeftServo.attach(3);  // attaches the servo on specified pin to the servo object
  // Quadrature encoders
  // Left encoder
  pinMode(c_LeftEncoderPinA, INPUT); // sets pin A as input
  digitalWrite(c_LeftEncoderPinA, LOW); // turn on pullup resistors
  pinMode(c_LeftEncoderPinB, INPUT); // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW); // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterrupt, HandleLeftMotorInterruptA, RISING);
  // Right encoder
  pinMode(c_RightEncoderPinA, INPUT); // sets pin A as input
  digitalWrite(c_RightEncoderPinA, LOW); // turn on pullup resistors
  pinMode(c_RightEncoderPinB, INPUT); // sets pin B as input
  digitalWrite(c_RightEncoderPinB, LOW); // turn on pullup resistors
  attachInterrupt(c_RightEncoderInterrupt, HandleRightMotorInterruptA, RISING);
  // Pin retroceso
  pinMode(Retroceso, OUTPUT); // sets pin Retroceso as output
  digitalWrite(Retroceso, LOW); // initilize in low state
  // Pin avance
  pinMode(Avance, OUTPUT); // sets pin Avance as output
  digitalWrite(Avance, LOW); // initilize in low state
  // Pin presion +
  pinMode(Presion_mas, OUTPUT); // sets presion_mas as output
  digitalWrite(Presion_mas, LOW); // initilize in low state
  // Pin presion -
  pinMode(Presion_menos, OUTPUT); // sets presion_menos as output
  digitalWrite(Presion_menos, LOW); // initilize in low state
  pinMode(Bomba, OUTPUT); // sets Bomba as output
  digitalWrite(Bomba, LOW); // initialize in low state
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
  lcd.setCursor(0, 1);
  lcd.print(_RightEncoderTicks);
  // lcd.setCursor(0, 0); lcd.print("hugo perra");
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
  // Esta rutina tiene como objetivo imponer ciclos de encendido de apagado de la valvula y la bomba durante dos segundos (1 segundo prendida y uno apagado), mostrarlos desplazamientos
  // después de cada ciclo
  // La rutina2 tiene como objetivo alternar ciclos de 3 y 1 segundos de encendido y apagado del pin de presión más, de igual manera, al final de cada ciclo se visualiza la distancia recorrida
  // Condición de búsqueda del cero en el movimiento vertical
  if (FinCarrera == 0)
  {
    // digitalWrite(Bomba, HIGH); //initialize in low state
    // digitalWrite(Presion_menos, HIGH);
  }
  if (ClearLCDLeft || ClearLCDRight)
  {
    lcd.clear();
    verticalmm = _LeftEncoderTicks * paso;
    lcd.setCursor(0, 0);
    lcd.print(verticalmm);
    lcd.print("mm");
    lcd.setCursor(0, 1);
    lcd.print(_RightEncoderTicks);
    ClearLCDLeft = 0;
    ClearLCDRight = 0;
  }
  /*
  //Serial.println(banderaConteo);
  if(inputString=="rutinatiempo"){
  Serial.println("Comenzo Rutina");
  comenzar=1;
  contadorRutinatiempos=0;
  contador=0;
  i=0;
  inputString="";
  }
  CambioSubidaBajada(i);
  RutinaTiempo(comenzar);
  bajarTickconTiempo(cambioRadio);
  if (contadorRutinatiempos>maxValueArray){
  contadorRutinatiempos=0;
  comenzar=0;
  Serial.println("Terminamos");
  }
  */
  if (inputString == "rutinaEncoder")
  {
    comenzarEncoder = 1;
    i = 0;
    contadorRutinatiempos = 0;
    contador = 0;
    TicksExtra=0;
    inputString = "";
  }
  if (contadorRutinatiempos > maxValueArray)
  {
    contadorRutinatiempos = 0;
    comenzarEncoder = 0;
    Serial.println("Terminamos");
    Serial.println(TicksExtra);
  }
  CambioSubidaBajada(i);
}

// fin loop
void ISR_Tiempo() // funcion de interrupcion por timer
{
  contadorRutinatiempos++;
  contador++;
}

/*Arreglo a usar tiemposAcumulados[]={    0,  2540,  4768,  6105,  7339,  8483,  9547, 10544, 11019, 11932,
12800, 13631, 14436, 15994, 17150, 18722, 19539, 20387, 21276, 22214,
22705, 23734, 24837, 26024, 27308, 28702, 31028};*/
void RutinaTiempo(bool beginRutina)
{
  if (beginRutina)
  {

#ifdef debugtime
    Serial.print("Contador=");
    Serial.println(contador);
    Serial.print("contadorRutinatiempos=");
    Serial.println(contadorRutinatiempos);
    Serial.print("i=");
    Serial.println(i);
#endif

    if (contadorRutinatiempos > tiemposAcumulados[i] and i < sizeArray)
    {
      i++;
      banderaConteo = 1;
      contador = 0;
      _LeftEncoderTicks = 0;
    }
  }
}

void bajarTickconTiempo(bool decicionsubir)
{
  if (banderaConteo)
  {
    if (decicionsubir)
    {
      Serial.print("Subir");
      encendidoMotorBombaSubir();
      if (contador > TiempoEncoder8cm)
      {
        banderaConteo = 0;
      }
    }
    else
    {
      Serial.print("Bajar");
      encendidoMotorbajar();
      if (contador > TiempoEncoder8cm)
      {
        banderaConteo = 0;
      }
    }
    // fin else
  }
  // fin if bandera conteo
  else
  {
    // Serial.print("Apagar");
    apagadoMotorBomba();
  }
}

/*
int Radios[]={1310, 1750, 2180, 2620, 3060, 3490, 3930, 4370, 4800, 5240, 5680, 6110,
6550, 6990, 6550, 6110, 5680, 5240, 4800, 4370, 3930, 3490, 3060, 2620,
2180, 1750, 1310,};
*/
void CambioSubidaBajada(int posArreglo)
{
  if (posArreglo < sizeArray)
  {
    diferenciasRadio = Radios[posArreglo + 1] - Radios[posArreglo];
    if (diferenciasRadio > 0)
    {
      cambioRadio = 0;
    }
    else
      if (diferenciasRadio < 0)
      {
        cambioRadio = 1;
      }
  }
  else
  {
    cambioRadio = 1;
  }
}

void bajarTickEncoder(bool decicionsubir)
{
  if (banderaConteo)
  {
    if (decicionsubir)
    {
      Serial.print("Subir");
      encendidoMotorBombaSubir();
      if (abs(_LeftEncoderTicks) > 0)
      {
        apagadoMotorBomba(); // aseguramos que una vez se movio se apaga
        banderaConteo = 0;
        if (abs(_LeftEncoderTicks) >= 2)
        {
          TicksExtra = TicksExtra + abs(_LeftEncoderTicks);
        }
      }
    }
    else
    {
      Serial.print("Bajar");
      encendidoMotorbajar();
      if (abs(_LeftEncoderTicks) > 0)
      {
        apagadoMotorBomba(); // aseguramos que una vez se movio se apaga
        banderaConteo = 0;
        // Vamos a ver cuantas ticks extra se contaron
        if (abs(_LeftEncoderTicks) >= 2)
        {
          TicksExtra = TicksExtra + abs(_LeftEncoderTicks);
        }
      }
    }
    // fin else
  }
  // fin if bandera conteo
  else
  {
    // Serial.print("Apagar");
    apagadoMotorBomba();
  }
}

// Interrupt service routines for the left motor's quadrature encoder
void HandleLeftMotorInterruptA()
{
  // for (int i=0; i<2; i++)
  estado = digitalRead(2);
  if (estado)
  {
    // Test transition; since the interrupt will only fire on 'rising' we don't need to read pin A
    _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB); // read the input pin
    // and adjust counter + if A leads B

#ifdef LeftEncoderIsReversed
    _LeftEncoderTicks -= _LeftEncoderBSet? -1:+ 1;
#else
    _LeftEncoderTicks += _LeftEncoderBSet? -1:+ 1;
#endif

    ClearLCDLeft = !ClearLCDLeft;
    if (_LeftEncoderTicks == -100)
    {
      prueba = LOW;
    }
  }
}

// Interrupt service routines for the right motor's quadrature encoder
void HandleRightMotorInterruptA()
{
  // for (int i=0; i<1; i++)
  estado = digitalRead(3);
  if (estado)
  {
    // Test transition; since the interrupt will only fire on 'rising' we don't need to read pin A
    _RightEncoderBSet = digitalReadFast(c_RightEncoderPinB); // read the input pin
    // and adjust counter + if A leads B

#ifdef RightEncoderIsReversed
    _RightEncoderTicks -= _RightEncoderBSet? -1:+ 1;
#else
    _RightEncoderTicks += _RightEncoderBSet? -1:+ 1;
#endif

    ClearLCDRight = !ClearLCDRight;
  }
}

// Interrupt service fot set zero of the presure piston encoder
void ISRFinCarreraInterrupt()
{
  for (int i = 0; i < 10; i++)
    estado = digitalRead(19);
  if (estado)
  {
    // digitalWrite(Bomba, LOW); //initialize in low state
    // digitalWrite(Presion_menos, LOW);
    FinCarrera = 1;
    _LeftEncoderTicks = 0;
    ClearLCDRight = !ClearLCDRight;
    ClearLCDLeft = !ClearLCDLeft;
  }
}

void encendidoMotorBombaSubir()
{
  digitalWrite(Bomba, HIGH); // initialize in low state
  digitalWrite(Presion_mas, HIGH);
  digitalWrite(Avance, HIGH);
}

void encendidoMotorbajar()
{
  digitalWrite(Bomba, HIGH); // initialize in low state
  digitalWrite(Presion_menos, HIGH);
  digitalWrite(Avance, HIGH);
}

void apagadoMotorBomba()
{
  digitalWrite(Bomba, LOW); // initialize in low state
  digitalWrite(Presion_menos, LOW);
  digitalWrite(Retroceso, LOW);
  digitalWrite(Presion_mas, LOW);
}
