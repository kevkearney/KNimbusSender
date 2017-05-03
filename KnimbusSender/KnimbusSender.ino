#include <AS3935mod.h>

#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>

#include <DHT.h>
#include "KnimbusRadio.h"
#include <LowPower.h>

Adafruit_BME280 bmp; // I2C
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define DHTPIN            4        // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

DHT dht(DHTPIN, DHTTYPE);
KnimbusRadio kRadio;

//Altitude of Station
const short stationAltitude = 342;
short sleepTime = 10;

volatile bool lightningDetected = false;

void alert() {
  lightningDetected = true;
}

void setup() {
  delay(3000);
  Serial.begin(9600);
  kRadio.SetupRadio();
  InitializeBarometer();
  InitializeThermometer();
  InitializeLightSensor();
  InitializeLightningSensor();
}

void loop() {
  Weather_t weatherData;

  float fltHumidity;
  bool tempSuccess = GetThermometerValue(weatherData.Temperature, fltHumidity);
  bool baroSuccess = GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature);
  bool lightSuccess = GetLightValue(weatherData.Lux);

  weatherData.Humidity = (int)fltHumidity;

  String radioResponse;

  Serial.println("Ready to Transmit");
  bool xmitSuccess = kRadio.XMitWeather(weatherData, radioResponse);
  if (xmitSuccess) {
    Serial.println("Successful Transmit");
    sleepTime = ParseSleepTimeResponse(radioResponse);
  }

  Serial.print("Next sleep time: ");
  Serial.println(sleepTime);

  short numberOfCycles = sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      translateIRQ(mod1016.getIRQ());
      lightningDetected = false;
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

int ParseSleepTimeResponse(String response) {
  int sleepTimeBeginIndex = response.indexOf(":") + 1;
  int sleepTimeEndIndex = response.indexOf("}");
  return response.substring(sleepTimeBeginIndex, sleepTimeEndIndex).toInt();;
}

void InitializeLightningSensor() {
  while (!Serial) {}
  Serial.println("Welcome to the MOD-1016!");
  //I2C
  //Wire.begin();
  mod1016.init(MOD1016IRQ_PIN);
  autoTuneCaps(MOD1016IRQ_PIN);
  //mod1016.setTuneCaps(2);
  mod1016.setOutdoors();
  mod1016.setNoiseFloor(4);

  Serial.println("TUNE\tIN/OUT\tNOISEFLOOR");
  Serial.print(mod1016.getTuneCaps(), HEX);
  Serial.print("\t");
  Serial.print(mod1016.getAFE(), BIN);
  Serial.print("\t");
  Serial.println(mod1016.getNoiseFloor(), HEX);
  Serial.print("\n");

  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);
  mod1016.getIRQ();
  Serial.println("after interrupt");
}

void translateIRQ(uns8 irq) {
  switch (irq) {
    case 1:
      Serial.println("NOISE DETECTED");
      kRadio.XMitLightning("LNoise", 0);
      break;
    case 4:
      Serial.println("DISTURBER DETECTED");
      kRadio.XMitLightning("LDisturber", 0);
      break;
    case 8:
      Serial.println("LIGHTNING DETECTED");
      int lightningDistance = mod1016.calculateDistance();
      kRadio.XMitLightning("Lightning:", lightningDistance);
      printDistance();
      break;
  }
}

void printDistance() {
  int distance = mod1016.calculateDistance();
  if (distance == -1)
    Serial.println("Lightning out of range");
  else if (distance == 1)
    Serial.println("Distance not in table");
  else if (distance == 0)
    Serial.println("Lightning overhead");
  else {
    Serial.print("Lightning ~");
    Serial.print(distance);
    Serial.println("km away\n");
  }
}

void InitializeBarometer() {
  Serial.println("Pressure Sensor Test");
  Serial.println("");
  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP280 ... check your connections */
    Serial.print("Ooops, no BMP280 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(500);
}

void InitializeThermometer() {
  Serial.println("DHT22 initialize!");
  dht.begin();
}

bool InitializeLightSensor() {
  Serial.println("Light Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
    return false;
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();
  return true;
}

bool GetLightValue(float &lux) {
  /* Get a new sensor event */
  delay(500);
  sensors_event_t event;
  tsl.getEvent(&event);

  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    lux = event.light;
    Serial.print(event.light); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
}
bool GetThermometerValue(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
  return true;
}

bool GetBarometerValue(float &barometerValue, float &temperature) {

  temperature = bmp.readTemperature();
  barometerValue = bmp.readPressure() / 100;

  Serial.print(F("Baro Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  float SeaLevelbarometerValue = seaLevelForAltitude(stationAltitude, barometerValue );

  Serial.print("Raw Pressure: ");
  Serial.println(barometerValue);
  Serial.print("Sea Level Pressure: ");
  Serial.println(SeaLevelbarometerValue);
}



/**************************************************************************/
/*!
    Calculates the pressure at sea level (in hPa) from the specified altitude
    (in meters), and atmospheric pressure (in hPa).

    @param  altitude      Altitude in meters
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/
float seaLevelForAltitude(float altitude, float atmospheric)
{
  // Equation taken from BMP180 datasheet (page 17):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

  return atmospheric / pow(1.0 - (altitude / 44330.0), 5.255);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

