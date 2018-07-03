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
// Quadrature encoders
// Presión encoder
#define c_LeftEncoderInterrupt 1 // pin 3 arduino mega
#define c_LeftEncoderPinB 4
#define LeftEncoderIsReversed
volatile bool _LeftEncoderBSet;
volatile long _LeftEncoderTicks = 0;
volatile long TicksComparacion = 0;
volatile float verticalmm;
volatile float paso = 0.6550;
// Funciones del sistema
// Pin retroceso
// Presion +
volatile int PWMwidthHigh;
volatile int PWMwidthLow;
// Presion -
// pines comunicacion y bombas

#define Avance 39
#define Presion_menos 40
#define Retroceso 41
#define Bomba 42
#define Presion_mas 45
#define inhabilitar 44



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
#define PWM 0
// # define debugtime 0
// arreglos para diferentes perfiles
// Variables para tiempos
#define TiempoEncoder8cm 200 // 44 milisegundos
volatile unsigned int i = 0;
unsigned int sizeArray = 0;
// Arreglos tiempo acumulados en ms
/*
Longitud de arco=3 a=2
tiempos
{   0,  450, 1320, 4510, 5670, 8910, 9790}
posicion
{   0,  655, 1310, 1965, 1310,  655,    0}
Longitud de arco=3 a=1,85
tiempos
{  0,  900, 1750, 4520, 5290, 8100, 8970}
possicion
{   0,  655, 1310, 1965, 1310,  655,    0}
Longitud de arco=3 a=1.7
tiempos
{  0,  470, 1360, 2210, 4580, 5350, 7750, 8620, 9540}
posicion
{ 0,  655, 1310, 1965, 2620, 1965, 1310,  655,    0}
Longitud de arco=3 a=2.1
tiempos
{   0,  440, 1720, 4490, 5640, 8440, 9740}
posicion
{ 0,  655, 1310, 1965, 1310,  655,    0}
*/
// a=3
// Luz = 8 metros
// int tiemposAcumulados[] ={   0,  5000, 10000, 20000, 30000, 40000, 50000};
//unsigned int tiemposAcumulados[] ={  0, 2283, 4566, 8741, 11757, 14396, 17292, 23479, 26378, 29022, 32045, 36231, 38514};
unsigned int tiemposAcumulados[] ={    0,  4950,  9070, 12090, 14780, 17650, 23480, 26380, 29120, 32210, 36430};
//int Radios[] ={655, 1310, 1965, 2620, 3275, 3930, 4585, 3930, 3275, 2620, 1965, 1310, 655};
int Radios[] ={ 0,  655, 1310, 1965, 2620, 3275, 2620, 1965, 1310,  655,    0};
volatile unsigned int contadorRutinatiempos = 0;
volatile bool banderaConteo = 0;
unsigned int maxValueArray = 0;
bool cambioRadio = 0;
bool comenzar = 0;
volatile bool comenzarEncoder = 0;
volatile bool comenzarTiempo = 0;
int diferenciasRadio = 0;
volatile int TicksExtra = 0;
volatile bool PWMdebug = 1;
volatile unsigned long dif = 0;
volatile unsigned long tiempoinicial = 0;
volatile bool noesruido = 1;
unsigned int contadorPWM = 0;
unsigned int dutty = 500;
unsigned int contadorMotorAvance=0;
unsigned int tiempoStopMotorAvance=0;
#define EncoderFlat 
#define StartButtonPin 0 //En este caso interrupcion 0 asigando a pin 2 
bool StartEncoderRoutine=0;
bool StartTimeRoutine=0;
bool FlatRutinaenCurso=1;

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
  i = 0;//volvems i a cero para poder utilizarlo mas adelante 
  tiempoStopMotorAvance=maxValueArray+2000; //dejamos encendido dos segundos mas el motor 
  Serial.println(maxValueArray);  Timer1.initialize(1000); // El timer se dispara cada 1 ms
  Timer1.attachInterrupt(ISR_Tiempo); // activacion de la interrupcion por timer 
  contador = 0;
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  PWMwidthHigh = 500;
  PWMwidthLow = 500;

//Interrupcion de encoder
  pinMode(c_LeftEncoderPinB, INPUT); // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW); // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterrupt, HandleLeftMotorInterruptA, RISING);
//interrupcion boton start 
  attachInterrupt(StartButtonPin, ISR_BotonStart, FALLING);
//Pines motor rodillos
  pinMode(Retroceso, OUTPUT); // sets pin Retroceso as output
  digitalWrite(Retroceso, LOW); // initilize in low state
  pinMode(Avance, OUTPUT); // sets pin Avance as output
  digitalWrite(Avance, LOW); // initilize in low state
//Pin inhabiitar sistema manual 
  pinMode(inhabilitar, OUTPUT); // sets pin Avance as output
  digitalWrite(inhabilitar, LOW); // initilize in low state  
//Pines motor rodillo de presion 
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
void ISR_BotonStart(){
  #ifdef EncoderFlat  
    if(FlatRutinaenCurso)
  {
    StartEncoderRoutine=1;    
  }
  #endif
  #ifndef EncoderFlat  
    if(FlatRutinaenCurso)
  {
    StartTimeRoutine=1;
  }
  #endif
}

