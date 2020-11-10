#include "couleur.h"

#define NB_PRISE 10

struct RGB 
{
	int16_t R;
	int16_t G;
	int16_t B;
};

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void start()
{
    //vérifie si le capteur est bien branché
    Serial.println("Color View Test!");
    
    if (tcs.begin())
    {
        Serial.println("Found sensor");
    }
    else
    {
        Serial.println("No TCS34725 found ... check your connections");
        while (1); // halt!
    }

}


int specter()
{
    struct RGB RGB[100], RGB_tot;
    uint16_t clear, red, green, blue; //Valeur Rouge Vert et Bleu de la surface vue
    int comptage=0;

    RGB_tot.R=0;
    RGB_tot.G=0;
    RGB_tot.B=0;

    
    
    //mesure de la couleur 200 fois
    for(int compteur = 0; compteur < NB_PRISE; compteur++)
    {   
        //prise de mesure
        tcs.setInterrupt(false);        // turn on LED
        delay(60);  // takes 50ms to read
        tcs.getRawData(&red, &green, &blue, &clear);
        tcs.setInterrupt(true);  // turn off LED

        float r, g, b;
        //Amplifie et Transforme les donnéees brutes en valeur int de 0 a 255 dans la structure RGB
        uint32_t sum = clear;
        

        r = red;
        r = (r*2)/sum;

        g = green;
        g=(g*2)/sum;

        b = blue;
        b=(b*2)/sum;

        RGB[compteur].R = r * 256; RGB[compteur].G = g * 256; RGB[compteur].B = b*256;
        

        Serial.print(RGB[compteur].R); Serial.print(" "); 
        Serial.print(RGB[compteur].G);Serial.print(" ");  
        Serial.println(RGB[compteur].B);
    }

    //calcule de la moyenne des valeurs Rouge Vert et Bleu sur 200 mesures
    //calcule de la somme
    for(int compteur = 1; compteur < NB_PRISE;compteur++)
    {
        RGB_tot.R = RGB[compteur].R + RGB_tot.R;
        RGB_tot.G = RGB[compteur].G + RGB_tot.G;
        RGB_tot.B = RGB[compteur].B + RGB_tot.B;
        comptage = compteur;
        
      //  Serial.print(RGB_tot.R); Serial.print("\t"); Serial.print(RGB_tot.G); Serial.print("\t"); Serial.println(RGB_tot.B);
        
    }

    //divise les valeurs RGB par le nombres de valeurs additionner RGB_tot=Moyenne des RGB
    Serial.print(comptage);
    Serial.print("\t");
    RGB_tot.R /= comptage;
    RGB_tot.G /= comptage;
    RGB_tot.B /= comptage;
    Serial.print(RGB_tot.B);


    //Analyse des résultat pour sortir une couleur précises selon des seuille basé sur une moyenne
    if(RGB_tot.B<=150 && RGB_tot.B>=125) //seuil originaux 144=< couleur >=125
    {
        Serial.print("\tLe carton est JAUNE\n");
        return 0; //couleur jaune detecter
    }
    else if(RGB_tot.B<=175 && RGB_tot.B>=155) //173=< couleur >=155
    {
        Serial.print("\tLe carton est ROSE\n");
        return 2; //couleur rose detecter
    }
    else if(RGB_tot.B>=180)       // 188=< couleur >=180
    {
        Serial.print("\tLe carton est BLEU\n");
        return 1; //couleur bleu detecter
    }
    else
    {
        Serial.print("\tCouleur non connu\n");
        return 4;
    }
    
    return 8;
}