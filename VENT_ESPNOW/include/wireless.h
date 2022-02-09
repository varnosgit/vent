#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <EEPROM.h>
#include <display.h>

typedef struct struct_message {
  uint8_t sender_MAC_addr[6];
  uint8_t reciever_MAC_addr[6];
  uint8_t _sender;   //0 unknown, 1 controller, 2 termostat, 3 vent
  uint8_t _command;  //0 unknown, 1 register
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;



void wireless_init(void);
bool pairNew_device(uint8_t pair_addr[]);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void sendDataTo(const uint8_t *mac_addr, uint8_t command, uint8_t *theData);