#include "lista.h"

void l_cria(lista_t *l){
	l->qtd = 0;
	l->elem_t = NULL;
}

void l_insere(lista_t *l, int valor){
	elem_t *aux, *novo = malloc(sizeof(elem_t));
	novo->valor = valor;
	if (!(l->qtd))
		l->inicio = novo;
	else{
		for (aux = l->inicio; aux->prox != NULL; aux = aux->prox);
		aux->prox = novo;
	}
	l->qtd++;
}

void l_insere_ord(lista_t *l, int valor){
	elem_t *aux, *novo = malloc(sizeof(elem_t));
	if (!(l->qtd)){
		l->inicio = novo;
		novo->valor = valor;
	}
	else {
		for (aux = l->inicio; aux != NULL && aux->valor <= valor; aux = aux->prox);
		if (aux==NULL){
			l_insere(l, valor);
			free(novo);
			return;
		}
		else{
			novo->valor = aux->valor;
			novo->p,rox = aux->prox;
			aux->valor = valor;
			aux->prox = novo;
		}
	}
	l->qtd++;
}

int l_retira(lista_t *l){
	int valor = l->inicio->valor;
	elem_t *aux = l->inicio;
	l->inicio = aux->prox;
	free(aux);
	return valor;
}

void l_destroi(lista_t *l){
	while (l->inicio!=NULL)
		l_retira(l);
}