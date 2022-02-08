#include"timerSetups.h"
#include <display.h>


extern TFT_eSPI tft;
extern TFT_eSprite logtxt1;
volatile int interruptCounter;
int totalInterruptCounter;

 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void timer_init(void)
{
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
    // 80 4000  = 250Hz
}

void IRAM_ATTR onTimer(void) 
{
   
        portENTER_CRITICAL_ISR(&timerMux);
        
        interruptCounter++;
        if (interruptCounter == 2500)  interruptCounter = 0; // 10s
        portEXIT_CRITICAL_ISR(&timerMux);
    
}

