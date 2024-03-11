#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "user6"
#define BLYNK_TEMPLATE_NAME "user6@wyns.it"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <EEPROM.h>

char auth[] = "d-2wJVr7YUGn_Gz62PawdciDImZBjU0g";
char ssid[] = "embed";
char pass[] = "weareincontrol";

#define DHTPIN 18
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
float oldDigiT;
float oldManuT;
float setTemp;
const int ledrood = 21;
const int potPin = 33;

BlynkTimer timer;

// Declaratie van de functie voordat deze wordt gebruikt
void sendSensor();

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(ledrood, OUTPUT);
  pinMode(potPin, INPUT);

  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);
  timer.setInterval(1000L, sendSensor);

  // Laatst aangepaste waarden inladen vanuit EEPROM
  int storedPotValue = EEPROM.read(0);
  if (storedPotValue != 255) {  // Controleer of er eerder waarden zijn opgeslagen
    analogWrite(potPin, storedPotValue);
  }
}

void loop() {
  Blynk.run();
  timer.run();

}

void sendSensor() {
  float getal = map(analogRead(potPin), 0, 4095, 0, 30);
  float t = dht.readTemperature();

  Serial.println(String(getal) + "°C");
  Serial.println(String(t) + "°C");
  Blynk.virtualWrite(V0, t);


  if (t < getal - 0.5) {
    digitalWrite(ledrood, HIGH);
    Blynk.virtualWrite(V2, 255);
  } else if (t > getal + 0.5) {
    digitalWrite(ledrood, LOW);
    Blynk.virtualWrite(V2, 0);
  }

  // Laatst aangepaste potentiometerwaarde opslaan in EEPROM
  EEPROM.write(0, analogRead(potPin));
  EEPROM.commit();
}

BLYNK_WRITE(V1) {
  int getal = param.asInt();

  if (dht.readTemperature() < getal - 0.5) {
    digitalWrite(ledrood, HIGH);
    Blynk.virtualWrite(V2, 255);
  } else if (dht.readTemperature() > getal + 0.5) {
    digitalWrite(ledrood, LOW);
    Blynk.virtualWrite(V2, 0);
  }
}
