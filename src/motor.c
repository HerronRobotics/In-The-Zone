#include "motor.h"
#include "API.h"
#include "main.h"

//This maps motors (the indices) to groups (the values)
int motorMap[] = {0, LEFT_DRIVE, LEFT_DRIVE, LIFT, LIFT, LIFT, LIFT, RIGHT_DRIVE, RIGHT_DRIVE, 0};
int inputs[10];

void setMotorGroup(int group, int power)
{
  for(int i = 0; i < 10; i++)
  {
    if(motorMap[i] == group)
    {
      inputs[i] = power;
    }
  }
}

int map(int input)
{
  if(abs(input) < 16)
  {
    return 0;
  }
  return (input > 0? 16 : -16) + ((input * input * input * 1.0)/16384)*.75;
}

void motorControl()
{
  for(int i = 0; i < 10; i++)
  {
    inputs[i] = 0;
  }

  while(true)
  {
    for(int i = 0; i < 10; i++)
    {
      motorSet(i + 1, map(inputs[i]));
    }

    delay(20);
  }
}
