#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


/* Fonction qui prend une chaine de caractères (requête) en entrée */
/* Remplis lignes[index] donnée en paramètre par la ligne n° index de la requête */
/* Retourne une liste d'entiers naturels égaux à la taille de
chacune des lignes de la requête */
void tailles_lignes(char *s, char *lignes[], int nombre_de_lignes, int *taille_des_lignes)
{
	int taille_ligne_n = 0;
	int total_decalage = 0;
	int index;

	char *ligne_tmp;

	for(index = 0; index < nombre_de_lignes; index++)
	{
		total_decalage = total_decalage + taille_ligne_n;

		taille_ligne_n = taille_ligne(s + total_decalage + index);

		ligne_tmp = trouver_ligne_n(s, index, taille_ligne_n);
		lignes[index] = ligne_tmp;
		//printf("\n\n Ligne temporaire numéro %d : \"%s\" \n\n",index,ligne_tmp);

		taille_des_lignes[index] = taille_ligne_n;
	}
}


/* Fonction chargée de parser la première ligne d'une requête */
/* Prend en entrée une chaine de caractères égale à la première ligne de la requête */
/* Retourne un entier 1 si la ligne est correcte syntaxiquement, 0 sinon */
int parseur_ligne_1(char *ligne_1)
{
	return 0;
}


/* Fonction chargée de parser la deuxième ligne d'une requête */
/* Prend en entrée une chaine de caractères égale à la deuxième ligne de la requête */
/* Retourne un entier 1 si la ligne est correcte syntaxiquement, 0 sinon */
int parseur_ligne_2(char *ligne_2)
{
        return 0;
}

int main(void)
{
	char requete[500] = "GET / HTTP/1.1\nUser-Agent: Wget/1.16 (linux-gnu)\nAccept: */*\nHost: www.google.com\nConnection: Keep-Alive\0";

	int nombre_de_lignes = compter_nombre_de_lignes(requete);
	int nombre_de_mots;

	int *tailles_des_lignes;
	tailles_des_lignes = malloc(nombre_de_lignes*sizeof(int));

	char *lignes[nombre_de_lignes];
	int index;
	for(index = 0; index < nombre_de_lignes; index++)
	{
		lignes[index] = malloc(sizeof(char*));
	}

	tailles_lignes(requete,lignes,nombre_de_lignes,tailles_des_lignes);

	//free(lignes);
	//free(taille_des_lignes);

	//printf("\n\n It works 1 !! \n\n");


	int index_mots;
	int taille_mot_n;
	int total_decalage;
	int nb_espaces_entre_mots;

	// taille_des_mots[0] contient la liste de la taille des mots de la ligne 0
	// taille_des_mots[0][0] contient la taille du premier mot de la première ligne
	int *taille_des_mots[nombre_de_lignes];

	// Possible de faire une liste de taille variable comme [[1], [1,2], [1,2,3]] ?
	// Sinon calculer le maximum des nombre de mots pour toutes les ligne
	// Remplacer le 100 ci-dessous par ce maximum
	char *mots[nombre_de_lignes][100];

	for (index = 0; index < nombre_de_lignes; index++)
	{
		total_decalage = 0;
		nb_espaces_entre_mots = 0;

		nombre_de_mots = compter_mots(lignes[index]);
		printf("\n\n La ligne %d : \"%s\" contient %d mots. \n\n",index,lignes[index],nombre_de_mots);

		for (index_mots = 0; index_mots < nombre_de_mots; index_mots++)
		{
			taille_mot_n = taille_mot(lignes[index] + total_decalage);
			//printf("\n\n Le mot %d de la ligne %d contient %d char. \n\n",index_mots,index,taille_mot_n);

			taille_des_mots[index] = malloc(nombre_de_mots*sizeof(int));
			taille_des_mots[index][index_mots] = taille_mot_n;

			mots[index][index_mots] = malloc(taille_mot_n*sizeof(char));
			recopier(lignes[index] + total_decalage, mots[index][index_mots], taille_mot_n);

			printf("\n\n Le mot %d de la ligne %d : \"%s\" contient %d char. \n\n",index_mots,index,mots[index][index_mots],taille_des_mots[index][index_mots]);

			total_decalage = total_decalage + taille_mot_n;
			nb_espaces_entre_mots = nombre_espaces_apres_chaine(lignes[index] + total_decalage);
			total_decalage = total_decalage + nb_espaces_entre_mots;
		}

		for (index_mots = 0; index_mots < nombre_de_mots; index_mots++) printf("\n\n %d %s \n\n",taille_des_mots[index][index_mots],mots[index][index_mots]);
	}

	printf("\n\n\n\n\n\n\n\n\n\n FIN DU CHARGEMENT . AFFICHAGE ... \n\n\n\n\n\n");


	for (index = 0; index < nombre_de_lignes; index++)
	{
		nombre_de_mots = compter_mots(lignes[index]);
		printf("\n\n La ligne %d : \"%s\" contient %d mots. \n\n",index,lignes[index],nombre_de_mots);
		for (index_mots = 0; index_mots < nombre_de_mots; index_mots++)
		{
			printf("\n\n Le mot %d de la ligne %d : \"%s\" contient %d char. \n\n",index_mots,index,mots[index][index_mots],taille_des_mots[index][index_mots]);
		}
	}

	printf("\n\n It works 2 !! \n\n");
	return 1;
}
