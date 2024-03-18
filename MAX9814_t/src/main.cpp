#include <Arduino.h>
#define MAX9814_OUT_PIN PIN_PF0
float readMAX9814()
{
    unsigned long StartMillis = millis();

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // read sensor in 50ms, take (signalMax - signMin) to caculate sound
    while (millis() - StartMillis < 50)
    {
        unsigned int sample = analogRead(MAX9814_OUT_PIN);

        if (sample < 1024)
        {
            if (sample > signalMax)
            {
                signalMax = sample;
            }
            else if (sample < signalMin)
            {
                signalMin = sample;
            }
        }
    }
    double volts = (signalMax - signalMin) * 3.3 / 1024;

    // with the microphone sensivity is -44db, so VRms/PA is 0.006309
    return 20.0 * log10(volts / 0.006309);
    // return volts;
}
void setup() {
  Serial.begin(115200);

  pinMode(PIN_PB4, OUTPUT);
}

void loop() {
  float value = readMAX9814();
  if (value)
  {
    Serial.println(readMAX9814());
  }
  digitalWrite(PIN_PB4, HIGH);
  delay(500);
  digitalWrite(PIN_PB4, LOW);
  delay(500);  
}
