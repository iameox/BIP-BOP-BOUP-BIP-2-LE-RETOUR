#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "compare.h"


/* Fonction prenant en entrée une chaine de caractères, et un caractère.
Cette fonction compte et retourne le nombre d’occurrences du caractère
dans la chaine. */
int compter_caractere(char *s, char c)
{
        int indice=0;
        int occurence=0;

        while (s[indice]!='\0')
        {
                if (s[indice]==c) occurence++;
                indice++;
        }

        return occurence;
}


int compter_nombre_de_lignes(char *s)
{
	return compter_caractere(s, '\n');
}


/* Fonction qui compte le nombre de caractères d'une chaine passée en paramètre */
/* Renvoie un entier égal au nombre de caractères jusqu'au premier '\n' ou '\0' */
int taille_ligne(char *s)
{
	int indice = 0;
	int taille = 0;

	while(s[indice] != '\0' && s[indice] != '\n' && s[indice] != '\r')
	{
		if (s[indice] != '\r') taille++;
		indice++;
	}

	return taille;
}



/* Fonction prenant en entrée deux char* et un nombre n de caractères */
/* Recopie les n premiers caractères de src dans dest */

void recopier(char *src, char *dest, int nb_recopies)
{
	int index = 0;

	while(index < nb_recopies)
	{
		if (src[index] != '\r') dest[index] = src[index];
		index++;
	}
	dest[index] = '\0';
}


/* Fonction prenant en entrée une chaine de caractères et un entier naturel non nul,
cette fonction retourne un pointeur vers la n-ième occurrence
du caractère '\n' dans la chaine, NULL si non trouvé */

char* trouver_ligne_n(char *s, int n, int taille_ligne_n)
{
	int indice=0;
	int compteur=0;
	char *pointeur = malloc(1 + taille_ligne_n*sizeof(char));
	recopier(s, pointeur, taille_ligne_n);

	while (s[indice]!='\0')
	{
		if (s[indice]=='\n' || s[indice] == '\r')
		{
			compteur++;
			if(compteur == n)
			{
				recopier(s+indice+1,pointeur,taille_ligne_n);
			}
		}
		indice++;
	}

	return pointeur;
}


/* Fonction prenant en entrée une chaine de caractères */
/* Compte le nombre de mots (chaines séparées par des ' ')
jusqu'à la fin de la chaine ('\n' ou '\0') */

int compter_mots(char *s)
{
	int nombre_de_mots = 0;
	int index = 0;

	//if(s[0] == '\0' || s[0] == '\n') index++;

	while(s[index] != '\0' && s[index] != '\n' && s[index] != '\r')
	{
		while(s[index] == ' ') index++;

		if(s[index] != '\0' && s[index] != '\n' && s[index] != '\r') nombre_de_mots++;

		while(s[index] != '\0' && s[index] != ' ') index++;
		//while(s[index] != '\0' && s[index] != '\n' && s[index] != '\r') index++;
	}

	return nombre_de_mots;
}


/* Fonction prenant en entrée une chaine de caractères */
/* Retourne le nombre de caractères jusqu'à ' ', '\n' ou '\0' ou '\r' */

int taille_mot(char *mot)
{
	int size = 0;

	while(mot[size] != '\0' && mot[size] != '\n' && mot[size] != ' ' && mot[size] != '\r') size++;

	return size;
}


/* Cette fonction prend en entrée une chaine de caractères */
/* Retourne le nombre d'espaces après le premier mot de cette chaine */
int nombre_espaces_apres_chaine(char *ligne)
{
	int index = 0;
	int nb_espaces = 0;

	while(ligne[index] != ' ' && ligne[index] != '\n' && ligne[index] != '\0' && ligne[index] != '\r')  index++;
	while(ligne[index] == ' ')
	{
		nb_espaces++;
		index++;
	}

	return nb_espaces;
}


