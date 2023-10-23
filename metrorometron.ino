#include <Wire.h>
#include <SPI.h>
#include <Adafruit_AHTX0.h>
#include <Arduino.h>
#include <BH1750.h>
#define BLYNK_PRINT Serial
#define TIMER_INTERRUPT_DEBUG         1
#define _TIMERINTERRUPT_LOGLEVEL_     1

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

#include <ESP8266TimerInterrupt.h>
#define TIMER_INTERVAL_MS       30000

// Init ESP8266 timer 1
ESP8266Timer Timer;

//=======================================================================
void IRAM_ATTR TimerHandler(){
  Serial.println("Task exceeded time allocation. Possible network issue. Entering Deep Sleep 10min");
  ESP.deepSleep(600e6); 
}

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL60wW1M9Jp"
#define BLYNK_TEMPLATE_NAME "Meteorometron"
#define BLYNK_AUTH_TOKEN "use-yours"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;
char ssid[] = "Oneirodyne";
char pass[] = "Oneirodyne";
Adafruit_AHTX0 aht;
BH1750 lightMeter;

float aht_hume = 0;
float aht_temp = 0;
float lux;

void refresh() {
  
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  aht_temp = temp.temperature;
  aht_hume = humidity.relative_humidity;
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degrees C");
  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println("% rH");
  read_light();
  Blynk.virtualWrite(V0, aht_temp);
  Blynk.virtualWrite(V1, aht_hume);
  Blynk.virtualWrite(V2, lux);
  Serial.println("Publishing results");
  delay(1000);
}

void read_light() {
  if (lightMeter.measurementReady(true)) {
    lux = lightMeter.readLightLevel();
    Serial.print(F("Light: "));
    Serial.print(lux);
    Serial.println(F(" lx"));

    if (lux < 0) {
      Serial.println(F("Error condition detected"));
    } else {
      if (lux > 40000.0) {
        // reduce measurement time - needed in direct sun light
        if (lightMeter.setMTreg(32)) {
          Serial.println(
            F("Setting MTReg to low value for high light environment"));
        } else {
          Serial.println(
            F("Error setting MTReg to low value for high light environment"));
        }
      } else {
        if (lux > 10.0) {
          // typical light environment
          if (lightMeter.setMTreg(69)) {
            Serial.println(F(
              "Setting MTReg to default value for normal light environment"));
          } else {
            Serial.println(F("Error setting MTReg to default value for normal "
                             "light environment"));
          }
        } else {
          if (lux <= 10.0) {
            // very low light environment
            if (lightMeter.setMTreg(138)) {
              Serial.println(
                F("Setting MTReg to high value for low light environment"));
            } else {
              Serial.println(F("Error setting MTReg to high value for low "
                               "light environment"));
            }
          }
        }
      }
    }
    Serial.println(F("--------------------------------------"));
  }
}
void setup() {
  // Open serial communications and wait for port to open:
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
  pinMode(13, OUTPUT);
  Serial.begin(74880);
  aht.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  Timer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler);
  digitalWrite(13, 1);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}


void loop() {
  Blynk.run();
  timer.run();
  refresh();
  Serial.println("Updated to cloud. Entering Deep Sleep 10min");
  ESP.deepSleep(600e6); 

}
