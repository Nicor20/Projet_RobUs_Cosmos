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
#include <Arduino.h>
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

  //Serial.println("START");
  //Serial.println(cycleCount);

  if(motorSelect == LEFT)
 {
    errorI = setpoint*cycleCount;
    errorI = errorI - encoderTotal_Left;
        //Serial.println(errorI);

    errorI = errorI * KI;

    // Serial.println(setpoint*cycleCount);
    // Serial.println(encoderTotal_Left);
    // Serial.println(errorI);
    // Serial.println("");
  
    errorP = setpoint;
    errorP = errorP - encoderRead_Left;
    //Serial.println(errorP);
    errorP = errorP * KP;

// Serial.println(setpoint);
// Serial.println(encoderRead_Left);
// Serial.println(errorP);
//     Serial.println("");


     //Serial.println(errorP+errorI);

// Serial.println("END");
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

  encoderRead_Left = ENCODER_ReadReset(LEFT);
  encoderTotal_Left += encoderRead_Left;

  encoderRead_Right = ENCODER_ReadReset(RIGHT);
  encoderTotal_Right += encoderRead_Right;

  speed_Left = speed_Left + correctionPID(LEFT, setpoint);
  speed_Right = speed_Right + correctionPID(RIGHT, setpoint);

  Serial.println(speed_Left);
  Serial.println(speed_Right);

  
  MOTOR_SetSpeed(LEFT, speed_Left);
  MOTOR_SetSpeed(RIGHT, speed_Right);

  if(cycleCount>500)
  {
    cycleCount = 0;
    encoderTotal_Left = 0;
    encoderTotal_Right = 0;
  }
}

void robus_Avance(float vitesse, uint32_t setpoint, uint32_t distance)
{
 
  uint32_t distanceParcourue = 0;

  MOTOR_SetSpeed(LEFT, vitesse);
  MOTOR_SetSpeed(RIGHT, vitesse + 0.01);
  delay(50);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);

  while(distanceParcourue < distance)
  {
    delay(100);
    updateSpeed(setpoint);
    distanceParcourue += encoderTotal_Left;
    if(distanceParcourue > 100000)
    {
      distanceParcourue = 0;
    }
  }

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
}

void robus_Tourne(float vitesse, uint32_t setpoint, uint32_t angle)
{
    uint32_t angleParcouru = 0;

      MOTOR_SetSpeed(LEFT, vitesse);
      MOTOR_SetSpeed(RIGHT, -(vitesse + 0.01));
}


/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les variables globales

void setup(){

  Serial.begin(9600);
  delay(1000);
  BoardInit();
  delay(1500);

  robus_Avance(0.30, 350, 1000000);

  //MOTOR_SetSpeed(LEFT, 0.30);
  //MOTOR_SetSpeed(RIGHT, 0.309);

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