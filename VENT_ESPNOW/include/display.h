#include <Arduino.h>

#include <Wire.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#define TFT_BLUEY 0x4FE7 // New colour

void display_init(void);
void display_log_init(void);
void display_log_print(const char *string);
void display_log_print(const String &s);
