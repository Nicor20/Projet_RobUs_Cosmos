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
  delay(1000);
  /*
  pinMode(31, OUTPUT);
  digitalWrite(31, LOW);
  pinMode(33, OUTPUT);
  digitalWrite(33, LOW);
  pinMode(37, OUTPUT);
  digitalWrite(37, LOW);
  */
  BoardInit();
}

void loop() 
{
  if(departSifflet()==1)
  {
    delay(1000);
    //Pour le robot balle
    // 1- Avance jusqu'a la ligne noir
    // 2- recule un peu
    // 3- Tourne à droite de 90°
    // 4- avance jusqu'a ce qu'il trouve l'autre ligne noir
    // 5- Se rend jusqu'a la couleur
    // 6- Scan la couleur
    // 7- Effectu le chemin selon la couleur scanner
    char path[] = "a 90/l 00/";
    ConversionChemin(path,false);
  }
  else
  {
    Serial.println("pas de siflet");
  }
  
}