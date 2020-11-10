#include "siflet.h"

int departSifflet()
{
  float voltage = 0;
    int frequence = 0;
    int half_period =0;
    int analogPinA5 = A5;
    int analogPinA6 = A6;

    int voltageRef = 0;
    int voltageAmbiant = 0;
    int depart = 0;

    int compteur = 0;

  while(depart == 0)
  {
    voltageRef = analogRead(analogPinA5);
    voltageAmbiant = analogRead(analogPinA6);

    if(voltageRef - voltageAmbiant > 50)
    {
      depart = 1;
    }
    compteur++;
    delay(100);
  }
  return 1;
}

void clignoterDEL (int numPin, int delai)
{
  //Mettre cette commande dans le set up absolument! (OUTPUT)
  pinMode(numPin, OUTPUT); // on indique la nature de la PIN (sortie)
  digitalWrite(numPin, HIGH);//Met la switch high
  delay(delai);
  digitalWrite(numPin, LOW);//Met la LED a low
}