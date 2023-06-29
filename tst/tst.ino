#include <OneWire.h>
#include <DallasTemperature.h>

#define dados 32
#define supply 35

int ndispositivos = 0;
float temperatura = 0.0;

OneWire oneWire(dados);
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
  ndispositivos = sensors.getDeviceCount();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Numero de sensores:");
  Serial.println(ndispositivos);
  
  sensors.requestTemperatures();

  Serial.println("Temperatura: ");
  for (int i=0; i<ndispositivos; i++){
      temperatura = sensors.getTempCByIndex(i);
      Serial.print("Sensor ");
      Serial.print(i+1);
      Serial.print(": ");
      Serial.println(temperatura);
  }
  
  delay(750);
}
