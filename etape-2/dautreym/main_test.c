#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "api.h"
/* A ENLEVER */
#include "utils.h"
#include "parseur.h"
#include "compare.h"
#include "tree.h"

#define false 0 


/*
typedef struct tree_node
{
    char *name;
    void *adress;
    char *content;
    int is_correct;
    struct tree_node *next_node;
    struct tree_node *children;
} tree_node;


int main(void)
{
	char requete[500] = "GET / HTTP/1.0\nHost: [6:1:E4BA:3:2:E4:222.155.17.250]\nTransfer-encoding: ,          ,       										,    chunked	,																	,\n\n";

	int result = parseur(requete, 500);

	return 0;
}
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
		/* To obtain file size */
        if (fstat(fi, &st) == -1)
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

