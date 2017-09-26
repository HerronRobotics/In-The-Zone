#include "main.h"

#define BUTTON_LEFT 1
#define BUTTON_CENTER 2
#define BUTTON_RIGHT 4

//Standin for a real auton function, the real ones will be defined in a header
//and reside in src/autons.
void noOp() {}

//The previous result of lcdReadButtons, used to determine if a button was pushed or held.
int lastButtons = 0;

//LCD screen to use
FILE * lcdPort = uart1;

//Determines if a button was just pressed
bool isButtonNewlyPressed(int button)
{
  int currentButton = lcdReadButtons(lcdPort);
  if((currentButton & button) != (lastButtons & button))
  {
    if((currentButton & button) > 0)
    {
      return true;
    }
  }
  return false;
}

//The message to loop when the auton is not set. This needs to be 32 chars for the scroll.
char message[] = " Auton not set!  Auton not set! ";

//"autonNames" is an array of names, "autonRoutines" is an array of functions.
//"max" is the amount of routines aka the max selector value
//Auton names are put on screen, the function pointers store the actual routine.
//Each name refers to its matching index in the FP array.
const char * autonNames[] = {"     <none>     ", "  <go forward>  ", "   <blocking>   ", " <new strategy> "};
void (* autonRoutines[])(void) = {noOp, noOp, noOp, noOp};
int max = sizeof(autonNames)/sizeof(autonNames[0]);

//The selected auton index
int selected = 0;

//True if the auton is set
bool autonSet = false;

void lcdControl()
{
  lcdInit(lcdPort);
  lcdSetBacklight(lcdPort, true);

  //Scrolling progress through the selection menu
  int scrollCounter = 0;

  //Every 50 ms this updates
  int timer = 0;

  //Keeps track of the backlight for the flashing
  bool backlight = true;

  while(true)
  {
    if((timer % 8) == 0) //50ms * 8 = 400ms
    {
      if(!autonSet)
      {
        //Scroll "Auton not set" message
        char * msg = &message[scrollCounter % 16];
        lcdSetText(lcdPort, 1, msg);
        lcdSetText(lcdPort, 2, autonNames[selected]);
        scrollCounter++;
      }
      else
      {
        //Static message for set auton
        lcdSetText(lcdPort, 1, "  <Auton  Set>  ");
        lcdSetText(lcdPort, 2, autonNames[selected]);
      }
    }
    if((timer % 32) == 0 && !autonSet) //50ms * 32 = 1600ms
    {
      //Toggles the backlight every 1.6 seconds while auton is not set
      lcdSetBacklight(lcdPort, backlight);
      backlight = !backlight;
    }

    //When center button is pressed, toggle autonSet.
    if(isButtonNewlyPressed(BUTTON_CENTER))
    {
      autonSet = !autonSet;
      if(autonSet)
      {
        //Set backlight to true if auton is set
        backlight = true;
        lcdSetBacklight(lcdPort, backlight);
      }
    }
    if(!autonSet && isButtonNewlyPressed(BUTTON_LEFT))
    {
      //Decrease selection value or wrap if auton is not set and left is pressed
      selected--;
      if(selected < 0)
      {
        selected = max - 1;
      }
    }
    if(!autonSet && isButtonNewlyPressed(BUTTON_RIGHT))
    {
      //Increase selection value or wrap if auton is not set and right is pressed
      selected++;
      if(selected > (max - 1))
      {
        selected = 0;
      }
    }
    if(autonSet && isButtonNewlyPressed(BUTTON_LEFT | BUTTON_RIGHT))
    {
      //Flash screen to warn if left or right is pressed and auton is set.
      lcdSetBacklight(lcdPort, false);
      delay(200);
      lcdSetBacklight(lcdPort, true);
    }

    //Populates lastButtons, then waits and updates timer.
    lastButtons = lcdReadButtons(lcdPort);
    delay(50);
    timer++;
  }
}

//That's all folks
//Definitely nothing left















int ourWins = 0;
int theirWins = 0;
int ties = 0;

#define SCISSORS 0
#define PAPER 1
#define ROCK 2

int rpsWaitForButton()
{
  while(!isButtonNewlyPressed(7)) //Waits for any button
  {
    delay(50);
  }

  int buttons = lcdReadButtons(lcdPort);
  if(buttons & BUTTON_LEFT) return ROCK;
  if(buttons & BUTTON_CENTER) return PAPER;
  return SCISSORS;
}

const char * getStringFromNum(int choice)
{
  switch(choice)
  {
    case ROCK: return "ROCK";
    case PAPER: return "PAPER";
    case SCISSORS: return "SCSRS";
    default: return "ERROR";
  }
}

const char * processGame(int ours, int theirs)
{
  if(ours == theirs)
  {
    ties++;
    return "TIE GAME!";
  }
  if(ours == ROCK)
  {
    if(theirs == PAPER)
    {
      theirWins++;
      return "YOU WIN!";
    }
    else
    {
      ourWins++;
      return "I WIN!";
    }
  }
  if(ours == PAPER)
  {
    if(theirs == SCISSORS)
    {
      theirWins++;
      return "YOU WIN!";
    }
    else
    {
      ourWins++;
      return "I WIN!";
    }
  }
  if(ours == SCISSORS)
  {
    if(theirs == ROCK)
    {
      theirWins++;
      return "YOU WIN!";
    }
    else
    {
      ourWins++;
      return "I WIN!";
    }
  }
  return "ERROR";
}

void rpsTask()
{
  lcdInit(lcdPort);
  lcdSetBacklight(lcdPort, true);
  while(true)
  {
    lcdPrint(lcdPort, 1, "U:%d Y:%d T:%d", ourWins, theirWins, ties);
    lcdSetText(lcdPort, 2, "ROCK   PPR   SCS");
    int ourChoice = rand() % 3;
    int theirChoice = rpsWaitForButton();
    lcdSetBacklight(lcdPort, false);
    lcdPrint(lcdPort, 1, "%s vs %s", getStringFromNum(ourChoice), getStringFromNum(theirChoice));
    lcdSetText(lcdPort, 2, processGame(ourChoice, theirChoice));
    delay(200);
    lcdSetBacklight(lcdPort, true);
    delay(1500);
  }
}
