#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "api.h" 
#include "test.h"

#define false 0 


/*
	On a une liste de headers de type char* []
	On a (?) une liste de headers dans le _Token* r (??? c'est une hypothèse)
	On veut s'assurer que chaque header du _Token* r n'apparait qu'une seule et unique fois
	quitte à corriger après pour les exceptions

	Pour chaque current_header de r :
		Pour chaque header_possible dans la headers_list :
			On regarde si current_header == header_possible
				Si c'est le cas et si la valeur du tableau à cet indice n'est pas déjà 1 :
					On met le tableau à l'indice du header posssible à 1
				Sinon on met to_return à 0
*/






int main(int argc,char *argv[])
{
	int res,i,fi;
	char *p=NULL,*addr;

	
        struct stat st;

	if (argc < 3 ) { printf("Usage: httpparser <file> <search>\nAttention <search> is case sensitive\n"); exit(1); }
	/* ouverture du fichier contenant la requête */ 
	if ((fi=open(argv[1],O_RDWR)) == -1) {
                perror("open");
                return false;
        }
        if (fstat(fi, &st) == -1)           /* To obtain file size */
                return false;
        if ((addr=mmap(NULL,st.st_size,PROT_WRITE,MAP_PRIVATE, fi, 0)) == NULL )
                return false;

	// This is a special HACK since identificateur in C can't have character '-'

	if (argc == 3 ) { 
		p=argv[2]; 	
		printf("searching for %s\n",p); 
		while (*p) { 
			if (*p=='-') { *p='_'; }
			p++; 
		}
		p=argv[2]; 	
	}
	// call parser and get results. 
	if (res=parseur(addr,st.st_size)) {
		_Token *r,*tok; 
		void *root=NULL;
		root=getRootTree(); 
		r=searchTree(root,p);



		// Modification exclusive pour tester l'unicité des headers
		int resultat = are_unique_headers(root);
		printf("\n\n Unicité des headers : %d \n\n", resultat);
		// Fin de la modification exclusive



		tok=r; 
		while (tok) {
			int l; 
			char *s; 
			s=getElementValue(tok->node,&l); 
			printf("FOUND [%.*s]\n",l,s);
			tok=tok->next; 
		}
		purgeElement(&r);
		purgeTree(root);
	}
	close(fi);
	return(res); 
}
