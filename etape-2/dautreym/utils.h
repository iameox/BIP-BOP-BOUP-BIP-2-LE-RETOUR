#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>


int compter_caractere(char *s, char c);

int compter_nombre_de_lignes(char *s);

int taille_ligne(char *s);

void recopier(char *src, char *dest, int nb_recopies);

char* trouver_ligne_n(char *s, int n, int taille_ligne_n);

int supprimer_espace(char *s);

int compter_mots(char *s);

int taille_mot(char *mot);

int nombre_espaces_apres_chaine(char *ligne);

char* trouver_mot_n(char *s, int n, int taille_mot_n);

#endif
