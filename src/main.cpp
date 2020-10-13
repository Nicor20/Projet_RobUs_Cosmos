/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
#define SETPOINT_30_SPEED 350
#define SETPOINT_50_SPEED 554
#define KP 0.0004
#define KI 0.00002
#define NB_ACTION 25

// L'ensemble des fonctions y ont acces
int32_t encoderRead_Left = 0;
int32_t encoderTotal_Left = 0;
int32_t encoderRead_Right = 0;
int32_t encoderTotal_Right = 0;
int32_t cycleCount = 0;

float speed_Left = 0.30;
float speed_Right = 0.31;


float correctionPID(uint8_t motorSelect, uint32_t setpoint);
void updateSpeed(uint32_t setpoint);
void robus_Avance(float distance);
void robus_TourneGauche(float vitesse, float angle);
void robus_TourneDroite(float vitesse, float angle);
uint32_t conversionDistancePulse(float centimetre);
void Chemin_Maker(char action[NB_ACTION],char mesure[NB_ACTION][15]);
void ConversionChemin(char path[],bool Aller_Retour);

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */
float correctionPID(uint8_t motorSelect, uint32_t setpoint)
{
  float errorI = 0.00;
  float errorP = 0.00;

  if(motorSelect == LEFT)
 {
    errorI = setpoint*cycleCount;
    errorI = errorI - encoderTotal_Left;
    errorI = errorI * KI;

    errorP = setpoint;
    errorP = errorP - encoderRead_Left;
    errorP = errorP * KP;

    return (errorP + errorI);
  }
  else
  {
    errorI = setpoint * cycleCount;
    errorI = errorI - encoderTotal_Right;
    errorI = errorI * KI;
  
    errorP = setpoint;
    errorP = errorP - encoderRead_Right;
    errorP = errorP * KP;

    return (errorP + errorI);
  }
}


void updateSpeed(uint32_t setpoint)
{
  cycleCount++;

  encoderRead_Left = ENCODER_ReadReset(0);
  encoderTotal_Left += encoderRead_Left;

  encoderRead_Right = ENCODER_ReadReset(1);
  encoderTotal_Right += encoderRead_Right;

  speed_Left = speed_Left + correctionPID(0, setpoint);// appelle la fonction PID
  speed_Right = speed_Right + correctionPID(1, setpoint);// appelle la fonction PID

  MOTOR_SetSpeed(0, speed_Left);
  MOTOR_SetSpeed(1, speed_Right);
//Pourrait probablement être enlevé.
  if(cycleCount>500)
  {
    cycleCount = 0;
    encoderTotal_Left = 0;
    encoderTotal_Right = 0;
  }
}

//Fonction avancé. Set point sert au PID
void robus_Avance(float distance)
{
  int32_t distanceParcourue = 0;
  int32_t  pulse = conversionDistancePulse(distance);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);

  float Vitesse = 0;
  float Vitesse_min = 200;
  float Vitesse_max = 1000;

  float h = pulse/2.0;
  float k = ((distance * 2)>Vitesse_max ? Vitesse_max : (distance * 2));
  float a = -k/(h*h);
  float x;
  
  
  //Boucle qui fait avancer le robot tant que la distance parcourue ne dépasse pas la distance voulue
  while(distanceParcourue < pulse)
  {
    x= distanceParcourue;
    Vitesse = a*((x-h)*(x-h))+k;

    updateSpeed((Vitesse < Vitesse_min ? Vitesse_min : Vitesse));
    Serial.println(Vitesse);
    distanceParcourue += encoderRead_Left;
    delay(100);
  }

  speed_Left = 0.00;
  speed_Right = 0.01;
  cycleCount = 0;
  encoderTotal_Left = 0;
  encoderTotal_Right = 0;
  encoderRead_Left = 0;

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
}

//Fonction tourner à gauche où on met l'angle en degré
void robus_TourneGauche(float vitesse, float angle)
{
  uint32_t angleParcouru = 0;
  angle = (angle / 360)*120.64;

  angle = conversionDistancePulse(angle);

  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, 0);

  delay(50);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  while(angleParcouru < angle)
  {
    delay(50);//Délai diminué pour une meilleure précision
    encoderRead_Right = ENCODER_ReadReset(RIGHT);
    angleParcouru += encoderRead_Right;
    Serial.println(angleParcouru);
  }
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

}

//Fonction pour tourner à droite avec l'angle en degré
void robus_TourneDroite(float vitesse, float angle)
{
  uint32_t angleParcouru = 0;
  angle = (angle / 360)*120.64;
  angle = conversionDistancePulse(angle);

  MOTOR_SetSpeed(LEFT, vitesse);
  MOTOR_SetSpeed(RIGHT, 0);

  delay(50);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  while(angleParcouru < angle)
  {
    delay(50);//Delai diminuer pour une meilleure précision
    encoderRead_Left = ENCODER_ReadReset(0);//Pourrait être optimisé en 1 ligne
    angleParcouru += encoderRead_Left;
    Serial.println(angleParcouru);
  }
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

}

