#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Arduino.h"
#include <digitalWriteFast.h> // library for high performance reads and writes by jrraines
#include <math.h>
#include <avr/wdt.h>
// and http://code.google.com/p/digitalwritefast/
// It turns out that the regular digitalRead() calls are too slow and bring the arduino down when
// I use them in the interrupt routines while the motor runs at full speed creating more than
// 40000 encoder ticks per second per motor.
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Presión encoder
#define c_LeftEncoderInterrupt 1 // pin 3 arduino mega
#define c_LeftEncoderPinB 4
#define LeftEncoderIsReversed
volatile bool _LeftEncoderBSet;
volatile long _LeftEncoderTicks = 0;
volatile long TicksComparacion = 0;
volatile float verticalmm;
volatile float paso = 0.31416;

// pines comunicacion y bombas
#define Avance 39
#define Presion_menos 40
#define Retroceso 41
#define Bomba 42
#define Presion_mas 45

//variables comunicacion Serial y LCD
volatile bool ClearLCDLeft;
String inputString = ""; // a string to hold incoming data
boolean stringComplete = false; // whether the string is complete
char inChar;
volatile bool LCDenrutina=0;

// Contadores
volatile long contador = 0; // contador propio del timer uno para las rutinas de rolado de las catenarias
volatile unsigned int contadorRutinatiempos = 0;
volatile unsigned int contadorMotorAvance = 0;
unsigned int tiempoStopMotorAvance = 0;
// debug
#define debug 0
#define PWM 0
#define TiempoEncoder8cm 200 // 44 milisegundos
// Variables arreglos
volatile unsigned int i = 0;
unsigned int sizeArray = 0;
//unsigned int tiemposAcumulados[] ={ 0,   772,  1544,  2316,  3088,  5588,  7565,  9227, 10677, 12044, 13292, 14526, 15792, 17292, 23479, 24981, 26249, 27485, 28736, 30104, 31558, 33224, 35206, 37713, 38485, 40029, 42345};
//int Radios[] ={314,  628,  942, 1257, 1571, 1885, 2199, 2513, 2827, 3142, 3456, 3770, 4084, 4398, 4084, 3770, 3456, 3142, 2827, 2513, 2199, 1885, 1571, 1257,  942,  628,  314};
unsigned int tiemposAcumulados[] ={ 0, 4950, 9070, 12090, 14780, 17650, 23480, 26380, 29120, 32210, 36430};
int Radios[] ={  0, 655, 1310, 1965, 2620, 3275, 2620, 1965, 1310, 655, 0};

unsigned int maxValueArray = 0;
// Variables Rutinas
volatile bool banderaConteo = 0;
bool cambioRadio = 0;
bool comenzar = 0;
volatile bool comenzarEncoder = 0;
volatile bool comenzarTiempo = 0;
int diferenciasRadio = 0;
volatile int TicksExtra = 0;
volatile unsigned long dif = 0;
volatile unsigned long tiempoinicial = 0;
volatile bool noesruido = 1;

#define EncoderFlat
#define StartButtonPin 0 // En este caso interrupcion 0 asigando a pin 2
bool StartEncoderRoutine = 0;
bool StartTimeRoutine = 0;
bool FlatRutinaenCurso = 1;
volatile bool BajarTick=0;
volatile int  contadorPruebas=0;
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
  i = 0; // volvems i a cero para poder utilizarlo mas adelante
  tiempoStopMotorAvance = maxValueArray + 2000; // dejamos encendido dos segundos mas el motor
  Timer1.initialize(1000); // El timer se dispara cada 1 ms
  Timer1.attachInterrupt(ISR_Tiempo); // activacion de la interrupcion por timer
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  // Interrupcion de encoder
  pinMode(c_LeftEncoderPinB, INPUT); // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW); // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterrupt, HandleLeftMotorInterruptA, RISING);
  // interrupcion boton start
  attachInterrupt(StartButtonPin, ISR_BotonStart, FALLING);
  // Pines motor rodillos
  pinMode(Retroceso, OUTPUT); // sets pin Retroceso as output
  digitalWrite(Retroceso, LOW); // initilize in low state
  pinMode(Avance, OUTPUT); // sets pin Avance as output
  digitalWrite(Avance, LOW); // initilize in low state

  // Pines motor rodillo de presion
  pinMode(Bomba, OUTPUT); // sets pinSubirBajar as output
  digitalWrite(Bomba, LOW); // initilize in low state
  pinMode(Presion_mas, OUTPUT); // sets presion_mas as output
  digitalWrite(Presion_mas, LOW); // initilize in low state
  pinMode(Presion_menos, OUTPUT); // sets pinSubirBajar as output
  digitalWrite(Presion_menos, LOW); // initilize in low state
  mensajeBienvenida();
  wdt_disable(); // deshabilito el perro guardian
  wdt_enable(WDTO_8S);
  // inputString="rutinatiempo";
  // inputString= "rutinaencoder";
}


