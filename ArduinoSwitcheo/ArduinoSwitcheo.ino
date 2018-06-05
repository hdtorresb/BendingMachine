#define pinSubirBajar 2
#define pinStart 3
#define SalidaBomba 4
#define SalidaSubir 5
#define SalidaBajar 6
void setup()
{
	Serial.begin(9600);
	pinMode(pinSubirBajar, INPUT);
	pinMode(pinStart, INPUT);
	pinMode(SalidaBomba, OUTPUT);
	pinMode(SalidaBajar, OUTPUT);
	pinMode(SalidaSubir, OUTPUT);
	digitalWrite(SalidaBomba, LOW);
	digitalWrite(SalidaBajar, LOW);
	digitalWrite(SalidaSubir, LOW);
}

void loop()
{
	if (digitalRead(pinStart))
	{
		if (digitalRead(pinSubirBajar))
		{
			subir();
		}
		else
		{
			bajar();
		}
	}
	else
	{
		Apagar();
	}
}

void subir()
{
	digitalWrite(SalidaBomba, HIGH);
	digitalWrite(SalidaBajar, LOW);
	digitalWrite(SalidaSubir, HIGH);
}

void bajar()
{
	digitalWrite(SalidaBomba, HIGH);
	digitalWrite(SalidaBajar, HIGH);
	digitalWrite(SalidaSubir, LOW);
}

void Apagar()
{
	digitalWrite(SalidaBomba, LOW);
	digitalWrite(SalidaBajar, LOW);
	digitalWrite(SalidaSubir, LOW);
}
