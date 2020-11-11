#include "chemin.h"

//Fonction pour la décision selon la couleur(Nico)
void ChoixChemin()
{
  int couleur = specter();
  //trouver la couleur
  
  if(couleur == 0)
  {
    //Jaune
    clignoterDEL(JAUNE,1000);
    
    char path[] = "a 120/c 00/a 82/d 90/a 18/o 00/";
    ConversionChemin(path,false);
  }
  else if(couleur == 1)
  {
    //Bleu
    clignoterDEL(BLEU,1000);
    char path[] = "a 120/c 00/a 159/g 90/a 18/o 00/";
    ConversionChemin(path,false);
  }
  else if(couleur == 2)
  {
    //Rose
    clignoterDEL(ROUGE,1000);
    char path[] = "a 120/c 00/a 232/d 90/a 18/o 00/";
    ConversionChemin(path,false);
  }
  else
  {
    for(int i=0; i<20;i++)
    {
      clignoterDEL(JAUNE,100);
      clignoterDEL(BLEU,100);
      clignoterDEL(ROUGE,100);
    }
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
        else if(path[debut] == 'u'|| path[debut] == 'U')
        {
          Serial.print("U turn : ");
          Serial.println(value);
          Action[compteur]='u';
          check = true;
        }
        else if(path[debut] == 'o'|| path[debut] == 'O')
        {
          //ouverture de la pince
          Serial.println("Ouverture de la pince");
          Action[compteur]='o';
          check = true;

        }
        else if(path[debut] == 'c'|| path[debut] == 'C')
        {
          //Fermeture de la pince
          Serial.println("Fermeture de la pince");
          Action[compteur]='c';
          check = true;
          
        }
        else if(path[debut] == 'l'|| path[debut] == 'L')
        {
          //Lecture de la couleur
          Serial.println("Lecture de la couleur");
          Action[compteur]='l';
          check = true;
          
        }
        else
        {
          Serial.println("Erreur : ne commence pas par (a-A),(d-D),(g-G),(r-R),(u-U),(o-O),(c-C),(l-L)");
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
  
  int pos2=compteur;

  if(Aller_Retour == true)
  {
    Action[pos2]='u';
    valeur[pos2][0]='1';
    valeur[pos2][1]='8';
    valeur[pos2][2]='0';
    valeur[pos2][3]='\0';
    pos2++;

    for(int i = (pos2-2);i>=0;i--)
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

  Serial.println("Fin conversion");

  Chemin_Maker(Action,valeur);

  Serial.println("Fin chemin");
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
      robus_Avance(value);
    }
    else if(action[i] == 'd')
    {
      //Pour tourner a droite avec 2 roues
      robus_TourneDroite(value);
    }
    else if(action[i] == 'g')
    {
      //Pour tourner a gauche avec 2 roues
      robus_TourneGauche(value);
    }
    else if(action[i] == 'u')
    {
      //Pour faire un U turn
      robus_Uturn();
    }
    else if(action[i]=='r')
    {
      //Pour reculer

    }
    else if(action[i]=='o')
    {
      //Ouverture de la pince
      ServoOpen();
    }
    else if(action[i]=='c')
    {
      //Fermeture de la pince
      ServoClose();
    }
    else if(action[i]=='l')
    {
      //Lecture de la couleur
      ChoixChemin();
    }
    else
    {
      MOTOR_SetSpeed(0,0);
      MOTOR_SetSpeed(1,0);
    }
    i++;
  }
}