void loop()
{
  
  if (StartTimeRoutine and FlatRutinaenCurso)
  {
    digitalWrite(inhabilitar, HIGH); // initilize in low state
    StartTimeRoutine=0;
    FlatRutinaenCurso=0;
    Serial.println("Rutina Tiempo");
    comenzar = 1;
    comenzarTiempo = 1;
    contadorRutinatiempos = 0;
    contadorMotorAvance=0;
    contador = 0;
    verticalmm = 0;
    ClearLCDRight = 1;
    _LeftEncoderTicks = 0;
    TicksComparacion = 0;
    TicksExtra = 0;
    i = 0;    
    digitalWrite(Avance, HIGH); // initilize in low state
  }
  if (StartEncoderRoutine and FlatRutinaenCurso)
  { 
    digitalWrite(inhabilitar, HIGH); // initilize in low state
    FlatRutinaenCurso=0;
    StartEncoderRoutine=0;
    Serial.println("RutinaEncoder");
    comenzar = 1;
    comenzarEncoder = 1;
    i = 0;
    contadorRutinatiempos = 0;
    contadorMotorAvance=0;
    contador = 0;
    TicksExtra = 0;
    verticalmm = 0;
    ClearLCDRight = 1;
    _LeftEncoderTicks = 0;
    TicksComparacion = 0;
    TicksExtra = 0;    
    digitalWrite(Avance, HIGH); // initilize in low state
  }
  if (ClearLCDLeft || ClearLCDRight)
  {
    lcd.clear();
    verticalmm = TicksComparacion * paso;
    lcd.setCursor(0, 0);
    lcd.print(verticalmm);
    lcd.print("mm");
    ClearLCDLeft = 0;
    ClearLCDRight = 0;
  }
  RutinaTiempo(comenzar);
  if (comenzarEncoder)
  {
    bajarTickEncoder(cambioRadio);
  }
  if (comenzarTiempo)
  {
    bajarTickconTiempo(cambioRadio);
  }
  if (contadorRutinatiempos > maxValueArray)
  {    
    contadorRutinatiempos = 0;
    comenzar = 0;
    comenzarEncoder = 0;
    comenzarTiempo = 0;    
    Serial.println("Terminamos");
    Serial.println(TicksExtra);
    apagadoMotorBomba();
    // inputString="rutinatiempo";
    // inputString= "rutinaencoder";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("acabo");
    delay(2000);
    ClearLCDRight = 1;

  }
  if(contadorMotorAvance>tiempoStopMotorAvance){
    digitalWrite(Avance, LOW); // Stop motors forwad 
    digitalWrite(inhabilitar, LOW); // initilize in low state
    FlatRutinaenCurso=1;
    contadorMotorAvance=0;
  }
  if (comenzarEncoder == 0 and comenzarTiempo == 0)
  {
    apagadoMotorBomba();
  }
  /*
  if(contadorPWM<dutty){
  digitalWrite(Bombadebug, HIGH);
  else{
  digitalWrite(Bombadebug, LOW);
  }
  if(contadorPWM>(dutty*2)){
  contadorPWM=0;
  }
  */
  wdt_reset();
}

// fin loop
void ISR_Tiempo() // funcion de interrupcion por timer
{
  contadorRutinatiempos++;
  contador++;
  contadorMotorAvance++;
  // contadorPWM++;
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

void bajarTickconTiempo(bool decicionsubir)
{
  if (banderaConteo)
  {
    if (decicionsubir)
    {
      encendidoMotorBombaSubir();
      if (contador > TiempoEncoder8cm)
      {
        Serial.println("Subir");
        banderaConteo = 0;
        apagadoMotorBomba();
      }
    }
    else
    {
      encendidoMotorbajar();
      if (contador > TiempoEncoder8cm)
      {
        Serial.println("Bajar");
        banderaConteo = 0;
        apagadoMotorBomba();
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
  if (posArreglo < (sizeArray - 1))
  {
    Serial.print("diferenciasRadio=");
    Serial.print(Radios[posArreglo + 1]);
    Serial.print("-");
    Serial.print(Radios[posArreglo]);
    Serial.print("=");
    diferenciasRadio = Radios[posArreglo + 1] - Radios[posArreglo];
    Serial.println(diferenciasRadio);
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
    // Serial.println("desbordado");
    // comenzarEncoder=0;
    cambioRadio = 1;
    // comenzarTiempo=0;
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
  _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB); // read the input pin
  dif = millis() - tiempoinicial;
  tiempoinicial = millis();
  if (dif > 2)
  {
    noesruido = 1;
  }
  else
  {
    noesruido = 0;
  }

  if (noesruido)
  {
    // Test transition; since the interrupt will only fire on 'rising' we don't need to read pin A
    // _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB); // read the input pin
    // and adjust counter + if A leads B

#ifdef LeftEncoderIsReversed
    _LeftEncoderTicks -= _LeftEncoderBSet? -1:+ 1;
    TicksComparacion -= _LeftEncoderBSet? -1:+ 1;
#else
    _LeftEncoderTicks += _LeftEncoderBSet? -1:+ 1;
    TicksComparacion += _LeftEncoderBSet? -1:+ 1;
#endif
    ClearLCDLeft = !ClearLCDLeft;
  }
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



