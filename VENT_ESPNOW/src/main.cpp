#include <Arduino.h>
#include <Arduino_JSON.h>

#include "WiFi.h"
#include <esp_now.h>
#include <Wire.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <display.h>
#include <EEPROM.h>

// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>

#include "timerSetups.h"
#include "algos.h"

#include "ESPAsyncWebServer.h"


#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */
#define EEPROM_SIZE 1

RTC_DATA_ATTR int bootCount = 0;

bool loop_start_flag = false;
TaskHandle_t CoreZEROTasks;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite logtxt1 = TFT_eSprite(&tft); // Sprite object stext1


// BLEServer *pServer;
// BLEService *pService;
// BLECharacteristic *pCharacteristic;

int interruptCounter_last = 0;
extern volatile int interruptCounter;

long irValue, irArray[240];
float irArray_float[2500], irMax = 0, irmin = 1000000;
int dataCounter = 0;


/////////////////////////////////////////////////////////////////////////////// esp now
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t WROOM_Address[] = {0xFF, 0xFF, 0xFF , 0xFF , 0xFF ,0xFF}; // {0x94, 0xB9, 0x7E , 0xD9 , 0xA1 ,0x04}; 
uint8_t TTGOPin_Address[] = {0x08, 0x3A, 0xF2 , 0x45 , 0xA5 ,0xC8};
uint8_t TTGO_NO_Pin_Address[] = {0x84, 0xCC, 0xA8 , 0x61 , 0x4B ,0x0C};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3],
                mac[4], mac[5]);
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a); display_log_print(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("Bool: ");
  Serial.println(myData.d);
  Serial.println();
}
////////////////////////////////////////////////////////////////////////////////////



void coreZEROTasks_code( void * pvParameters );

void setup()
{
 // ledState = EEPROM.read(0);
  // EEPROM.write(0, ledState);
  // EEPROM.commit();
  display_init();
  display_log_init();   display_log_print("Initialising...");
 
  delay(100); Serial.begin(115200);   display_log_print("Serial Debug connect!");
  logtxt1.drawNumber(getCpuFrequencyMhz(), 71, 240, 2);
  delay(300); display_log_print("CPU Freq.:     MHz");

  

 
  xTaskCreatePinnedToCore(
                    coreZEROTasks_code,      /* Task function. */
                    "Task1",        /* name of task. */
                    10000,          /* Stack size of task */
                    NULL,           /* parameter of the task */
                    1,              /* priority of the task */
                    &CoreZEROTasks, /* Task handle to keep track of created task */
                    0);             /* pin task to core 0 */                  
  delay(500); 

  delay(200); display_log_print("2nd Core setup!");
  delay(100); timer_init(); display_log_print("Timers connected!");

  // tft.setCursor(0, 0);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.print("No Finger    ");
    ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  //logtxt1.drawNumber(getCpuFrequencyMhz(), 71, 240, 2);
  delay(300); display_log_print("Boot number: " + String(bootCount));

  //WiFi.persistent( false );
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress()); display_log_print("Wi-Fi MAC Address:"); display_log_print(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");display_log_print("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

    // Register peer
  esp_now_peer_info_t peerInfo1, peerInfo2;
  memcpy(peerInfo1.peer_addr, WROOM_Address, 6);
  peerInfo1.channel = 0;  
  peerInfo1.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo1) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
   memcpy(peerInfo2.peer_addr, TTGOPin_Address, 6);
  peerInfo2.channel = 0;  
  peerInfo2.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo2) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

     esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

 
}

void coreZEROTasks_code( void * pvParameters ){
 

  for(;;){
    // Serial.print("Task1 running on core ");  Serial.println(xPortGetCoreID());
  //  Serial.println(irValue); 
    delay(4);
  } 
}

// loop will be excuted on core1
void loop()
{
 
   // Set values to send
  strcpy(myData.a, "A DATA TO RECIVE");
  myData.b = random(1,20);
  myData.c = 1.2;
  myData.d = false;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(TTGO_NO_Pin_Address, (uint8_t *) &myData, sizeof(myData));
  esp_err_t result2 = esp_now_send(TTGOPin_Address, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);

  esp_deep_sleep_start();
//tft.pushImage(0,0, 240, 135, info);  
// irValue = PPG_MAX_Sensor.getIR();
//Serial.println(irValue); //Send raw data to plotter
}
