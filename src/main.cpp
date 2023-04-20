/*
  Titre      : SECS1055_Projet Final
  Auteur     : Anis Aliouachene
  Date       : 20/04/2023
  Description: projet visant à améliorer la sécurité des serrures de porte IoT en y intégrant un lecteur biométrique 
               et une fonction de notification pour le propriétaire. Cette solution permettra une authentification plus sécurisée 
               et m'informera de toute tentative d'accès non autorisé.
  Version    : 0.0.4
*/

#include <Arduino.h>
#include <SPI.h>
#include <WIFIConnector_MKR1010.h>
#include "MQTTConnector.h"
#include "Fonction.h"


void setup() {

  Serial.begin(9600); 
  waitForTime(INTERVAL_1); // attente 

  wifiConnect(); // Branchement au WIFI 
  MQTTConnect(); // Branchement au broker MQTT
  
  // Setup des broches
  pinMode(FINGER_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT); 
  pinMode(RED_LED_PIN,OUTPUT); 

  Serial.println("Entrer le pin et taper # pour confirmer: ");
  waitForTime(INTERVAL_1); // attente 

}

void loop() {
  
  // Appeler la fonction qui est dans le fichier fonction.h 
  readKp4x4();   

}