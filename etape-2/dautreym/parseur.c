#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "compare.h"

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




void tailles_mots(char *lignes[], int nombre_de_lignes, int *tailles_des_mots[], char *mots[nombre_de_lignes][100])
{
	int index;
	int index_mots;

	int taille_mot_n;
	int nombre_de_mots;
	
	int total_decalage;
	int nb_espaces_entre_mots;

	for (index = 0; index < nombre_de_lignes; index++)
	{
		total_decalage = 0;
		nb_espaces_entre_mots = 0;

		nombre_de_mots = compter_mots(lignes[index]);
		//printf("\n\n La ligne %d : \"%s\" contient %d mots. \n\n",index,lignes[index],nombre_de_mots);

		tailles_des_mots[index] = malloc(nombre_de_mots*sizeof(int));

		for (index_mots = 0; index_mots < nombre_de_mots; index_mots++)
		{
			taille_mot_n = taille_mot(lignes[index] + total_decalage);
			//printf("\n\n Le mot %d de la ligne %d contient %d char. \n\n",index_mots,index,taille_mot_n);

			tailles_des_mots[index][index_mots] = taille_mot_n;

			mots[index][index_mots] = malloc(taille_mot_n*sizeof(char));
			recopier(lignes[index] + total_decalage, mots[index][index_mots], taille_mot_n);

			//printf("\n\n Le mot %d de la ligne %d : \"%s\" contient %d char. \n\n",index_mots,index,mots[index][index_mots],tailles_des_mots[index][index_mots]);

			total_decalage = total_decalage + taille_mot_n;
			nb_espaces_entre_mots = nombre_espaces_apres_chaine(lignes[index] + total_decalage);
			total_decalage = total_decalage + nb_espaces_entre_mots;
		}
	}
}




/* Fonction chargée de parser la première ligne d'une requête */
/* Prend en entrée une chaine de caractères égale à la première ligne de la requête */
/* Retourne un entier 1 si la ligne est correcte syntaxiquement, 0 sinon */
int parseur_ligne_1(char *ligne, int taille_ligne, char *mots[], int tailles_des_mots[])
{
	int to_return = 0;
	int to_return_status;
	int to_return_request;
	if(taille_ligne >= 11)
	{
		printf("\n Début de la comparaison... \n\n");
		to_return_status = (is_status_line(ligne,mots,tailles_des_mots));
		to_return_request = (is_request_line(ligne,mots,tailles_des_mots));
		to_return = (to_return_status || to_return_request);
		if (to_return_request) printf("\n C'est une request_line !!\n\n");
		else if (to_return_status) printf("\n C'est une status line !!\n\n");
		else printf("\n Ce n'est ni une request line ni une status line... \n\n");
	}
	return to_return;
	// La ligne donnée en paramètre est la start_line d'un message HTTP
	// Soit elle commence par HTTP/digit.digit et c'est une status-line
	// Soit elle commence par autre chose et c'est une request-line
		// ATTENTION : vérifier que HTTP ne soit puisse pas être la méthode d'une request-line

	// Supposons d'abord que la ligne start_line est une request-line
		// Son premier mot est sa méthode
			// Son premier mot est son token
			// Chacun de ses caractères (au moins 1) doit être un TCHAR
				// TCHAR = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA 
		
		// Vérifier qu'il y a un seul espace
		
		// Son deuxième mot est sa request target
			// Son deuxième mot est son origin-form
			// Son origin-form est son absolute-path[ "?" query ] 
				// Son absolute-path est au moins un ('/' + segment) (here + means concatenation)
				// Un segment est une combinaison de pchar
					// Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@" 
						// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" 
						// pct-encoded = "%" HEXDIG HEXDIG 
						// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
				// Une query est une combinaison de pchar / "/" / "?"

		// Vérifier qu'il y a un seul espace
		
		// Sa HTTP_version est son troisième mot
			// Les quatre premiers caractères doivent être "HTTP"
			// Le caractère suivant doit être un '/'
			// Le caractère suivant doit être un DIGIT
			// Le caractère suivant doit être un '.'
			// Le caractère suivant doit être un DIGIT

		// Vérifier que la ligne se termine bien par un CRLF

	// Supposons maintenant que la ligne start_line soit une status_line
		// Le premier mot est sa HTTP_version
			// Les quatre premiers caractères doivent être "HTTP"
			// Le caractère suivant doit être un '/'
			// Le caractère suivant doit être un DIGIT
			// Le caractère suivant doit être un '.'
			// Le caractère suivant doit être un DIGIT

		// Vérifier qu'il n'y a qu'un seul espace 

		// Le deuxième mot est son status_code
			// Il ne doit comporter que trois caractères 
			// Chacun de ces caractères doit être un DIGIT

		// Vérifier qu'il n'y a qu'un seul espace

		// Le troisième mot est sa reason_phrase
			// Celle-ci doit être une combinaison de HTAB, d'espaces, de VCHAR ou d'obs-text (=?? %x80-FF)

		// Vérifier que la ligne se termine bine par un CRLF
}







