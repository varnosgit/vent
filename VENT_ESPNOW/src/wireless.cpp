#include "wireless.h"

uint8_t registerStatus = 0;
bool newData_flag = 0;
uint8_t myMAC_Address[] = {0xFF, 0xFF, 0xFF , 0xFF , 0xFF ,0xFF};
uint8_t Brodcast_Address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF ,0xFF}; // {0x94, 0xB9, 0x7E , 0xD9 , 0xA1 ,0x04}; 
uint8_t Controller_Address[] = {0x08, 0x3A, 0xF2 , 0x45 , 0xA5 ,0xC8};
uint8_t TEMP_Address[] = {0x83, 0xCC, 0xA8 , 0x61 , 0x4B ,0x0C};
struct_message myData;

void wireless_init(void)
{
    //WiFi.persistent( false );
  WiFi.mode(WIFI_STA); //WIFI_MODE_STA
  esp_wifi_get_mac(WIFI_IF_STA, myMAC_Address);
  Serial.println(WiFi.macAddress()); display_log_print("Wi-Fi MAC Address:"); display_log_print(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");display_log_print("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  pairNew_device(Brodcast_Address);
}
////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////// // Register new peer
bool pairNew_device(uint8_t pair_addr[])
{
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, pair_addr, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return false;
  }
  else Serial.println("Peer added successfully!");
  Serial.printf("Peer Address: %02X:%02X:%02X:%02X:%02X:%02X\n", pair_addr[0], pair_addr[1], pair_addr[2],
                                                                 pair_addr[3], pair_addr[4], pair_addr[5]);

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  newData_flag = true;
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  //display_log_print("new data recieved");
}

void sendDataTo(uint8_t *mac_addr, uint8_t command, uint8_t *theData)
{   
    uint8_t temp_send_mac[6];
    memcpy(temp_send_mac, mac_addr, 6);
    memcpy(myData.sender_MAC_addr, myMAC_Address, 6);

    myData._sender = 0x01; //0 unknown, 1 controller, 2 termostat, 3 vent
    myData._command = command; // register it to controller

    if (esp_now_send(temp_send_mac, (uint8_t *) &myData, sizeof(myData)) == ESP_OK)
    {  
      Serial.println("Sent with success");
    }
    else 
    {
      Serial.println("Error sending data");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
