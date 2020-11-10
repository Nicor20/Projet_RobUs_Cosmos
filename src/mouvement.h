#include <LibRobus.h>
#include <math.h>

void robus_Avance(float distance);
void robus_TourneGauche(float angle);
void robus_TourneDroite(float angle);
uint32_t conversionDistancePulse(float centimetre);
uint32_t conversionAnglePulse(float degre);
float correctionPID(uint8_t motorSelect, uint32_t setpoint);
void updateSpeed(uint32_t setpoint);
void robus_Uturn();