//fonctio nqui traduit une distance en cm en pulses pour que ROBUS comprenne
uint32_t conversionDistancePulse(float centimetre)
{
  return (int)((centimetre * 3200)/ 23.939);
}

//Fonction pour la transmission des commandes(Nico)
void Chemin_Maker(char action[NB_ACTION],char mesure[NB_ACTION][15])
{
  int i=0;
  while(action[i]!='\0')
  {
    char test[15];

    int a=0;
    while(mesure[i][a]!='\0')
    {
      test[a] = mesure[i][a];
      a++;
    }
    test[a] ='\0';

    float value = (float)atof(test);

    if(action[i] == 'a')
    {
      //Pour avancer
      robus_Avance(0.50, 500,conversionDistancePulse(value));
    }
    else if(action[i] == 'd')
    {
      //Pour tourner a droite avec 2 roues
      robus_TourneDroite(0.30, value);
    }
    else if(action[i] == 'g')
    {
      //Pour tourner a gauche avec 2 roues
      robus_TourneGauche(0.30, value);
    }
    else if(action[i] == 'u')
    {
      //Pour faire un U turn
    }
    else
    {
      //Pour reculer
    }
    i++;
  }
}

//Fonction pour la lecture du chemin(Nico)
void ConversionChemin(char path[],bool Aller_Retour)
{
  int debut = 0;
  int fin = 0;
  int compteur = 0;
  
  char test[15];
  char Action[NB_ACTION] = {0};
  char valeur[NB_ACTION][15] = {0};

  while(path[fin] != '\0')
  {
    if(path[fin] == '/')
    { 
      bool valid = true;
      int a;
      for(a = debut+2 ; a < fin ; a++)
      {
        if(path[a] == '0'||path[a] == '1'||path[a] == '2'||path[a] == '3'||path[a] == '4'||path[a] == '5'||path[a] == '6'||path[a] == '7'||path[a] == '8'||path[a] == '9'||path[a] == '.')
        {
          test[a-(debut+2)] = path[a];
        }
        else
        {
          valid = false;
          break;
        } 
      }
      test[a-(debut+2)] = '\0';

      if(valid == true && path[debut+1] == ' ')
      {
        bool check = false;
        float value = (float)atof(test);

        if(path[debut] == 'a'|| path[debut] == 'A')
        {
          Serial.print("Avance : ");
          Serial.println(value);
          Action[compteur]='a';
          check = true;
        }
        else if(path[debut] == 'd'|| path[debut] == 'D')
        {
          Serial.print("Tourne a droit : ");
          Serial.println(value);
          Action[compteur]='d';
          check = true;
        }
        else if(path[debut] == 'g'|| path[debut] == 'G')
        {
          Serial.print("Tourne a gauche : ");
          Serial.println(value);
          Action[compteur]='g';
          check = true;
        }
        else if(path[debut] == 'r'|| path[debut] == 'R')
        {
          Serial.print("Recule : ");
          Serial.println(value);
          Action[compteur]='r';
          check = true;
        }
        else
        {
          Serial.println("Erreur : ne commence pas par (a-A),(d-D),(g-G),(r-R)");
        }

        if(check == true)
        {
          int i=0;
          while(test[i] != '\0')
          {
            valeur[compteur][i] = test[i];
            i++;
          }
          valeur[compteur][i] ='\0';
          compteur++;
        }
      }
      else
      {
        Serial.println("Erreur : La valeur n'est pas valide ou le caractere #2 n'est pas un espace");
      }
      debut = fin+1;
    }
    fin++;
  }
  
  int pos2=compteur+1;

  if(Aller_Retour == true)
  {
    Action[compteur]='u';
    valeur[compteur][0]='1';
    valeur[compteur][1]='8';
    valeur[compteur][2]='0';
    valeur[compteur][3]='\0';

    for(int i = (compteur-1);i>=0;i--)
    {
      if(Action[i] == 'd')
      {
        Action[pos2] = 'g';
      }
      else if(Action[i] == 'g')
      {
        Action[pos2] = 'd';
      }
      else
      {
        Action[pos2] = Action[i];
      }

      int a = 0;
      while(valeur[i][a] != '\0')
      {
        valeur[pos2][a] = valeur[i][a];
        a++;
      }
      valeur[pos2][a] ='\0';
      pos2++;
    }
  }

  Action[pos2] = '\0';

  Serial.println("Fin conversion");

  Chemin_Maker(Action,valeur);

  Serial.println("Fin chemin");
}


/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les variables globales

void setup()
{
  //a = Avancer (en cm)
  //d = Tourner à droit (en °)
  //g = Tourner à gauche (en °)
  //r = Reculer (en cm)
  //Ne pas écrire le U turn dans la path
  //Terminer path par un /

  Serial.begin(9600);
  delay(1000);
  BoardInit();

  char path[] = "a 100/";
  ConversionChemin(path,true);

  delay(1500);
}

/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop() {
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(1000);

  // encoderRead_Right = ENCODER_ReadReset(RIGHT);

  // Serial.println(encoderRead_Right);
  // Serial.println("");
}