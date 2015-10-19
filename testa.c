#include <stdio.h>
#include "lista.h"

int main(){
	lista_t l;
	l_cria(&l);
	l_insere(&l,3);
	l_insere(&l,3);
	l_insere(&l,1);
	l_insere(&l,4);
	printf("qtd %d\n",l.qtd);
	printf("%d ",l_retira(&l));
	printf("%d ",l_retira(&l));
	printf("%d\n",l_retira(&l));
	printf("qtd %d\n",l.qtd);
	l_insere_ord(&l,3,0);
	l_insere_ord(&l,3,0);
	l_insere_ord(&l,1,0);
	l_insere_ord(&l,4,0);
	l_insere_ord(&l,4,0);
	printf("qtd %d\n",l.qtd);
	printf("%d ",l_retira(&l));
	printf("%d ",l_retira(&l));
	printf("%d\n",l_retira(&l));
	printf("qtd %d\n",l.qtd);
	l_insere_ord(&l,3,1);
	l_insere_ord(&l,3,1);
	l_insere_ord(&l,1,1);
	l_insere_ord(&l,4,1);
	printf("qtd %d\n",l.qtd);
	printf("%d ",l_retira(&l));
	printf("%d ",l_retira(&l));
	printf("%d\n",l_retira(&l));
	printf("qtd %d\n",l.qtd);
}