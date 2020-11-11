#include <math.h>
#include "mouvement.h"
#include "couleur.h"
#include "siflet.h"
#include "servo.h"



#define NB_ACTION 45

void Chemin_Maker(char action[NB_ACTION],char mesure[NB_ACTION][15]);
void ConversionChemin(char path[],bool Aller_Retour);
void ChoixChemin();