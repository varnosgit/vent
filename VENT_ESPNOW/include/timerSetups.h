#include <Arduino.h>

#include <Wire.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <display.h>

void timer_init(void);
void IRAM_ATTR onTimer(void);