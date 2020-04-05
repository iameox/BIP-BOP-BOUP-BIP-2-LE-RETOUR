#ifndef __PARSEUR_H__
#define __PARSEUR_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "compare.h"



/* Fonction qui prend une chaine de caractères (requête) en entrée */
/* Remplis lignes[index] donnée en paramètre par la ligne n° index de la requête */
/* Retourne une liste d'entiers naturels égaux à la taille de
chacune des lignes de la requête */
void tailles_lignes(char *s, char *lignes[], int nombre_de_lignes, int *taille_des_lignes);


void tailles_mots(char *lignes[], int nombre_de_lignes, int *tailles_des_mots[], char *mots[nombre_de_lignes][500]);


#endif