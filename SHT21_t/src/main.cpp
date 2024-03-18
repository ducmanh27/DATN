#include <Arduino.h>

// put function declarations here:



#include "Wire.h"
#include "SHT2x.h"

uint32_t start;
uint32_t stop;

SHT2x sht;



void setup()
{
  pinMode(PIN_PB4, OUTPUT);
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("SHT2x_LIB_VERSION: \t");
  Serial.println(SHT2x_LIB_VERSION);

  Wire.begin();
  Serial.print("Status SHT21: ");
  sht.begin();

  uint8_t stat = sht.getStatus();
  Serial.print(stat, HEX);
  Serial.println();
}


void loop()
{
  digitalWrite(PIN_PB4, HIGH);

  start = micros();
  sht.read();
  stop = micros();

  Serial.print("\t");
  Serial.print(stop - start);
  Serial.print("\t Temperature: ");
  Serial.print(sht.getTemperature(), 1);
  Serial.print("\t Humidity:");
  Serial.println(sht.getHumidity());
  delay(1000);
  digitalWrite(PIN_PB4, LOW);
  delay(1000);
}
