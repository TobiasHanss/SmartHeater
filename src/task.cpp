#include <Arduino.h>
#include "task.h"


void printStack(void)
{
  char *SpStart = NULL;
  char *StackPtrAtStart = (char *)&SpStart;
  UBaseType_t watermarkStart = uxTaskGetStackHighWaterMark(NULL);
  char *StackPtrEnd = StackPtrAtStart - watermarkStart;
  Serial.printf("=== Stack info ===\n");
  Serial.printf("Free Stack near start is:  %d\r\n", (uint32_t)StackPtrAtStart - (uint32_t)StackPtrEnd);
}


void ckeckStack(void)
{
  UBaseType_t watermarkStart = uxTaskGetStackHighWaterMark(NULL);
  if(watermarkStart < 50)
  {
    Serial.printf("Warning: Task %s low stack: %d \n",pcTaskGetName(NULL), watermarkStart);
    delay(1000);
  }
}