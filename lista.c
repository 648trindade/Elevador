#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

void l_cria(lista_t *l){
	l->qtd 		= 0;
	l->inicio	= NULL;
}

void l_insere(lista_t *l, int valor){
	elem_t *aux;
	elem_t *novo = malloc(sizeof(elem_t));
	novo->valor	= valor;
	if (l->qtd){
		for (aux = l->inicio; aux->prox != NULL; aux = aux->prox);
		aux->prox = novo;
	}
	else	l->inicio = novo;
	l->qtd++;
}

void l_insere_ord(lista_t *l, int valor){
	elem_t *aux, *novo = malloc(sizeof(elem_t));
	if (l->qtd) {
		for (aux = l->inicio; aux != NULL && aux->valor <= valor; aux = aux->prox);
		if (aux==NULL){
			l_insere(l, valor);
			free(novo);
		}
		else{
			novo->valor = aux->valor;
			novo->prox 	= aux->prox;
			aux->valor 	= valor;
			aux->prox 	= novo;
		}
	}
	else {
		l->inicio 	= novo;
		novo->valor	= valor;
	}
	l->qtd++;
}

int l_retira(lista_t *l){
	int valor 	= l->inicio->valor;
	elem_t *aux = l->inicio;
	l->inicio 	= aux->prox;
	free(aux);
	l->qtd--;
	return valor;
}

void l_destroi(lista_t *l){
	while (l->qtd)
		l_retira(l);
}