#include "mouvement.h"

#define SETPOINT_30_SPEED 350
#define SETPOINT_50_SPEED 554
#define KP 0.0004
#define KI 0.00002

int32_t encoderRead_Left = 0;
int32_t encoderTotal_Left = 0;
int32_t encoderRead_Right = 0;
int32_t encoderTotal_Right = 0;
int32_t cycleCount = 0;

float speed_Left = 0.30;
float speed_Right = 0.31;

//Fonction contenant le PID
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


//Update la vitesse du robot
void updateSpeed(uint32_t setpoint)
{
  cycleCount++;

  encoderRead_Left = ENCODER_ReadReset(0);
  encoderTotal_Left += encoderRead_Left;

  encoderRead_Right = ENCODER_ReadReset(1);
  encoderTotal_Right += encoderRead_Right;

  speed_Right = speed_Right + correctionPID(1, setpoint);// appelle la fonction PID
  speed_Left = speed_Left + correctionPID(0, setpoint);// appelle la fonction PID

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


//Fonction pour avance avec une accélération
void robus_Avance(float distance)
{
  int32_t distanceParcourue = 0;
  int32_t  pulse = conversionDistancePulse(distance);
  Serial.print("Pulse : ");
  Serial.println(pulse);
  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  
  float Vitesse = 0;
  float Vitesse_max = 1000;
  float h = pulse/2.0;
  float diviseur = 20.0;
  float k = ((pulse/diviseur)>Vitesse_max ? Vitesse_max : pulse/diviseur);
  float a = -k/(h*h);
  float x;

  while(distanceParcourue < pulse)
  {
    cycleCount = 0;
    encoderTotal_Left = 0;
    encoderTotal_Right = 0;
    encoderRead_Left = 0;
    encoderRead_Right = 0;

    x = distanceParcourue;
    Vitesse = a*((x-h)*(x-h))+k;

    if(distanceParcourue < pulse/2)
    {
      Vitesse = Vitesse < 150 ? 150 : Vitesse;
    }
    else
    {
      Vitesse = Vitesse < 15 ? 15 : Vitesse;
    }
    updateSpeed(Vitesse);

    Serial.println(Vitesse);
    distanceParcourue += encoderRead_Left;
    delay(100);
  }

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
}


//Fonction tourner à gauche où on met l'angle en degré
void robus_TourneGauche(float angle)
{
  uint32_t angleParcouru = 0;
  uint32_t pulse = conversionAngleGauche(angle);
  Serial.print("Pulse : ");
  Serial.println(pulse);

  float Vitesse = 0;
  float Vitesse_max = 1000;
  float h = pulse/2.0;
  float diviseur = 20.0;
  float k = ((pulse/diviseur)>Vitesse_max ? Vitesse_max : pulse/diviseur);
  float a = -k/(h*h);
  float x;

  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  while(angleParcouru < pulse)
  {
    x = angleParcouru;
    Vitesse = a*((x-h)*(x-h))+k;

    if(angleParcouru < pulse/2)
    {
      Vitesse = Vitesse/1000.0 < 0.1 ? 0.1 : Vitesse/1000.0;
    }
    else
    {
      Vitesse = Vitesse/1000.0 < 0.1 ? 0.1 : Vitesse/1000.0;
    }
    

    MOTOR_SetSpeed(1,Vitesse);
    Serial.println(angleParcouru);

    encoderRead_Left = ENCODER_ReadReset(1);
    angleParcouru += encoderRead_Left;

    delay(100);
  }

  MOTOR_SetSpeed(0,0);
  MOTOR_SetSpeed(1,0);
}


//Fonction pour tourner à droite avec l'angle en degré
void robus_TourneDroite(float angle)
{
  uint32_t angleParcouru = 0;
  uint32_t pulse = conversionAngleDroit(angle);
  Serial.print("Pulse : ");
  Serial.println(pulse);

  float Vitesse = 0;
  float Vitesse_max = 1000;
  float h = pulse/2.0;
  float diviseur = 20.0;
  float k = ((pulse/diviseur)> Vitesse_max ? Vitesse_max : pulse/diviseur);
  float a = (-1*k)/(h*h);
  float x;

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);

  while(angleParcouru < pulse)
  {
    x = angleParcouru;

    Vitesse = a*((x-h)*(x-h))+k;

    if(angleParcouru< pulse/2)
    {
      Vitesse = Vitesse/1000.0 <0.1 ? 0.1 : Vitesse/1000.0;
    }
    else
    {
      Vitesse = Vitesse/1000.0 < 0.1 ? 0.1 : Vitesse/1000.0;
    }

    MOTOR_SetSpeed(0, Vitesse);
    Serial.println(angleParcouru);

    encoderRead_Left = ENCODER_ReadReset(0);
    angleParcouru += encoderRead_Left;

    delay(50);
  }
  MOTOR_SetSpeed(0,0);
  MOTOR_SetSpeed(1,0);
}


//Fonction pour tourner à 2 roues
void robus_Uturn()
{
  int32_t angle = 3715;

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  delay(500);
  int32_t angleParcouru = 0;

  MOTOR_SetSpeed(RIGHT, 0.3);
  MOTOR_SetSpeed(LEFT, -0.3);

  //delay(50);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  while(angleParcouru < angle)
  {
    delay(5);//Délai diminué pour une meilleure précision
    encoderRead_Right = ENCODER_ReadReset(RIGHT);
    angleParcouru += encoderRead_Right;
    //Serial.println(angleParcouru);
  }
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
  delay(500);
}


//Convertie une distance en pulse
uint32_t conversionDistancePulse(float centimetre)
{
  return (int)((centimetre * 3200)/ 23.939);
}


//Convertie un angle en pulse
uint32_t conversionAngleDroit(float degre)
{
  float rayon_tour = 18.8;//en cm
  float circonference_tour = 2.0 * PI * rayon_tour;
  float distance = (degre/360.0) * circonference_tour;

  float diametre_roue = 7.62;
  float circonference_roue = PI * diametre_roue;

  int32_t pulse = ((distance/circonference_roue) * 3200);
  return pulse;
}

uint32_t conversionAngleGauche(float degre)
{
  float rayon_tour = 18.4;//en cm
  float circonference_tour = 2.0 * PI * rayon_tour;
  float distance = (degre/360.0) * circonference_tour;

  float diametre_roue = 7.62;
  float circonference_roue = PI * diametre_roue;

  int32_t pulse = ((distance/circonference_roue) * 3200);
  return pulse;
}