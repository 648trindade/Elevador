#ifndef _TELA_H_
#define _TELA_H_

typedef struct elem{
	int valor;
	struct elem *prox;
}elem_t;

typedef struct{
	int qtd;
	elem_t *inicio;
}lista_t;

void l_cria(lista_t *l);

void l_insere(lista_t *l, int valor);

void l_insere_ord(lista_t *l, int valor, int sent);

int l_retira(lista_t *l);

void l_destroi(lista_t *l);

int l_vazio(lista_t *t);

#endif