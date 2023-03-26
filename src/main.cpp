#include <Arduino.h>
#include <SPI.h>
#include <WIFIConnector_MKR1010.h>
#include "MQTTConnector.h"
#include "Fonction.h"


void setup() {
  Serial.begin(9600); 
  delay(4000);
  wifiConnect(); // Branchement au WIFI 
  MQTTConnect(); // Branchement au broker MQTT 
  pinMode(FingerPin, INPUT);
  Serial.println("Entrer le pin et taper # pour confirmer: ");
  delay(3000);
}

void loop() {
  
  // Appeler la fonction 
  readKp4x4();   

}