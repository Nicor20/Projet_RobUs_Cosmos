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

// L'ensemble des fonctions y ont acces
int32_t encoderRead_Left = 0;
int32_t encoderTotal_Left = 0;
int32_t encoderRead_Right = 0;
int32_t encoderTotal_Right = 0;
int32_t cycleCount = 0;

float speed_Left = 0.30;
float speed_Right = 0.31;

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
void robus_Avance(float vitesse, uint32_t setpoint, uint32_t distance)
{
 
  uint32_t distanceParcourue = 0;

  MOTOR_SetSpeed(LEFT, vitesse);
  MOTOR_SetSpeed(RIGHT, vitesse + 0.01);
  delay(50);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  //Boucle qui fait avancer le robot tant que la distance parcourue ne dépasse pas la distance voulue
  while(distanceParcourue < distance)
  {
    delay(100);
    updateSpeed(setpoint);//PID
    distanceParcourue += encoderRead_Left;
    Serial.println(distanceParcourue);
  }

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
}

//Fonction tourner à gauche où on met l'angle en degré
void robus_TourneGauche(float vitesse, float angle)
{

  uint32_t angleParcouru = 0;
  float temp;
  angle = (angle / 360)*120.64;
  temp = angle;
  angle = conversionDistancePulse(temp);

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
  float temp;
  angle = (angle / 360)*120.64;
  temp = angle;
  angle = conversionDistancePulse(temp);

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
  float distance = 0;

  distance = (centimetre * 3200);
  distance = distance / 23.939;

  return (int)distance;
}

//fonction d'appellation des directions (Mrs N)
void Lecture_Chemin(char path[])
{
  int i=0;
  int y=2;
  int a;
  char valeur[100];

  while(path[i] != '\0')
  {
    if(path[i] == '/')
    { 
      int True = 1;
      for(a = y ; a < i ; a++)
      {
        if(path[a] == '0'||path[a] == '1'||path[a] == '2'||path[a] == '3'||path[a] == '4'||path[a] == '5'||path[a] == '6'||path[a] == '7'||path[a] == '8'||path[a] == '9'||path[a] == '.')
        {
          valeur[a-y] = path[a];
        }
        else
        {
          True = 0;
          break;
        } 
      }
      valeur[a-y] = '\0';

      if(True == 1 && path[y-1] == ' ')
      {
        float value = (float)atof(valeur);
        if(path[y-2] == 'a' || path[y-2] == 'A')
        {
          //Pour avancer
          Serial.print("Avance : ");
          Serial.println(value);
          robus_Avance(0.30, 350,conversionDistancePulse(value));
        }
        else if(path[y-2] == 'd' || path[y-2] == 'D')
        {
          //Pour tourner a droite avec une roue
          Serial.print("Tourne a droit : ");
          Serial.println(value);
          robus_TourneDroite(0.30, value);
        }
        else if(path[y-2] == 'g'||path[y-2] == 'G')
        {
          //Pour tourner a gauche avec une roue
          Serial.print("Tourne a gauche : ");
          Serial.println(value);
          robus_TourneGauche(0.30, value);
        }
        else if(path[y-2] == 'r'||path[y-2] == 'R')
        {
          
        }
        else if(path[y-2] == 'x'||path[y-2] == 'X')
        {
          //Pour tourner a droite avec deux roues
          Serial.print("U turn a droit : ");
          Serial.println(value);
        }
        else if(path[y-2] == 'y'||path[y-2] == 'Y')
        {
          //Pour tourner a gauche avec deux roues
          Serial.print("U turn a gauche : ");
          Serial.println(value);
        }
        else
        {
          Serial.println("Erreur : ne commence pas par (a-A),(d-D),(g-G),(r-R),(x-X),(y-Y)");
        }
        
      }
      else
      {
        Serial.println("Erreur : LA valeur n'est pas valide ou le caractere #2 n'est pas un espace");
      } 
      y=i+3;
    }
    i++;
  }
}

/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les variables globales

void setup(){

  //variables
  uint32_t i = 100000;

  Serial.begin(9600);
  //a = Avancer (en cm)
  //d = Tourner à droit (en °)
  //g = Tourner à gauche (en °)
  //r = Reculer (en cm)
  //x = Uturn droit
  //y = uturn gauche

  char path[] = "a 50/d 45/g 40.5/r 160/x 45/y 45/";
  //Lecture_Chemin(path);
  delay(1000);
  BoardInit();
  delay(1500);

//Parcours
  robus_Avance(0.30, 350,conversionDistancePulse(110));
    delay(1000);
  robus_TourneGauche(0.30, 88);
    delay(1000);
  robus_Avance(0.30, 350,conversionDistancePulse(68));
    delay(1000);
  robus_TourneDroite(0.30, 88);
    delay(1000);
  robus_Avance(0.30, 350,conversionDistancePulse(85));
    delay(1000);
  robus_TourneDroite(0.30, 39);
    delay(1000);
  robus_Avance(0.30, 350,conversionDistancePulse(175));
    delay(1000);
  robus_TourneGauche(0.30, 90);
  delay(1000);
 robus_Avance(0.30, 350, conversionDistancePulse(60));
  delay(1000);
   robus_TourneDroite(0.30, 45);
   delay(1000);
   robus_Avance(0.30, 350, conversionDistancePulse(95));
  robus_TourneDroite(0.30, conversionDistancePulse(180));

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