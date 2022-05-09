/*
  Language: cpp
  Comments Language : French
  Date: 2022-04-19
  Name: Arrosoir
  Hardware: Arduino Nano
  Firmware: 1.1
  Author: Pierre
  Description: Arrosoir automatisé en fonction de l'humidité
*/

// Ajout des librairies
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Définition des constantes
#define LED_RED 2
#define LED_GREEN 4
#define BUTTON 3
#define MOTOR 5
#define MOISTURE_SENSOR A1
#define THR_KNOB A0


LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialisation de l'écran ( optionnel )

// Variables globales
bool currentState = false;
bool buttonClicked = false;



void buttonPressed()
{
  buttonClicked = true;
}

void setup() 
{
  // Configuration de la com série et des I/O
  Serial.begin(115200);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(MOTOR, OUTPUT);
  pinMode(MOISTURE_SENSOR, INPUT);

  // Démarrage écran
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  // Interruption pour le bouton
  attachInterrupt(digitalPinToInterrupt(3), buttonPressed, FALLING);
 
}



void modeSwitch(bool state = !currentState) 
{
  digitalWrite(LED_RED, state);
  digitalWrite(LED_GREEN, !state);
  digitalWrite(MOTOR, state);  
  currentState = state;
}

void loop() 
{
  
  if(buttonClicked)
  {
    buttonClicked = false;
    unsigned long TTL = millis() + 120000; // Timer 2m après l'heure actuelle

    while(!buttonClicked)
    {
      modeSwitch(true);
      if(TTL < millis())
        break;
    }
  }
    // Mesure des valeurs
    int threshold = map(analogRead(THR_KNOB), 0, 1023, 20, 50);
    int moisture = map(analogRead(MOISTURE_SENSOR), 0, 1023, 0, 100);

    // Affichage des infos sur l'écran LCD et la com série
    String thresholdString = "Threshold : " + String(threshold) + " %";
    String moistureString = "Moisture : " + String(moisture) + " %";
    String states = "Motor State : " + String(currentState);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(thresholdString);
    lcd.setCursor(0, 1);
    lcd.print(moistureString);
    Serial.println(thresholdString + " ; " + moistureString + " ; " + states);

    // Si l'humidité est inférieure au seuil choisi avec le potentiomètre, le relai s'active.
    if (moisture < threshold) 
    {
      modeSwitch(true);   
    }
    else
    {
      modeSwitch(false);
    }
    
  
  
    
  // Délai car pas besoin d'un rafraichissement fréquent ( 1 fois toutes les secondes est déjà beaucoup )
  delay(250);
}
