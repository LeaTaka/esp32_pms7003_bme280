#include "PMS.h"
#include "driver/rtc_io.h"

/* * * * * * * * * * * * * * * *
 * PMS7003-TX  >> ESP32-GPIO5  *
 * PMS7003-RX  >> ESP32-GPIO4  *
 * PMS7003-SET >> ESP32-GPIO13 *
 * PMS7003-RST >> ESP32-GPIO14 *
 * * * * * * * * * * * * * * * */

// To use Deep Sleep connect RST to GPIO16 (D0)
// https://www.espruino.com/PMS7003

#define DEBUG_OUT Serial

// PMS_READ_INTERVAL (4:30 min) and PMS_READ_DELAY (30 sec) CAN'T BE EQUAL! 
// Values are also used to detect sensor state.
static const uint32_t PMS_READ_INTERVAL = 60000;
static const uint32_t PMS_READ_DELAY = 30000;

String pms10;
String pms25;
String pms100;

// Default sensor state.
uint32_t timerInterval = PMS_READ_DELAY;

PMS pms(Serial1);

void readData()
{
  PMS::DATA data;

  // Clear buffer (removes potentially old data) before read. Some data could have been also sent before switching to passive mode.
  while (Serial1.available()) { Serial1.read(); }

  DEBUG_OUT.println("Send read request...");
  pms.requestRead();

  DEBUG_OUT.println("Reading data...");
  if (pms.readUntil(data))
  {
    pms10 = data.PM_AE_UG_1_0;
    DEBUG_OUT.print("PM 1.0 (ug/m3): ");
    DEBUG_OUT.println(data.PM_AE_UG_1_0);

    pms25 = data.PM_AE_UG_2_5;
    DEBUG_OUT.print("PM 2.5 (ug/m3): ");
    DEBUG_OUT.println(data.PM_AE_UG_2_5);

    pms100 = data.PM_AE_UG_10_0;
    DEBUG_OUT.print("PM 10.0 (ug/m3): ");
    DEBUG_OUT.println(data.PM_AE_UG_10_0);
  }
  else
  {
    DEBUG_OUT.println("No data.");
    // Reset the sensor
    rtc_gpio_hold_dis(GPIO_NUM_14);
    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
    delay(500U);
    digitalWrite(14, HIGH);
    delay(500U);
    rtc_gpio_hold_en(GPIO_NUM_14);
  }
}

void pms7003()
{
  // GPIO5, GPIO4 (TX/RX pin on ESP-32 Development Board)
  Serial1.begin(PMS::BAUD_RATE, SERIAL_8N1, 5, 4);
  DEBUG_OUT.begin(115200);

  // Switch to passive mode.
  pms.passiveMode();
  Serial.println();
  Serial.println("Activate the fan by putting pin on HIGH");
  // in case gpio was on hold: disable hold
  rtc_gpio_hold_dis(GPIO_NUM_13);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500U);
  
  // Default state after sensor power, but undefined after ESP restart e.g. by OTA flash, so we have to manually wake up the sensor for sure.
  // Some logs from bootloader is sent via Serial port to the sensor after power up. This can cause invalid first read or wake up so be patient and wait for next read cycle.
  pms.wakeUp();

  delay(PMS_READ_DELAY);
  readData();
  pms.sleep();
  Serial.println("Deactivate the fan by putting pin on LOW");
  Serial.println();
  digitalWrite(13, LOW);
  // enable hold on gpio
  rtc_gpio_hold_en(GPIO_NUM_13);
}
