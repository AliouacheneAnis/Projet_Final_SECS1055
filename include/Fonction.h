#include<Arduino.h>
#include <Keypad.h>
#include "DFRobot_RGBLCD1602.h"
#include<Wire.h>

char dtaUart[15];
char dtaLen = 0;
DFRobot_RGBLCD1602 lcd(/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show
void waitForTime(unsigned long timeToWait); 

//Constants
#define ROWS 4
#define COLS 4

//Parameters
const char kp4x4Keys[ROWS][COLS]  = {{'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};

const String pin = "1111";
const int FingerPin = 1; 

unsigned long previousMillis = 0;
const unsigned long interval = 2000; // 2 second interval
const unsigned long interval1 = 4000; // 2 second interval
const unsigned long doorOpenTime = 30000; // 30 second door open time
const unsigned long doorblockedTime = 30000; // 30 second door open time

String keyEntrer, Pass; 
bool StatusPorte = false;
int NbrErreurs = 0; 

// Setup Pin
byte rowKp4x4Pin [4] = {13, 12, 11, 10};  
byte colKp4x4Pin [4] = {9, 8, 7, 6};

//Variables
Keypad kp4x4  = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);


// Declaration Fonction 
void readKp4x4(); 


void readKp4x4() {

 // function readKp4x4 
 //  Read button states from keypad
  char customKey = kp4x4.getKey();
  lcd.init();
  lcd.setCursor(0,0); 
  lcd.print("PIN :");
  lcd.print(Pass); 
  lcd.setCursor(0,1); 
  lcd.print("# Pour Confirme");


  if (customKey) {
      
      switch (customKey)
      {  
        
        case 'A':  // Dans le cas de pression sur la touche * rien qui ce passe 
            break;

        case 'B': // Dans le cas de pression sur la touche * rien qui ce passe 
            break;

        case 'C': /// Dans le cas de pression sur la touche * rien qui ce passe 
            break;

        case 'D': // Dans le cas de pression sur la touche * rien qui ce passe 
            break;
        
        case '*':  // Dans le cas de pression sur la touche * rien qui ce passe 
           break;

        case '#':  // Dans le cas de pression sur la touche # rien qui ce passe  

            if (keyEntrer == pin)
            {   

                Serial.print("le pin entrer est : "); 
                Serial.println(keyEntrer); 
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Pin correct !");
                lcd.setCursor(0,1); 
                lcd.print("Scan Finger now");
                /*
                while (millis() - previousMillis < interval) {
                 // do nothing
                }
                */ 
                waitForTime(interval);
                previousMillis = millis(); // reset the timer
                
                while (digitalRead(FingerPin)!= 1)
                {   
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Please scan ");
                    lcd.setCursor(0,1); 
                    lcd.print("Your Finger");
                    if (millis() - previousMillis >= 500) {
                            previousMillis = millis(); // reset the timer
                            // do something every 500ms
                    }
                }
                
                lcd.clear();
                lcd.print("porte ouverte !"); 
                Serial.println("porte ouverte ");
                StatusPorte = true; 
                appendPayload("PorteStatus", StatusPorte);  //Ajout de la donnée  au message MQTT
                sendPayload();  //Envoie du message via le protocole MQTT
                waitForTime(doorOpenTime);
                StatusPorte = false; 
                keyEntrer = "";
                Pass = "";
                NbrErreurs = 0; 
                lcd.clear(); 
                lcd.print("Porte fermee !"); 
                Serial.println("La porte est maintenant ferme" ) ; 
                Serial.println("Veuillez entrer votre code pin et taper # pour confirmer: "); 
                appendPayload("PorteStatus", StatusPorte);  //Ajout de la donnée  au message MQTT
                sendPayload();  //Envoie du message via le protocole MQTT
                waitForTime(interval1);
                lcd.clear();

                
            }else
            {   
                NbrErreurs += 1; 
                keyEntrer = "";
                Pass = "";
                 if (NbrErreurs == 3)
                    {
                        lcd.clear(); 
                        lcd.setCursor(0,0); 
                        lcd.print(" Porte Bloquer "); 
                        lcd.setCursor(0,1);
                        lcd.print("Pour 30 minutes");
                        appendPayload("NbrErreurs", NbrErreurs);  //Ajout de la donnée  au message MQTT
                        sendPayload();  //Envoie du message via le protocole MQTT
                        waitForTime(doorblockedTime);
                        NbrErreurs = 0 ; 
                        appendPayload("NbrErreurs", NbrErreurs);  //Ajout de la donnée  au message MQTT
                        sendPayload();  //Envoie du message via le protocole MQTT

                    }else{
                        lcd.clear(); 
                        lcd.setCursor(0,0); 
                        lcd.print("Erreur Pin !! "); 
                        lcd.setCursor(0,1);
                        lcd.print("Reessayez ...."); 
                        waitForTime(interval1);
                        Serial.print("Veuillez reentrer votre code pin : "); 
                    }
                    
                
                

            }
        
           break;

      default:   // Dans le cas de pression sur les touches numeriques entre 0 et 9 
            
            int Key = customKey - '0'; // Converssion char en type entier int on converti les char '0' - '9' en chiffre entier
            keyEntrer = keyEntrer + String(Key);
            Pass = Pass + "*";
            Serial.println(keyEntrer); 

          break;
      }     
 }

}

void waitForTime(unsigned long timeToWait) {
  unsigned long previousMillis = millis();
  while (millis() - previousMillis < timeToWait) {
    // do nothing
  }
}


