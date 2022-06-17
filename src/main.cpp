#include <BME280.h>
#include <PMS7003.h>
#include <WIFIMANAGER.h>

void setup() {

  Serial.begin(115200);
  Serial.println("\nStarting\n");

  wifimanager_setup();
  bme280();
  pms7003();
}

void loop() {}
