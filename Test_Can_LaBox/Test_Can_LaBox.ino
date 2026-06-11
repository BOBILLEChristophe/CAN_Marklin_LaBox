/*

  Test de communication CXAN avec LaBox Locoduino

  Christophe Bobille - Locoduino

  v 0.7.0 - 11/06/26 : Modification de l'identifiant long   
*/

#ifndef ARDUINO_ARCH_ESP32
#error "Select an ESP32 board"
#endif

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "CanMsg.h"
#include "LaBoxCmd.h"

const uint32_t myID = 0x10;  // Identifiant de cet expéditeur

Loco *loco = new Loco;

enum : byte {
  off,
  on
};

LaBoxCmd laBox(myID);

void setup() {
  Serial.begin(115200);
  CanMsg::setup();

  loco->address = 6400;  // Renseigner l'adresse de votre locomotive
}

// Index de fonctions de locomotive
byte fnIdx = 0;

void loop() {
  /*------------------------------------------------------------------
   Serie de commandes envoyées a LaBox pour tests
  --------------------------------------------------------------------*/

  // Power on
  Serial.println("-> Envoi : Power on");
  laBox.setPower(on);
  delay(1000);

  //Test des differentes fonctions du decodeur
  for (fnIdx = 0; fnIdx < 28; fnIdx++) {
    // Activation
    Serial.printf("Fonction %d -> on\n", fnIdx);
    loco->fn[fnIdx] = on;
    laBox.setFunction(loco, fnIdx);
    delay(1000);

    // Desactivation
    Serial.printf("Fonction %d -> off\n", fnIdx);
    loco->fn[fnIdx] = off;
    laBox.setFunction(loco, fnIdx);
    delay(1000);
  }

  // Active les feux et le bruit de la locomotive
  fnIdx = 0;  // Fonction 0
  Serial.printf("-> Envoi : Fonction %d -> inversee\n", fnIdx);
  laBox.toggleFunction(loco, fnIdx);
  delay(100);
  fnIdx = 1;  // Fonction 1
  Serial.printf("-> Envoi : Fonction %d -> inversee\n", fnIdx);
  laBox.toggleFunction(loco, fnIdx);
  delay(100);

  // Klaxon on
  fnIdx = 3;  // Fonction 3
  Serial.printf("-> Envoi : Fonction %d -> on\n", fnIdx);
  loco->fn[fnIdx] = on;
  laBox.setFunction(loco, fnIdx);
  delay(1000);
  // Klaxon off
  Serial.printf("-> Envoi : Fonction %d -> off\n", fnIdx);
  loco->fn[fnIdx] = off;
  laBox.setFunction(loco, fnIdx);
  delay(100);

  // Vers l'avant

  Serial.println("-> Envoi : Roulage avant");
  loco->direction = 1;
  laBox.setDirection(loco);
  delay(100);

  // ... vitesse 500 (sur 1000)
  Serial.println("-> Envoi : Vitesse 50%");
  loco->speed = 500;
  laBox.setSpeed(loco);
  delay(10000);

  Serial.println("-> Envoi : Vitesse 0%");
  loco->speed = 0;
  laBox.setSpeed(loco);
  delay(10000);

  // Vers l'arriere
  Serial.println("-> Envoi : Roulage arriere");
  loco->direction = 2;
  laBox.setDirection(loco);
  delay(100);

  Serial.println("-> Envoi : Vitesse 50%");
  loco->speed = 500;
  laBox.setSpeed(loco);
  delay(10000);

  Serial.println("-> Envoi : Vitesse 0%");
  loco->speed = 0;
  laBox.setSpeed(loco);
  delay(10000);

  // emergency stop (only this loco)
  Serial.printf("-> Envoi : Emergency loco %d", loco->address);
  laBox.emergencyOne(loco);
  delay(1000);

  // emergency stop (all locos)
  Serial.println("-> Envoi : Emergency (all))");
  laBox.emergency();
  delay(1000);

  // Desactive les feux et le bruit de la locomotive
  fnIdx = 0;  // Fonction 0
  Serial.printf("-> Envoi : Fonction %d -> inversee\n", fnIdx);
  laBox.toggleFunction(loco, fnIdx);
  delay(100);
  fnIdx = 1;  // Fonction 1
  Serial.printf("-> Envoi : Fonction %d -> inversee\n", fnIdx);
  laBox.toggleFunction(loco, fnIdx);
  delay(100);

  //power off
  Serial.println("-> Envoi : Power off");
  laBox.setPower(off);
  delay(5000);
}