void ISR_BotonStart()
{
  if (FlatRutinaenCurso)
  {
    //StartEncoderRoutine = 1;
    LCDenrutina=1;
    BajarTick=1;
    contadorPruebas=0;
    _LeftEncoderTicks = 0;
    TicksComparacion = 0;

  }
}

void loop()
{
  if(BajarTick){
    encendidoMotorbajar();
        if(abs(_LeftEncoderTicks) > 0)
  {
    apagadoMotorBomba();
    BajarTick=0;
    /*
    ClearLCDLeft = 1;
    Serial.print("Ticks Totales= ");
    Serial.println(_LeftEncoderTicks);
    Serial.print("Tiempo Transcurrido= ");
    Serial.println(contadorPruebas);
    */
  }
}

  
  if (StartEncoderRoutine and FlatRutinaenCurso)
  {     
    FlatRutinaenCurso = 0;
    StartEncoderRoutine = 0;
    Serial.println("RutinaEncoder");
    comenzar = 1;
    comenzarEncoder = 1;
    i = 0;
    contadorRutinatiempos = 0;
    contadorMotorAvance = 0;
    contador = 0;
    TicksExtra = 0;
    verticalmm = 0;
    ClearLCDLeft = 1;
    _LeftEncoderTicks = 0;
    TicksComparacion = 0;
    TicksExtra = 0;
    digitalWrite(Avance, HIGH); // Start moving motor on horizontal axis
  }
  clearLCD();
  if(comenzarEncoder){
 CambioSubidaBajada(i);
}
  RutinaTiempo(comenzar);
  if (comenzarEncoder)
  {
    bajarTickEncoder(cambioRadio);
  }
  if (contadorRutinatiempos > maxValueArray)
  {
    contadorRutinatiempos = 0;
    comenzar = 0;
    comenzarEncoder = 0;    
    Serial.println("Terminamos");
    Serial.println(TicksExtra);
    apagadoMotorBomba();
    ClearLCDLeft = 1;
  }
  if (contadorMotorAvance > tiempoStopMotorAvance)
  {
    digitalWrite(Avance, LOW); // Stop motors forwad    
    FlatRutinaenCurso = 1;
    contadorMotorAvance = 0;
    LCDenrutina=0;
  }
  wdt_reset();
}

// fin loop
void ISR_Tiempo() // funcion de interrupcion por timer
{
  contadorRutinatiempos++;
  contador++;
  contadorMotorAvance++;
}

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

    if (contadorRutinatiempos > tiemposAcumulados[i] and i <= sizeArray)
    {
      CambioSubidaBajada(i);
      Serial.print("i= ");
      Serial.println(i);
      Serial.print("TiempoActual= ");
      Serial.println(tiemposAcumulados[i]);
      i++;
      banderaConteo = 1;
      contador = 0;
      if (comenzarEncoder)
      {
        _LeftEncoderTicks = 0;
      }
    }
  }
}

void CambioSubidaBajada(int posArreglo)
{
  if (posArreglo < (sizeArray - 1))
  {
    diferenciasRadio = Radios[posArreglo + 1] - Radios[posArreglo];

#ifdef debugtime
    Serial.print("diferenciasRadio=");
    Serial.print(Radios[posArreglo + 1]);
    Serial.print("-");
    Serial.print(Radios[posArreglo]);
    Serial.print("=");
    Serial.println(diferenciasRadio);
#endif

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
      encendidoMotorBombaSubir();
      if (abs(_LeftEncoderTicks) > 0)
      {
        _LeftEncoderTicks = 0;
        Serial.print("Subir");
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
      encendidoMotorbajar();
      if (abs(_LeftEncoderTicks) > 0)
      {
        Serial.print("Bajar");
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
  // Leemos el estd de otro pin para saber si estamos avanzando o retrocediendo
  _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB); // read the input pin
#ifdef LeftEncoderIsReversed
    _LeftEncoderTicks -= _LeftEncoderBSet? -1:+ 1;
    TicksComparacion -= _LeftEncoderBSet? -1:+ 1;
#else
    _LeftEncoderTicks += _LeftEncoderBSet? -1:+ 1;
    TicksComparacion += _LeftEncoderBSet? -1:+ 1;
#endif
ClearLCDLeft = !ClearLCDLeft;  
}

void encendidoMotorBombaSubir()
{
  digitalWrite(Bomba, HIGH);
  digitalWrite(Presion_mas, HIGH);
}

void encendidoMotorbajar()
{
  digitalWrite(Bomba, HIGH);
  digitalWrite(Presion_menos, HIGH);
}

void apagadoMotorBomba()
{
  digitalWrite(Bomba, LOW);
  digitalWrite(Presion_menos, LOW);
  digitalWrite(Presion_mas, LOW);
}
