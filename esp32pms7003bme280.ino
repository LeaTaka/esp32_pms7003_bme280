/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
 * Licensed under MIT license
 * Version: 1.0.4
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      07/10/2019 Initial coding
 *  1.0.1   K Hoang      13/12/2019 Fix bug. Add features. Add support for ESP32
 *  1.0.4   K Hoang      07/01/2020 Use ESP_WiFiManager setHostname feature
 *******************************************************************
 * This example will open a configuration portal when no WiFi configuration has 
 * been previously entered or when a button is pushed. It is the easiest scenario 
 * for configuration but requires a pin and a button on the ESP32 device. The 
 * Flash button is convenient for this on NodeMCU devices.
 *
 * Also in this example a password is required to connect to the configuration portal 
 * network. This is inconvenient but means that only those who know the password or those 
 * already connected to the target WiFi network can access the configuration portal and 
 * the WiFi network credentials will be sent from the browser over an encrypted connection and
 * can not be read by observers.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
//For ESP32, To use ESP32 Dev Module, QIO, Flash 4MB/80MHz, Upload 921600
 
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "cfg.h"
#include "wakeUp.h"
#include "wifiManager.h"
#include "bme280.h"
#include "localTime.h"
#include "pms7003.h"
#include "powerBi.h"

void setup() 
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_PIN2, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("\nStarting");

  unsigned long startedAt = millis();

  //Local intialization. Once its business is done, there is no need to keep it around
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager("ConfigOnSwitch");
  
  ESP_wifiManager.setMinimumSignalQuality(-1);
  
  // We can't use WiFi.SSID() in ESP32as it's only valid after connected. 
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();
  
  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  // SSID to uppercase 
  ssid.toUpperCase();
  
  if (Router_SSID == "")
  {
    Serial.println("We haven't got any access point credentials, so get them now");   
     
    digitalWrite(PIN_LED, LED_ON); // Turn led on as we are in configuration mode.
    
    //it starts an access point 
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password)) 
      Serial.println("Not connected to WiFi but continuing anyway.");
    else 
      Serial.println("WiFi connected...yeey :)");    
  }

  digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.
  
  #define WIFI_CONNECT_TIMEOUT        15000L
  #define WHILE_LOOP_DELAY            200L
  #define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))
  
  startedAt = millis();
  
  while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
  {   
    WiFi.mode(WIFI_STA);
    WiFi.persistent (true);
    // We start by connecting to a WiFi network
  
    Serial.print("Connecting to ");
    Serial.println(Router_SSID);
  
    WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());

    int i = 0;
    while((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
    {
      delay(WHILE_LOOP_DELAY);
    }    
  }

  Serial.print("After waiting ");
  Serial.print((millis()- startedAt) / 1000);
  Serial.print(" secs more in setup(), connection result is ");

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("connected.");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else if (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_NO_SSID_AVAIL || WiFi.status() == WL_DISCONNECTED)
  {
    Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
    Serial.println("configuration portal started due to lack of correct credentials");
    wifiManager();
  }
  else
  {
    Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
  }

  //Print the wakeup reason for ESP32
  wakeupReason();
  
  //Initiate AP if requested by push button
  if (esp_sleep_get_wakeup_cause() == 2)
  {
    Serial.println(esp_sleep_get_wakeup_cause());
    wifiManager();
  }

  // Mijn eigen baksels erbij
  bme280();
  delay(5000);
  yield();
  pms7003();
  yield();
  localTime();
  yield();
  powerBi();
  yield();
  wakeUp();
}


void loop() 
{ 
}
