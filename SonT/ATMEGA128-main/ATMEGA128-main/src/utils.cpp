#include "utils.h"

void ledDebug()
{
  delay(1000);
  digitalWrite(LEDPIN, 1);
  delay(1000);
  digitalWrite(LEDPIN, 0);
}

/**
 * @brief Get the Operator object
 *
 * @param myOperator
 * @return int
 */
int getOperator(String myOperator)
{
  for (int i = 0; i < NUM_OF_OPERATOR; i++)
  {
    if (myOperator == operatorList[i])
    {
      return i;
    }
  }
  return -1;
};
