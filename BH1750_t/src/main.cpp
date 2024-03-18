#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;
// put function declarations here:

void setup()
{
  pinMode(PIN_PB4, OUTPUT);
  Serial.begin(115200);
    Wire.begin();
      lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
}



void loop()
{
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
}
