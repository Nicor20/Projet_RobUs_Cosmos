/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/
#include <Arduino.h>
#include "mouvement.h"
#include "chemin.h"
#include "siflet.h"
#include "couleur.h"
#include "servo.h"




void setup()
{
  //a = Avancer (en cm)
  //d = Tourner à droit (en °)
  //g = Tourner à gauche (en °)
  //r = Reculer (en cm)
  //u = Uturn
  //o = ouvre la pince
  //c = ferme la pince
  //l = Lecture de la couleur
  Serial.begin(9600);

  start();
  ServoStart();
  delay(1000);
  
  pinMode(ROUGE, OUTPUT);
  digitalWrite(ROUGE, LOW);
  pinMode(BLEU, OUTPUT);
  digitalWrite(BLEU, LOW);
  pinMode(JAUNE, OUTPUT);
  digitalWrite(JAUNE, LOW);
  
  BoardInit();
}

void loop() 
{
  /*
  if(ROBUS_IsBumper(3)==1)
  {
    delay(1000);
    char path[] = "d 90/";
    ConversionChemin(path,false);
  }
*/



  if(departSifflet()==1)
  {
    clignoterDEL(JAUNE,500);
    clignoterDEL(ROUGE,500);
    clignoterDEL(BLEU,500);

    clignoterDEL(JAUNE,500);
    clignoterDEL(ROUGE,500);
    clignoterDEL(BLEU,500);

    clignoterDEL(JAUNE,500);
    clignoterDEL(ROUGE,500);
    clignoterDEL(BLEU,500);

    clignoterDEL(JAUNE,500);

    //Pour le robot balle
    // 1- Avance jusqu'a la ligne noir
    // 2- recule un peu
    // 3- Tourne à droite de 90°
    // 4- avance jusqu'a ce qu'il trouve l'autre ligne noir
    // 5- Se rend jusqu'a la couleur
    // 6- Scan la couleur
    // 7- Effectu le chemin selon la couleur scanner

    char path[] = "o 00/a 10/g 90/a 97/l 00/";
    ConversionChemin(path,false);
  }
  else
  {
    Serial.println("pas de siflet");
  }
  
}