/* Fonction chargée de parser la deuxième ligne d'une requête */
/* Prend en entrée une chaine de caractères égale à la deuxième ligne de la requête */
/* Retourne un entier 1 si la ligne est correcte syntaxiquement, 0 sinon */
int parseur_ligne_2(char *ligne, int taille_ligne, char *mots[], int tailles_des_mots[])
{
	/* On assume que la ligne donnée en paramètre est soit un Host Header, soit un Transfer-Encoding-Header */
	int to_return = 0;
	if (taille_ligne > 0) to_return = is_host_header(ligne);
	if (to_return == 0) printf("\n %s n'est pas un Host Header valide. \n\n",ligne);
	else printf("\n C'est un Host Header valide !!!! \n\n");

    return to_return;
}







int main(void)
{
	//char requete[500] = "GET / HTTP/1.1\nUser-Agent: Wget/1.16 (linux-gnu)\nAccept: */*\nHost: www.google.com\nConnection: Keep-Alive\0";
	char requete[500] = "POST /abc?language=fr HTTP/1.1\nHost: www.google.com\n";
	// "POST /abc/def?language=fr HTTP/1.1\n"
	//char requete[500] = "HTTP/1.1 / HTTP/1.1\n";

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

	// tailles_des_mots[0] contient la liste de la taille des mots de la ligne 0
	// tailles_des_mots[0][0] contient la taille du premier mot de la première ligne
	int *tailles_des_mots[nombre_de_lignes];

	// Possible de faire une liste de taille variable comme [[1], [1,2], [1,2,3]] ?
	// Sinon calculer le maximum des nombre de mots pour toutes les ligne
	// Remplacer le 100 ci-dessous par ce maximum
	char *mots[nombre_de_lignes][100];

	tailles_mots(lignes, nombre_de_lignes, tailles_des_mots, mots);
	int resultat = parseur_ligne_1(lignes[0], tailles_des_lignes[0], mots[0], tailles_des_mots[0]);
	int resultat_2 = parseur_ligne_2(lignes[1], tailles_des_lignes[1], mots[1], tailles_des_mots[1]);

	printf("\n\n\n\n\n\n\n\n\n\n FIN DU CHARGEMENT . AFFICHAGE ... %d %d \n\n\n\n\n\n",resultat,resultat_2);

	int index_mots;
	for (index = 0; index < nombre_de_lignes; index++)
	{
		nombre_de_mots = compter_mots(lignes[index]);
		printf("\n\n La ligne %d : \"%s\" contient %d mots. \n\n",index,lignes[index],nombre_de_mots);
		for (index_mots = 0; index_mots < nombre_de_mots; index_mots++)
		{
			printf("\n\n Le mot %d de la ligne %d : \"%s\" contient %d char. \n\n",index_mots,index,mots[index][index_mots],tailles_des_mots[index][index_mots]);
			free(mots[index][index_mots]);
		}
		free(tailles_des_mots[index]);
		printf("\n\n");
	}


	printf("\n\n It works !! \n\n");
	//printf("\n\n %d %d %d %d \n\n", ((int) -2 < (int) -3), ((int) -2 < (int) -1), ((int) (-2) < (int) 1), ((int) -2 < (int) 257));
/*
	char test[3]; char test_advanced[4];
	test[0] = 0x0D; test_advanced[0] = 0x0D;
	test[1] = 0x0A; test_advanced[1] = 0x0A;
	test[2] = '\0'; test_advanced[2] = 0x09;
	test_advanced[3] = '\0';
	char test_tab = 0x09;
	printf("Real size : %lu\n\n",strlen(test));

	printf("IS CR : %d ; IS LF : %d ; IS HTAB : %d ; IS WSP : %d \n\n",is_cr(test[0]),is_lf(test[1]),is_htab(test_tab),is_wsp(test_tab));
	printf("Is CRLF 'CRLF' : %d\n\n\n Is lwsp 'tab' : %d \n\n\n Is_lwsp 'CRLFtab' : %d\n\n",is_crlf(test),is_htab(test_tab),is_lwsp(test_advanced));
*/
	return 1;
}
