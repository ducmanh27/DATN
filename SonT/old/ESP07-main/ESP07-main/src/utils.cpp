#include "utils.h"

static SoftwareSerial logger(15, 13);

void utilsInit()
{
  pinMode(LEDPIN, OUTPUT); // led for debug
  logger.begin(115200);
}

/**
 * @brief blink led to debug
 *
 */
void ledDebug()
{
  digitalWrite(LEDPIN, LOW);
  delay(100);
  digitalWrite(LEDPIN, HIGH);
  delay(100);
}

void log(const char *msg)
{
  if(DEBUG_ENABLE){
    logger.println(msg);
  }
  
}