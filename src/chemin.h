#include <math.h>
#include "mouvement.h"
#include "couleur.h"
#include "siflet.h"

#define ROUGE 31
#define JAUNE 37
#define BLEU 33

#define NB_ACTION 45

void Chemin_Maker(char action[NB_ACTION],char mesure[NB_ACTION][15]);
void ConversionChemin(char path[],bool Aller_Retour);
void ChoixChemin();