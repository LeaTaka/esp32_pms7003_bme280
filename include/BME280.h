/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

/* * * * * * * * * * * * * * *  
 *  BME280         * ESP32   *
 *  SCK (SCL Pin)  * GPIO 22 *
 *  SDI (SDA pin)  * GPIO 21 *
 * * * * * * * * * * * * * * */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 22
#define BME_MISO 23
#define BME_MOSI 19
#define BME_CS 21

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
String temperature;
String humidity;

void bme280()
{
  unsigned status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  Serial.println(String("Temperature      = ")+bme.readTemperature()+String(" *C"));
  Serial.println(String("Pressure         = ")+bme.readPressure()/100.0F+String(" hPa"));
  Serial.println(String("Approx. Altitude = ")+bme.readAltitude(1013)+String(" mtr"));
  Serial.println(String("Humidity         = ")+bme.readHumidity()+String(" %"));
}