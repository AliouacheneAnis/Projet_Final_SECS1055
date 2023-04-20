#include<Arduino.h>
#include <Keypad.h>
#include "DFRobot_RGBLCD1602.h"
#include<Wire.h>

// Objet et variable pour l'utilisation de l'ecran LCD 
DFRobot_RGBLCD1602 lcd(/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show
char dtaUart[15];
char dtaLen = 0;

//Constants pour le clavier 4*4 qui 4 row et 4 column 
#define ROWS 4
#define COLS 4

// Assiger les row et les colums de clavier au broches de Uc
const char kp4x4Keys[ROWS][COLS]  = {{'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};
byte rowKp4x4Pin [4] = {13, 12, 11, 10};  
byte colKp4x4Pin [4] = {9, 8, 7, 6};
Keypad kp4x4  = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);

// Variables 
String KeyEntrer, Pass; 
bool StatusPorte = false;
int NbrErreurs = 0; 

// Constantes 
const String PIN = "1111"; // Code pin de securite 

// Broche pour les capteur et actuateur 
const int FINGER_PIN = 1; 
const int RED_LED_PIN = 2, GREEN_LED_PIN = 3; 

// Variable utiliser pour les temps d'attente 
unsigned long PreviousMillis = 0;

const unsigned long INTERVAL = 2000; // 2 secondes INTERVAL
const unsigned long INTERVAL_1 = 4000; // 4 secondes INTERVAL
const unsigned long DOOR_OPEN_TIME = 30000; // 30 secondes door open 
const unsigned long DOOR_BLOCK_TIME = 300000; // 30 minutes door block time

// Declaration Fonctions
void readKp4x4();  // fonction principale 
void waitForTime(unsigned long timeToWait); // fonction temps d'attente 

void readKp4x4() {

  // lecture status des boutons du clavier 
  char CustomKey = kp4x4.getKey();

  // Lancement de l'ecran LCD et affichage d'instruction pour inserer le pin 
  lcd.init();
  lcd.setCursor(0,0); 
  lcd.print("PIN :");
  lcd.print(Pass); 
  lcd.setCursor(0,1); 
  lcd.print("# Pour Confirme");

  // Indiquer l'etat de la porte Red : Fermer , Green : Ouvert 
  digitalWrite(RED_LED_PIN, HIGH); 
  digitalWrite(GREEN_LED_PIN, LOW); 
  
  // Si y a un clique sur un bouton de clavier 
  if (CustomKey) {
      
      // un switch case pour entrer dans une condition selon le bouton appuyer 
    switch (CustomKey)
    {  
        
        case 'A':  // Dans le cas de pression sur la touche A rien qui ce passe 
            break;

        case 'B': // Dans le cas de pression sur la touche B rien qui ce passe 
            break;

        case 'C': /// Dans le cas de pression sur la touche C rien qui ce passe 
            break;

        case 'D': // Dans le cas de pression sur la touche D rien qui ce passe 
            break;
        
        case '*':  // Dans le cas de pression sur la touche * rien qui ce passe 
           break;

        case '#':  // Dans le cas de pression sur la touche # on verifier le pin entrer par l'utilisateur  
            
            /* Si le pin saisie correspond a notre PIN de securite entrer dans la condition et passer a l'empreinte digitale
            Si non incrementer le nombre d'erreur + 1 et redonner l'acces a l'utilisateur pour saisisr a nouveau le Pin  */ 
            if (KeyEntrer == PIN)
            {   

                Serial.print("le pin entrer est : "); 
                Serial.println(KeyEntrer); 

                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Pin correct !");
                lcd.setCursor(0,1); 
                lcd.print("Scan Finger now");

                waitForTime(INTERVAL); // attente 
                PreviousMillis = millis(); // reset du timer 
                
                // affichage du message scan finger si l'utilisateur n'a pas encore scanner son empreinte pour la validation 
                while (digitalRead(FINGER_PIN)!= 1)
                {   
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Please scan ");
                    lcd.setCursor(0,1); 
                    lcd.print("Your Finger");
                    if (millis() - PreviousMillis >= 500) {
                            PreviousMillis = millis();
                    }
                }
                
                // Affichage sur l'ecran LCD que la porte est ouverte 
                lcd.clear();
                lcd.print("porte ouverte !"); 
                Serial.println("porte ouverte ");

                // Changer l'etat des LED et le status de la porte 
                digitalWrite(RED_LED_PIN, LOW); 
                digitalWrite(GREEN_LED_PIN, HIGH); 
                StatusPorte = true; 

                // Envoi Status de la porte vers Thingsbaord
                appendPayload("PorteStatus", StatusPorte);  //Ajout de la donnée  au message MQTT
                sendPayload();  //Envoie du message via le protocole MQTT

                waitForTime(DOOR_OPEN_TIME); // attente 30 secondes puis la porte se ferme 
                StatusPorte = false;   // changement etat porte vers ferme 
                KeyEntrer = "";
                Pass = "";
                NbrErreurs = 0; 
                lcd.clear(); 
                lcd.print("Porte fermee !"); 
                Serial.println("La porte est maintenant ferme" ) ; 
                Serial.println("Veuillez entrer votre code pin et taper # pour confirmer: "); 

                // Envoi etat de la porte vers thingsbaord
                appendPayload("PorteStatus", StatusPorte);  //Ajout de la donnée  au message MQTT
                sendPayload();  //Envoie du message via le protocole MQTT

                waitForTime(INTERVAL_1);
                lcd.clear();

                
            }else
            {   
                NbrErreurs += 1; 
                KeyEntrer = "";
                Pass = "";

                 // Si l'utilisateur se trompe 3 fois de suite dans la saise de pin la porte se bloque pour 30 minutes 
                 if (NbrErreurs == 3)
                    {
                        lcd.clear(); 
                        lcd.setCursor(0,0); 
                        lcd.print(" Porte Bloquer "); 
                        lcd.setCursor(0,1);
                        lcd.print("Pour 30 minutes");

                        // Envoi nombre d'erreur de saise pour avertir le proprietaire d'une tentative d'access non autorise 
                        appendPayload("NbrErreurs", NbrErreurs);  //Ajout de la donnée  au message MQTT
                        sendPayload();  //Envoie du message via le protocole MQTT

                        waitForTime(DOOR_BLOCK_TIME); // attente 30 minutes : porte bloquer 
                        
                        // Apres 30 seconde reset nombres d'erreur et debloquer la porte 
                        NbrErreurs = 0 ; 
                        appendPayload("NbrErreurs", NbrErreurs);  //Ajout de la donnée  au message MQTT
                        sendPayload();  //Envoie du message via le protocole MQTT

                    }else{
                        
                        // en cas d'erreur dans la saise de pin redonner access a l'utilisateur
                        lcd.clear(); 
                        lcd.setCursor(0,0); 
                        lcd.print("Erreur Pin !! "); 
                        lcd.setCursor(0,1);
                        lcd.print("Reessayez ...."); 
                        waitForTime(INTERVAL_1);
                        Serial.print("Veuillez reentrer votre code pin : "); 
                    }
            }

           break;

      default:   // Dans le cas de pression sur les touches numeriques entre 0 et 9 
            
            int Key = CustomKey - '0'; // Converssion char en type entier int on converti les char '0' - '9' en chiffre entier
            KeyEntrer = KeyEntrer + String(Key); // construction de Pin entrer pour l'utiliser en comparaison avec le pin de securite 
            Pass = Pass + "*";  // * qui sert a l'affichage sur l'ecran a chaque fois que l'utilisateur sasie un numero

          break;
        }     
    }
}

// Fonction d'attente en utilisant millis 
void waitForTime(unsigned long timeToWait) {
    unsigned long PreviousMillis = millis();
    while (millis() - PreviousMillis < timeToWait) {
    }
}


