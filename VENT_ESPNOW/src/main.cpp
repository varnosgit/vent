#include <Arduino.h>

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <display.h>

#include "timerSetups.h"
#include "algos.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */
#define EEPROM_SIZE 1

RTC_DATA_ATTR int bootCount = 0;
uint8_t registrStatus = 0;

bool loop_start_flag = false;
TaskHandle_t CoreZEROTasks;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite logtxt1 = TFT_eSprite(&tft); // Sprite object stext1


int interruptCounter_last = 0;
extern volatile int interruptCounter;

int dataCounter = 0;


/////////////////////////////////////////////////////////////////////////////// esp now
uint8_t myMAC_Address[] = {0xFF, 0xFF, 0xFF , 0xFF , 0xFF ,0xFF};
uint8_t Brodcast_Address[] = {0xFF, 0xFF, 0xFF , 0xFF , 0xFF ,0xFF}; // {0x94, 0xB9, 0x7E , 0xD9 , 0xA1 ,0x04}; 
uint8_t Controller_Address[] = {0x08, 0x3A, 0xF2 , 0x45 , 0xA5 ,0xC8};
uint8_t TERMO_Address[] = {0x84, 0xCC, 0xA8 , 0x61 , 0x4B ,0x0C};

typedef struct struct_message {
  uint8_t sender_MAC_addr[6];
  uint8_t reciever_MAC_addr[6];
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////// // Register new peer
bool pairNew_device(uint8_t pair_addr[])
{
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, pair_addr, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////////
void coreZEROTasks_code( void * pvParameters ){
  for(;;){
    delay(4);
  } 
}
////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  registrStatus = EEPROM.read(0);  // the 0 location determind if the vent is registered to the controller before 0 = no, 1 = yes
  EEPROM.write(0, registrStatus);
  EEPROM.commit();

  // ++bootCount;
  // Serial.println("Boot number: " + String(bootCount));
  //logtxt1.drawNumber(getCpuFrequencyMhz(), 71, 240, 2);
  // delay(300); display_log_print("Boot number: " + String(bootCount));

  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");


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


  //WiFi.persistent( false );
  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_get_mac(WIFI_IF_STA, myMAC_Address);
  Serial.println(WiFi.macAddress()); display_log_print("Wi-Fi MAC Address:"); display_log_print(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");display_log_print("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

}
////////////////////////////////////////////////////////////////////////////////////
void loop()
{
 if (registrStatus == 0)  // this vent is not registerd before
 {
    *myData.sender_MAC_addr = *myMAC_Address;
    strcpy(myData.a, "A DATA TO RECIVE");
    myData.b = random(1,20);
    myData.c = 1.2;
    myData.d = false;
    // Send message via ESP-NOW
    if (esp_now_send(Brodcast_Address, (uint8_t *) &myData, sizeof(myData)) == ESP_OK)
    {  
      Serial.println("Sent with success");
    }
    else 
    {
      Serial.println("Error sending the data");
      delay(2000);
    }
    
 }
  

delay(500);
 // esp_deep_sleep_start();
}
