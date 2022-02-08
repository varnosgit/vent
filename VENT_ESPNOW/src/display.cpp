#include <display.h>

extern TFT_eSPI tft;
extern TFT_eSprite logtxt1;


void display_init(void)
{
    tft.init();
    // tft.setRotation(1);
    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);
    // tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // tft.setCursor(0, 0, 2);
    // tft.setTextColor(TFT_BLUEY);
    // // tft.setTextFont(4);
    // tft.println("Initializing...");
}

void display_log_init(void)
{
    
    logtxt1.setColorDepth(8);
    logtxt1.createSprite(135, 240);
    logtxt1.fillSprite(TFT_BLACK); // Fill sprite with blue
    logtxt1.setScrollRect(0, 0, 135, 240, TFT_BLACK);     // here we set scroll gap fill color to blue
    logtxt1.setTextColor(TFT_WHITE); // White text, no background
    logtxt1.setTextDatum(BL_DATUM);  // Bottom right coordinate datum
    // logtxt1.pushSprite(0, 0);

    // logtxt1.scroll(0,-16); // scroll stext 0 pixels left/right, 16 up

    // logtxt1.drawNumber(222, 31, 63, 2); // plot value in font 2
    // logtxt1.drawString("Hello World", 6, 0, 2); // draw at 6,0 in sprite, font 2
}

void display_log_print(const char *string)
{
  logtxt1.drawString(string, 0, 240, 2);
  logtxt1.pushSprite(0, 0);
  logtxt1.scroll(0,-16);
}

void display_log_print(const String &s)
{
  logtxt1.drawString(s, 0, 240, 2);
  logtxt1.pushSprite(0, 0);
  logtxt1.scroll(0,-16);
}
