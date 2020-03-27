#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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

	while(s[indice] != '\0' && s[indice] != '\n')
	{
		indice++;
	}

	return indice;
}



/* Fonction prenant en entrée deux char* et un nombre n de caractères */
/* Recopie les n premiers caractères de src dans dest */

void recopier(char *src, char *dest, int nb_recopies)
{
	int index = 0;

	while(index < nb_recopies)
	{
		dest[index] = src[index];
		index++;
	}
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
		if (s[indice]=='\n')
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

	if(s[0] == '\0' || s[0] == '\n') index++;

	while(s[index] != '\0' && s[index] != '\n')
	{
		while(s[index] == ' ') index++;

		if(s[index] != '\0' && s[index] != '\n') nombre_de_mots++;

		while(s[index] != ' ' && s[index] != '\n' && s[index] != '\0') index++;
	}

	return nombre_de_mots;
}


/* Fonction prenant en entrée une chaine de caractères */
/* Retourne le nombre de caractères jusqu'à ' ', '\n' ou '\0' */

int taille_mot(char *mot)
{
	int size = 0;

	while(mot[size] != ' ' && mot[size] != '\n' && mot[size] != '\0') size++;

	return size;
}


/* Cette fonction prend en entrée une chaine de caractères */
/* Retourne le nombre d'espaces après le premier mot de cette chaine */
int nombre_espaces_apres_chaine(char *ligne)
{
	int index = 0;
	int nb_espaces = 0;

	while(ligne[index] != ' ' && ligne[index] != '\n' && ligne[index] != '\0')  index++;
	while(ligne[index] == ' ')
	{
		nb_espaces++;
		index++;
	}

	return nb_espaces;
}


char* trouver_mot_n(char *s, int n, int taille_mot_n)
{
	supprimer_espace(s);

        int indice=0;
        int compteur=0;
        char *pointeur = malloc(1 + taille_mot_n*sizeof(char));
        recopier(s, pointeur, taille_mot_n);

        while (s[indice]!='\0')
        {
                if (s[indice]==' ')
                {
                        compteur++;
                        if(compteur == n)
			{
                                recopier(s+indice+1,pointeur,taille_mot_n);
                        }
                }
                indice++;
        }

        return pointeur;

}

/* Fonction supprimer_espace, prenant en entrée une chaine de caractères
cette fonction remplace dans la chaine de caractères en paramètre
toutes les successions de plusieurs espaces consécutifs par un seul espace
Cette fonction retourne le nombre de suppression */
int supprimer_espace(char *s)
{
	int indice_s=0,indice_tmp=0;
	int suppression=0;

	int taille_s=strlen(s);
	char tmp[taille_s];

	while (s[indice_s]!='\0' && s[indice_s+1]!='\0')
	{
		while (s[indice_s]==' ' && s[indice_s+1]==' ')
		{
			indice_s++;
			suppression++;
		}
		tmp[indice_tmp]=s[indice_s];
		indice_tmp++;
		indice_s++;
	}
	tmp[indice_tmp]='\0';

	strcpy(s,tmp);

	return suppression;
}
