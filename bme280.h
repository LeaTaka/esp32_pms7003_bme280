/* * * * * * * * * * * * * * *  
 *  BME280         * ESP32   *
 *  SCK (SCL Pin)  * GPIO 22 *
 *  SDI (SDA pin)  * GPIO 21 *
 * * * * * * * * * * * * * * */

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme; // I2C
String temperature;
String humidity;

void bme280()
{
  bool status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  Serial.println(String("Temperature      = ")+bme.readTemperature()+String(" *C"));
  //Serial.println(String("Pressure         = ")+bme.readPressure()/100.0F+String(" hPa"));
  //Serial.println(String("Approx. Altitude = ")+bme.readAltitude(1013)+String(" mtr"));
  Serial.println(String("Humidity         = ")+bme.readHumidity()+String(" %"));
}
